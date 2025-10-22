//
//  waves.cpp
//  firstSketch
//
//  Created by Kevin Katz on 9/22/21.
//

#include "BaseWaves.hpp"

#include <math.h>

BaseWaves::BaseWaves(std::string name) : VisualForm(name) {
    parameters.add(glowIntensity.set("glowIntensity", 0.7, 0.1, 4.0));
    parameters.add(intensityAtEighthWidth.set("intensityAtEighthWidth", 0.667, 0, 1.0));
    parameters.add(blendMode.set("blendMode", 1, 0, 8));
    parameters.add(toneMap.set("toneMap", true));
}

void BaseWaves::drawUnit(ColorProvider & clr, KeyState & ks, DrawManager & dm, const Press & p, float alpha) {
    int index = p.note % NUM_NOTES;
    float halfOfTheWidthOfNotesZone = (ofGetWidth() / NUM_NOTES) / 2.0;
    float centerOfNotesZone = ofMap(index, 0, NUM_NOTES, 0, ofGetWidth()) + halfOfTheWidthOfNotesZone;
    // float noteScaler = 1.0; // ofMap(p.noteOverallPct(), 0, 1, 0.25, 1.0);
    float intensityScaler = 1.0;  // ofMap(p.velocityPct, 0, 1, 0.25, 1.0);
    float computedDampenRadius = getGlowDampenRatio(glowIntensity * ks.arousalGain(), intensityAtEighthWidth,
                                                    std::min(ofGetWidth(), ofGetHeight()) / 8.0);
    dm.shadeGlowLine(ofVec2f(centerOfNotesZone, 0), ofVec2f(centerOfNotesZone, ofGetHeight()), clr.color(p, alpha),
                     glowIntensity, computedDampenRadius, blendMode, toneMap);
}

void BaseWaves::update(KeyState & ks, ColorProvider & clr) {
    // nop
}

void BaseWaves::draw(KeyState & ks, ColorProvider & clr, DrawManager & dm) {
    ofPushStyle();

    for (const auto & press : ks.allPresses()) {
        double a = press.audibleAmplitudePct(ks.attackTimeS, ks.decayTimeS, ks.sustainLevelPct, ks.releaseTimeS) *
                   exponentialMap(press.velocityPct, 0, 1, 0, 1, true, 1 / 8.0);
        if (ofIsFloatEqual(a, 0.0)) {
            continue;
        }
        drawUnit(clr, ks, dm, press, a);
    }

    for (const auto & press : ks.allEphemeralPresses()) {
        if (ofIsFloatEqual(press.velocityPct, 0.0f)) {
            continue;
        }
        drawUnit(clr, ks, dm, press, pow(press.velocityPct, 0.2));
    }
    dm.shadeBlurX(ks.arousalGain());
    dm.shadeBlurY(ks.arousalGain());
    ofPopStyle();
}
