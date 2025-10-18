//
//  RandomParticles.cpp
//  orgb
//
//  Created by Kevin Katz on 5/27/22.
//

#include "RandomParticles.hpp"

RandomParticles::RandomParticles(std::string name) : BaseParticles(name) {
    particleRate.set(2000);
    initialVelocityLowerBound.set(40.0);
    blurGain.set(2.75);
    blurLowerLimit.set(1);
    blurUpperLimit.set(2);
}

ofVec3f RandomParticles::startPositionForPress(const Press & p) {
    int xPos = ofRandomWidth();
    // Subtly pertub to avoid initial streak
    int yPos = ofRandomHeight();
    return ofVec3f(xPos, yPos, 0);
}

void RandomParticles::createParticlesForPress(Press & press, int numberOfParticlesToCreate, ofColor c,
                                              float arousalPct) {
    // int xPos = ofMap(kv.second.note % NUM_NOTES, 0, NUM_NOTES, 0, ofGetWidth());
    float arousalModifier = ofMap(arousalPct, 0, 1, 0.5, 2.0);
    for (int i = 0; i < numberOfParticlesToCreate; i++) {
        // Perturb +/- 0.5 to accommodate for the more keys than particles in a frame, situation
        float angle = ofMap(ofRandomf(), -1, 1, 0, TWO_PI);  // + PI/2 because it's centered about PI/2 (down)
        float velocity = ofMap(press.note, GUITAR_MIDI_MIN, GUITAR_MIDI_MAX, initialVelocityLowerBound,
                               initialVelocityLowerBound * topToBottomInitialVelocityRatio, true) *
                         arousalModifier;
        float vx = cos(angle) * velocity;
        float vy = sin(angle) * velocity;

        Particle myParticle(startPositionForPress(press), ofVec3f(vx, vy, 0), c);
        particles.emplace(press.id, myParticle);
    }
}

// Override these two methods depending on if particles should disappear
float RandomParticles::opacityForEphemeralPress(KeyState & ks, const Press & p) const { return p.velocityPct; }

float RandomParticles::opacityForPress(KeyState & ks, const Press & p) const {
    return p.audibleAmplitudePct(ks.attackTimeS, ks.decayTimeS, ks.sustainLevelPct, ks.releaseTimeS);
}
