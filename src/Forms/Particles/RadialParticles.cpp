//
//  RadialParticles.cpp
//  orgb
//
//  Created by Kevin Katz on 2/23/22.
//

#include "RadialParticles.hpp"

#include <math.h>

#define TARGET_FRAME_RATE 60.0

RadialParticles::RadialParticles(std::string name) : BaseParticles(name) {
    particleRate.set(1500);
    initialVelocityLowerBound.set(32);
    blurLowerLimit.set(0);
    blurUpperLimit.set(2);
    noiseSpatialFrequency.set(1);
    noiseScale.set(10);
}

ofVec3f RadialParticles::startPositionForPress(const Press & p) {
    return ofVec3f(ofGetWidth() / 2.0, ofGetHeight() / 2.0, 0);
}

void RadialParticles::createParticlesForPress(Press & press, int numberOfParticlesToCreate, ofColor c,
                                              float arousalPct) {
    // int xPos = ofMap(kv.second.note % NUM_NOTES, 0, NUM_NOTES, 0, ofGetWidth());
    float arousalModifier = ofMap(arousalPct, 0, 1, 0.5, 2.0);
    for (int i = 0; i < numberOfParticlesToCreate; i++) {
        // Perturb +/- 0.5 to accommodate for the more keys than particles in a frame, situation
        // Subtly perturb to avoid streaking (ofRandom multiply)
        float velocity = ofMap(press.note, GUITAR_MIDI_MIN, GUITAR_MIDI_MAX, initialVelocityLowerBound,
                               initialVelocityLowerBound * topToBottomInitialVelocityRatio, true) *
                         ofRandom(0.8, 1.2) * arousalModifier;
        ofVec3f initialVelocity = randomUnitVector2D() * velocity;
        // ofVec3f scoot = (1.0 / TARGET_FRAME_RATE * ofRandom(0, 0.001)) * initialVelocity; // In order to avoid
        // banding at the start point, scoot a bit into the framerate
        Particle myParticle(startPositionForPress(press), initialVelocity, c);
        particles.emplace(press.id, myParticle);
    }
}
