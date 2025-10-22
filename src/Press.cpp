//
//  press.cpp
//  firstSketch
//
//  Created by Kevin Katz on 9/23/21.
//

#include "Press.hpp"

#include <math.h>

#include <iostream>

// For std::min and std::max (stupid)
#include <algorithm>

#include "Utilities.hpp"
#include "core/MathUtils.hpp"

std::ostream & operator<<(std::ostream & os, const Press & p) {
    os << "(" << p.note << ", " << p.velocityPct << ", " << p.tSystemTimeSeconds << ")";
    //    if (p.sustainTimeS.has_value()) {
    //        os << " (sustained @ " << ;
    //    }
    return os;
}

Press::Press(int n, float vPct, double pressTime, PressType pt, unsigned int messageId) {
    id = messageId;
    note = n;
    velocityPct = vPct;
    tSystemTimeSeconds = pressTime;
    pressType = pt;
}

std::optional<double> Press::getReleaseTime() const {
    if (!t_released.has_value()) {
        // Still held
        return std::nullopt;
    } else {
        // Released
        if (!sustainTimeS.has_value()) {
            // This was never sustained
            return t_released;
        } else {
            // This was, at some point sustained
            if (sustainReleasedTimeS.has_value()) {
                // This was released
                if (sustainReleasedTimeS.value() < t_released.value()) {
                    // Sustain was released before the key was released. No impact.
                    return t_released.value();
                } else {
                    // Sustain was released after the key was released
                    return sustainReleasedTimeS;
                }
            } else {
                // This released key is still sustained
                return std::nullopt;  // could also return sustainReleasedTimeS but that's less immediately obvious
            }
        }
    }
}

void Press::setReleased(double time) { t_released = std::optional<double>(time); }

void Press::setSustained(double timeS) {
    // NOTE: A key can only be sustained if it hasn't been released.
    if (!t_released.has_value()) {
        // This is still pressed
        // * If the press was already sustained, this will overwrite it with the new
        //   sustainTime
        sustainTimeS = std::optional<double>(timeS);
        sustainReleasedTimeS = std::nullopt;
    } else {
        // This is already released, sustaining does nothing
    }
}

void Press::releaseSustain(double timeS) {
    if (!sustainTimeS.has_value()) {
        // This isn't sustained. Do nothing.
    } else {
        // This is sustained.
        if (sustainReleasedTimeS.has_value()) {
            // This is us calling release on something already desustained. Do nothing
        } else {
            sustainReleasedTimeS = std::optional<double>(timeS);
        }
    }
}

// # Simple - 0
// Press Release

// # Single-sustain - 1
// Press Sustain Release Desustain
// Press Sustain Desustain Release
// Sustain Press Release Desustain
// Sustain Press Desustain Release

// # Second sustain after release (Use original sustain time) - Desustain 1
// Press Sustain Release Desustain Sustain2 ...
// Press Sustain Desustain Release Sustain2 ...
// Sustain Press Release Desustain Sustain2 ...
// Sustain Press Desustain Release Sustain2 ...

// # Second sustain before release, desustain before - 2
// Press Sustain Desustain Sustain2 Desustain2 Release
// Sustain Press Desustain Sustain2 Desustain2 Release
// Press Sustain Desustain Sustain2 Desustain2 Release
// Sustain Press Desustain Sustain2 Desustain2 Release

// # Second sustain before release, desustain after
// Press Sustain Desustain Sustain2 Release Desustain2
// Sustain Press Desustain Sustain2 Release Desustain2
// Press Sustain Desustain Sustain2 Release Desustain2
// Sustain Press Desustain Sustain2 Release Desustain2

float Press::noteOverallPct() const {
    // Return the [0,1) interval note value
    switch (pressType) {
        case PressType::GUITAR:
            return orgb::core::MathUtils::map(static_cast<float>(note), static_cast<float>(GUITAR_MIDI_MIN),
                                              static_cast<float>(GUITAR_MIDI_MAX), 0.0f, 1.0f, true);
        case PressType::PIANO:
        default:
            return orgb::core::MathUtils::map(static_cast<float>(note), static_cast<float>(PIANO_MIDI_MIN),
                                              static_cast<float>(PIANO_MIDI_MAX), 0.0f, 1.0f, true);
    }
}

float Press::noteChromaticPct() const {
    // Return the [0,1) interval note value
    return (note % NUM_NOTES) / static_cast<float>(NUM_NOTES);
}

double Press::audibleAmplitudePct(double attackTimeS, double decayTimeS, double sustainLevelPct,
                                  double releaseTimeS) const {
    /*

     This allows us to consider the release envelope of

     a press, considering the possibility that it is sustained.
     */

    double now = getSystemTimeSecondsPrecise();
    double dt = now - tSystemTimeSeconds;

    std::optional<double> dtReleased;
    if (getReleaseTime().has_value()) {  // Released
        dtReleased = std::optional<double>(now - getReleaseTime().value());
    } else {
        // Held
        dtReleased = std::nullopt;
    }

    double timeSpentAttackingS = NAN;
    double timeSpentDecayingS = NAN;
    double timeSpentReleasingS = NAN;

    if (dt < attackTimeS) {
        // Attack phase (or released before)

        if (dtReleased.has_value()) {
            // Released (during attack)
            timeSpentAttackingS = getReleaseTime().value() - tSystemTimeSeconds;
            timeSpentDecayingS = 0;
            timeSpentReleasingS = now - getReleaseTime().value();
            assert(timeSpentAttackingS < attackTimeS);
        } else {
            // Attacking
            timeSpentAttackingS = now - tSystemTimeSeconds;
            timeSpentDecayingS = 0.0;
            timeSpentReleasingS = 0.0;
        }

    } else if (dt < attackTimeS + decayTimeS) {
        // Decay phase (or released before)
        if (dtReleased.has_value()) {
            // Released (potentially during attack or decay)
            if (getReleaseTime().value() < tSystemTimeSeconds + attackTimeS) {
                // Released during attack
                timeSpentAttackingS = getReleaseTime().value() - tSystemTimeSeconds;
                timeSpentDecayingS = 0;
                timeSpentReleasingS = now - getReleaseTime().value();
                assert(timeSpentAttackingS < attackTimeS);
            } else {
                // Released during decay
                timeSpentAttackingS = attackTimeS;
                timeSpentDecayingS = getReleaseTime().value() - (tSystemTimeSeconds + attackTimeS);
                timeSpentReleasingS = now - getReleaseTime().value();
                assert(timeSpentDecayingS < decayTimeS);
            }
        } else {
            // Naturally decaying (no release)
            timeSpentAttackingS = attackTimeS;
            timeSpentDecayingS = now - (tSystemTimeSeconds + attackTimeS);
            timeSpentReleasingS = 0.0;
        }
    } else {
        // Sustain phase (or released before)
        if (dtReleased.has_value()) {
            // Released (potentially during attack, decay, or sustain)
            if (getReleaseTime().value() < tSystemTimeSeconds + attackTimeS) {
                // Released during attack
                timeSpentAttackingS = getReleaseTime().value() - tSystemTimeSeconds;
                timeSpentDecayingS = 0;
                timeSpentReleasingS = now - getReleaseTime().value();
                assert(timeSpentAttackingS < attackTimeS);
            } else if (getReleaseTime().value() < tSystemTimeSeconds + attackTimeS + decayTimeS) {
                // Released during decay
                timeSpentAttackingS = attackTimeS;
                timeSpentDecayingS = getReleaseTime().value() - (tSystemTimeSeconds + attackTimeS);
                timeSpentReleasingS = now - getReleaseTime().value();
                assert(timeSpentDecayingS < decayTimeS);
            } else {
                // Released during sustain
                timeSpentAttackingS = attackTimeS;
                timeSpentDecayingS = decayTimeS;
                timeSpentReleasingS = now - getReleaseTime().value();

                // timeSpentReleasingS cant be any value, as is the case when you hold a long release.
                // This assert is over-restrictive.
                // !! assert(timeSpentReleasingS < releaseTimeS);
            }
        } else {
            // Sustained
            timeSpentAttackingS = attackTimeS;
            timeSpentDecayingS = decayTimeS;
            timeSpentReleasingS = 0.0;
        }
    }

    //    float attackPctPerSecond = ofIsFloatEqual(attackTimeS, 0.0f) ? CGFLOAT_MAX : 1.0 / attackTimeS; // Attack
    //    Velocity float decayPctPerSecond = ofIsFloatEqual(decayTimeS, 0.0f) ? CGFLOAT_MAX : -((1.0 - sustainLevelPct)
    //    / decayTimeS);  // Decay Velocity float releasePctPerSecond = ofIsFloatEqual(releaseTimeS, 0.0f) ? CGFLOAT_MAX
    //    : -(sustainLevelPct / releaseTimeS);      // Release Velocity
    double attackComponent = NAN;
    double decayComponent = NAN;
    double releaseComponent = NAN;
    if (orgb::core::MathUtils::floatEqual(attackTimeS, 0.0)) {
        attackComponent = 1.0;
    } else {
        double attackVelocity = 1.0 / attackTimeS;
        attackComponent = attackVelocity * timeSpentAttackingS;
        assert(attackComponent <= 1.0);
    }
    if (orgb::core::MathUtils::floatEqual(decayTimeS, 0.0)) {
        if (!orgb::core::MathUtils::floatEqual(timeSpentDecayingS, 0.0)) {
            decayComponent = -(1.0 - sustainLevelPct);
        } else {
            decayComponent = 0.0;
        }
    } else {
        double decayVelocity = -(1.0 - sustainLevelPct) / decayTimeS;
        decayComponent = decayVelocity * timeSpentDecayingS;
        assert(-1.0 <= decayComponent && decayComponent <= 0.0);
    }
    if (orgb::core::MathUtils::floatEqual(releaseTimeS, 0.0)) {
        // Instead of releasing from the sustain level, calculate velocity from 1.0-mark. This avoids the 0.0001 sustain
        // super-slow velocity.
        if (!orgb::core::MathUtils::floatEqual(timeSpentReleasingS, 0.0)) {
            releaseComponent = -1.0;
        } else {
            releaseComponent = 0.0;
        }
    } else {
        double releaseVelocity = -1.0 / releaseTimeS;
        releaseComponent = releaseVelocity * timeSpentReleasingS;
        // Release can be less than -1, as is the case when a press has been released for a long time
        assert(releaseComponent <= 0.0);
    }
    return orgb::core::MathUtils::clamp(attackComponent + decayComponent + releaseComponent, 0.0, 1.0);
}

// float computeEnvelope(float dt, std::optional<float> dtReleased, float attackTimeSeconds, float releaseTimeSeconds)
// {
//    // Ease out exponential 3 on attack means aggressive entrance (60% after 0.25, 85% after 0.5)
//    float x;
//    if (attackTimeSeconds == 0) {
//        x = 1.0;
//    } else {
//        x = transitionEaseOutExponential(dt / attackTimeSeconds, 3);
//    }
//
//    if (!dtReleased.has_value()) {
//        return x;
//    } else {
//        float decay;
//        if (releaseTimeSeconds != 0.0) {
//            decay = 1 - transitionEaseOutExponential(dtReleased.get() / releaseTimeSeconds, 5.0);
//        } else {
//            decay = 0.0;
//        }
//        return decay * x;
//    }
//}
