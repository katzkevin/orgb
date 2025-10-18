//
//  KeyState.hpp
//  orgb
//
//  Created by Kevin Katz on 10/13/21.
//

#ifndef KeyState_hpp
#define KeyState_hpp

#include <stdio.h>

#include <list>
#include <unordered_map>

#include "Press.hpp"
#include "ofMain.h"

class KeyState {
   public:
    KeyState();
    virtual ~KeyState() {};

    void cleanup(float ttlSecondsAfterRelease, unsigned int currentFrame, double deltaTime);
    void keyReleasedHandler(int key);

    bool isActivelyPressed(int key);
    Press newKeyPressedHandler(int key, float velocityPct, unsigned int messageId);
    void ephemeralKeyPressMapHandler(std::unordered_map<int, float> incomingPresses,
                                     std::unordered_map<int, unsigned int> messageIds);
    void ephemeralKeyPressedHandler(int key, float velocityPct, unsigned int messageId);

    std::optional<Press> getActivePress(int key);
    std::optional<Press> getMostRecentPress();

    std::list<Press> presses;
    const std::list<Press> & allPresses();
    const std::multimap<int, Press> allPressesChromaticGrouped();
    std::list<Press> activePresses();

    std::unordered_map<int, Press> ephemeralPresses;
    void decayEphemeralKeypressAmplitudes(double deltaTime);

    const std::list<Press> allEphemeralPresses();
    const std::multimap<int, Press> allEphemeralPressesChromaticGrouped();

    std::optional<double> sustainTimeS;
    void sustainOnHandler(double timeSeconds);
    void sustainOffHandler(double timeSeconds);

    // Return True iff sequential c, c#, d, d#, e are pressed
    std::optional<int> getMetaInput();

    float valencePct() const;
    float arousalPct() const;
    void setArousalPct(float arousalPct);
    void setValencePct(float valencePct);
    void circumplexHomeostasis();

    float arousalGain() const;
    float valenceGain() const;

    unsigned int randomSeed;

    ofParameter<double> attackTimeS;
    ofParameter<double> decayTimeS;
    ofParameter<double> sustainLevelPct;
    ofParameter<double> releaseTimeS;
    ofParameter<double> ephemeralDecayPerS;
    ofParameter<float> arousalKurtosis;
    ofParameter<float> valenceKurtosis;

   private:
    float arousal;
    std::optional<float> arousalLastUpdateS;
    float valence;
    std::optional<float> valenceLastUpdateS;
};

#endif /* KeyState_hpp */
