//
//  Orbit.cpp
//  orgb
//
//  Created by Kevin Katz on 5/4/22.
//

#include "Orbit.hpp"

#include <math.h>

Orbit::Orbit(const std::string & name) : VisualForm(name) {
    parameters.add(glowIntensity.set("glowIntensity", 0.95, 0.5, 4.0));
    parameters.add(intensityAtEighthWidth.set("intensityAtEighthWidth", 0.927, 0, 1.0));
    parameters.add(blendMode.set("blendMode", 1, 0, 8));
    parameters.add(toneMap.set("toneMap", true));
    parameters.add(orbRadius.set("orbRadius", 4.0, 0.0, 50.0));
    parameters.add(rate.set("rate", 5, 0.1, 8));

    for (int i = 0; i < NUM_NOTES; i++) {
        particles.emplace(i, BaseParticle(ofVec3f(0, 0, 0), ofVec3f(0, 0, 0)));
    }
}

void Orbit::update(KeyState & ks, ColorProvider & clr) {
    float minSide = std::min(ofGetWidth(), ofGetHeight());
    ofVec3f center = ofVec3f(ofGetWidth() / 2, ofGetHeight() / 2, 0);

    for (auto it = particles.begin(); it != particles.end(); ++it) {
        float offset = it->first / static_cast<float>(NUM_NOTES) * TWO_PI;
        float theta = ofMap(ofGetFrameNum() % 60, 0, 60, 0, TWO_PI) + offset;
        float phi = ofMap(ofGetFrameNum() % 67, 0, 67, 0, TWO_PI) + offset;
        it->second.updatePosition(center + sphericalToRectangular(minSide, theta, phi));
    }
}

void Orbit::draw(KeyState & ks, ColorProvider & clr, DrawManager & dm) {
    ofPushStyle();

    float computedDampenRadius =
        getGlowDampenRatio(glowIntensity, intensityAtEighthWidth, std::min(ofGetWidth(), ofGetHeight()) / 8.0);

    std::multimap<int, Press> notePressTable;
    for (const auto & press : ks.allPresses()) {
        notePressTable.emplace(press.note % NUM_NOTES, press);
    }

    for (const auto & press : ks.allEphemeralPresses()) {
        notePressTable.emplace(press.note % NUM_NOTES, press);
    }

    for (int i = 0; i < NUM_NOTES; i++) {
        auto search = particles.find(i);
        if (search == particles.end()) {
            ofLogError() << "Missing key at particles: " << i;
            throw std::runtime_error("Couldn't find expected key");
        }
        BaseParticle particle = search->second;

        auto range = notePressTable.equal_range(i);
        for (auto it = range.first; it != range.second; ++it) {
            float alpha =
                it->second.audibleAmplitudePct(ks.attackTimeS, ks.decayTimeS, ks.sustainLevelPct, ks.releaseTimeS);
            dm.shadeGlowCircle(ofVec2f(particle.position.x, particle.position.y), orbRadius,
                               clr.color(it->second, alpha), glowIntensity, computedDampenRadius, blendMode, toneMap);
        }
    }

    ofPopStyle();
}
