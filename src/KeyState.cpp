//
//  KeyState.cpp
//  orgb
//
//  Created by Kevin Katz on 10/13/21.
//

#include "KeyState.hpp"

#include <math.h>

#include "Utilities.hpp"
#include "core/Random.hpp"

// #define EXPONENTIAL_DECAY_CONSTANT -0.001 // Slow decay
#define SECONDS_TO_DECAY 14
#define PRUNE_BELOW_VELOCITY (1 / 128.0)
// For example, 1 second gives approx. 4.85, 2 seconds is approx. 2.42
#define EXPONENTIAL_DECAY_CONSTANT log(PRUNE_BELOW_VELOCITY) / SECONDS_TO_DECAY

#define WARN_IF_PRESS_LIST_LARGER_THAN_SIZE 100

#define MAX_SECONDS_PRESSED 15
#define ALLOW_NOTE_INCREASE_PCT 0.20

KeyState::KeyState() : arousal(0.5), valence(0.5) {
    randomSeed = orgb::core::Random::generateSeed();
    //    attackTimeS.set("Attack Time S", 0, 0, 0.2);
    //    decayTimeS.set("Decay Time S", 3.0, 0, 5);
    //    sustainLevelPct.set("Sustain Level Percent", 1, 0, 1);
    //    releaseTimeS.set("Release Time S", 0.17, 0, 2.0);
    // Olga Piano
    attackTimeS.set("Attack Time S", 0, 0, 0.2);
    decayTimeS.set("Decay Time S", 0.22, 0, 5);
    sustainLevelPct.set("Sustain Level Percent", 0.57, 0, 1);
    releaseTimeS.set("Release Time S", 0.65, 0, 2.0);
    ephemeralDecayPerS.set("Ephemeral Decay Per S", 0.8, 0.6, 1.0);
    arousalKurtosis.set("Valence Kurtosis", 1, 0, 4);
    valenceKurtosis.set("Arousal Kurtosis", 1, 0, 4);
}

std::optional<Press> KeyState::getActivePress(int key) {
    // TODO Potential consideration If a sustain is invoked, we would want two ? Perhaps not
    for (auto press : presses) {
        if (press.note == key && !press.getReleaseTime().has_value()) {
            // There is already a press, not released.
            return std::optional<Press>(press);
        }
    }
    return std::nullopt;
}

std::optional<Press> KeyState::getMostRecentPress() {
    std::optional<Press> mostRecent = std::nullopt;
    for (auto press : presses) {
        if (!mostRecent.has_value() || press.tSystemTimeSeconds > mostRecent.value().tSystemTimeSeconds) {
            mostRecent = std::optional<Press>(press);
        }
    }
    return mostRecent;
}

bool KeyState::isActivelyPressed(int key) { return KeyState::getActivePress(key).has_value(); }

void KeyState::ephemeralKeyPressMapHandler(std::unordered_map<int, float> incomingPresses,
                                           std::unordered_map<int, unsigned int> messageIds) {
    std::set<int> pressQueue;
    for (auto & pair : incomingPresses) {
        pressQueue.insert(pair.first);
    }
    for (const auto pair : incomingPresses) {
        unsigned int messageId = messageIds[pair.first];
        ephemeralKeyPressedHandler(pair.first, pair.second, messageId);
        pressQueue.erase(pair.first);
    }
}

void KeyState::ephemeralKeyPressedHandler(int key, float velocityPct, unsigned int messageId) {
    auto result = ephemeralPresses.find(key);

    if (result == ephemeralPresses.end() && velocityPct != 0) {  // Stupid fucking presence chess in c++ map
        double now = getSystemTimeSecondsPrecise();
        Press p = Press(key, velocityPct, now, Press::PressType::GUITAR, messageId);
        p.setReleased(now);  // Immediately released-- release begins right away.
        ephemeralPresses.emplace(key, p);
    } else {
        if (velocityPct < PRUNE_BELOW_VELOCITY) {
            // The new velocity suggests that this should be deleted
            ephemeralPresses.erase(key);
            assert(ephemeralPresses.find(key) == ephemeralPresses.end());
        } else if (velocityPct < result->second.velocityPct + ALLOW_NOTE_INCREASE_PCT) {
            // The press is already here, decaying (increase < threshold)
            result->second.velocityPct = velocityPct;
            // Do not change ID
        } else {
            // If the velocity goes up by >= ALLOW_NOTE_INCREASE_PCT, it's not a sustain, it's a new note
            double now = getSystemTimeSecondsPrecise();
            Press p = Press(key, velocityPct, now, Press::PressType::GUITAR, messageId);
            p.setReleased(now);
            ephemeralPresses.erase(key);
            ephemeralPresses.emplace(key, p);
        }
    }
}

// If metaInputMode, return the note of the fundamental C.
// e.g. if C4, C#4, D4, D#4, E4 enabled, return 60.
// e.g. if C6, C#6, D6, D#6, E6 enabled, return 84.
std::optional<int> KeyState::getMetaInput() {
    std::vector<int> allPressedKeys;
    for (const auto & press : activePresses()) {
        allPressedKeys.push_back(press.note);
    }
    std::sort(allPressedKeys.begin(), allPressedKeys.end());

    // Size 9,
    // 0   1   2   3   4   5   6   7   8
    // f2  c3  g6  c7  c8 c#8  d8  d#8 e8
    for (int i = 0; i + 4 < static_cast<int>(allPressedKeys.size()); i++) {
        int thisNote = allPressedKeys[i];
        if (thisNote % NUM_NOTES == 0 &&              // This is a C
            allPressedKeys[i + 1] == thisNote + 1 &&  // Next is C#
            allPressedKeys[i + 2] == thisNote + 2 &&  // Next is D
            allPressedKeys[i + 3] == thisNote + 3 &&  // Next is D#
            allPressedKeys[i + 4] == thisNote + 4) {  // Next is E
            return std::optional<int>(thisNote);
        }
    }
    return std::nullopt;
}

Press KeyState::newKeyPressedHandler(int key, float velocityPct, unsigned int messageId) {
    if (getActivePress(key).has_value()) {
        // This should not have been invoked.
        ofLogWarning() << key << " already pressed.";
    }
    Press p = Press(key, velocityPct, getSystemTimeSecondsPrecise(), Press::PressType::PIANO, messageId);
    if (sustainTimeS.has_value()) {
        // Pressed while sustain pedal held.
        p.setSustained(sustainTimeS.value());
    }
    presses.push_back(p);

    return p;
}

void KeyState::keyReleasedHandler(int key) {
    for (auto & press : presses) {
        // Why check getReleaseTime? Because the keyreleasehandler is multiply invoked while held down for multiple
        // frames.
        if (press.note == key && !press.getReleaseTime().has_value()) {
            // if (press.note == key) {
            // This continues in order to catch the potential (and ideally impossible) case
            // of multiple unreleased presses of the same key.g
            press.setReleased(getSystemTimeSecondsPrecise());
        }
    }
}

const std::list<Press> & KeyState::allPresses() { return presses; }

const std::multimap<int, Press> KeyState::allPressesChromaticGrouped() {
    std::multimap<int, Press> grouped;
    for (const auto & press : allPresses()) {
        grouped.emplace(press.note % NUM_NOTES, press);
    }
    return grouped;
}

std::list<Press> KeyState::activePresses() {
    std::list<Press> emit;
    for (auto elt : presses) {
        if (!elt.getReleaseTime().has_value()) {
            // If this is sustained, getReleaseTime returns none and we skip
            emit.push_back(elt);
        }
    }
    return emit;
}

void KeyState::cleanup(float ttlSecondsAfterRelease, unsigned int currentFrame, double deltaTime) {
    decayEphemeralKeypressAmplitudes(deltaTime);

    if (currentFrame % 100 == 0 && (presses.size() > WARN_IF_PRESS_LIST_LARGER_THAN_SIZE ||
                                    ephemeralPresses.size() > WARN_IF_PRESS_LIST_LARGER_THAN_SIZE)) {
        ofLogVerbose("KeyState") << "Presses overload: presses.size()=" << presses.size()
                                 << " / ephemeralPresses.size()=" << ephemeralPresses.size();
    }

    double now = getSystemTimeSecondsPrecise();

    for (auto it = presses.begin(); it != presses.end();) {
        if (!it->getReleaseTime().has_value() && now - it->tSystemTimeSeconds > MAX_SECONDS_PRESSED) {
            // If the press has been held (and not released), it's possible we never received a key-released message
            // from our midi element.
            ofLogNotice("KeyState") << it->note << " has been enabled for more than " << MAX_SECONDS_PRESSED
                                    << " seconds, forcing a release.";
            it->setReleased(getSystemTimeSecondsPrecise());
        }

        if (it->getReleaseTime().has_value() && now - it->getReleaseTime().value() > ttlSecondsAfterRelease) {
            it = presses.erase(it);
        } else {
            ++it;
        }
    }
}

void KeyState::decayEphemeralKeypressAmplitudes(double deltaTime) {
    double now = getSystemTimeSecondsPrecise();
    std::list<int> keysToRemove;

    double dt = deltaTime;

    // 0.8 ^ (1s/60) = 0.99628,   0.99628 ^ 60 = 0.8
    // 0.8 ^ (2s) = 0.64,   0.64 ^ 1/2 = 0.8
    double decay = pow((1 - ephemeralDecayPerS), dt);

    for (auto & pair : ephemeralPresses) {
        double decayedVelocity = decay * pair.second.velocityPct;

        pair.second.velocityPct = decayedVelocity;
        pair.second.tSystemTimeSeconds = now;

        if (decayedVelocity < PRUNE_BELOW_VELOCITY) {
            keysToRemove.push_back(pair.first);
        }
    }

    for (auto key : keysToRemove) {
        ephemeralPresses.erase(key);
    }
}

const std::multimap<int, Press> KeyState::allEphemeralPressesChromaticGrouped() {
    std::multimap<int, Press> grouped;
    for (const auto & press : allEphemeralPresses()) {
        grouped.emplace(press.note % NUM_NOTES, press);
    }
    return grouped;
}

const std::list<Press> KeyState::allEphemeralPresses() {
    std::list<Press> presses;

    for (auto const & pair : ephemeralPresses) {
        presses.push_back(pair.second);
    }

    return presses;
}

void KeyState::sustainOnHandler(double timeSeconds) {
    sustainTimeS = std::optional<double>(timeSeconds);
    for (auto & press : presses) {
        // Press will ignore this if it is released
        press.setSustained(timeSeconds);
    }
}

void KeyState::sustainOffHandler(double timeSeconds) {
    sustainTimeS = std::nullopt;
    for (auto & press : presses) {
        press.releaseSustain(timeSeconds);
    }
}

#define CIRCUMPLEX_HOMEOSTASIS_PER_SECOND 0.1

void KeyState::circumplexHomeostasis() {
    double now = getSystemTimeSecondsPrecise();
    if (arousalLastUpdateS.has_value() && abs(arousal - 0.5) > 0.0001) {
        // arousal is already level
        float arousalDt = now - arousalLastUpdateS.value();
        if (arousal > 0.5) {
            arousal = std::max(0.5, arousal - arousalDt * CIRCUMPLEX_HOMEOSTASIS_PER_SECOND);
        } else {
            arousal = std::min(0.5, arousal + arousalDt * CIRCUMPLEX_HOMEOSTASIS_PER_SECOND);
        }
        arousalLastUpdateS = std::optional<float>(now);
    }
    if (valenceLastUpdateS.has_value() && abs(valence - 0.5) > 0.0001) {
        float valenceDt = now - valenceLastUpdateS.value();
        if (valence > 0.5) {
            valence = std::max(0.5, valence - valenceDt * CIRCUMPLEX_HOMEOSTASIS_PER_SECOND);
        } else {
            valence = std::min(0.5, valence + valenceDt * CIRCUMPLEX_HOMEOSTASIS_PER_SECOND);
        }
        valenceLastUpdateS = std::optional<float>(now);
    }

    // 0.98^300 = 0.0023. 300 frames is 5 seconds at 60fps
    // TODO THis is not stable over frame rate =/= 60.
    // For each frame, 98% arousal, 2% towards 0.5. This gets us 0.002 after 5 seconds

    // 0.98^60 -> 0.29
    // 0.29^5 = 0.00205 also
    // 0.29^(1/60) = 0.97958013
    // 0.29^(1/2) = 0.53851648
    // 0.29^(5) = 0.002
}

void KeyState::setArousalPct(float arousalPct) {
    arousal = arousalPct;
    ofLogVerbose("KeyState") << "Arousal set to " << arousal;
    arousalLastUpdateS = std::optional<float>(getSystemTimeSecondsPrecise());
}

float KeyState::arousalPct() const { return arousal; }

void KeyState::setValencePct(float valencePct) {
    valence = valencePct;
    ofLogVerbose("KeyState") << "Valence set to " << valence;
    valenceLastUpdateS = std::optional<float>(getSystemTimeSecondsPrecise());
}

float KeyState::valencePct() const { return valence; }

float KeyState::arousalGain() const { return pctToGain(arousalPct(), arousalKurtosis); }

float KeyState::valenceGain() const { return pctToGain(valencePct(), valenceKurtosis); }
