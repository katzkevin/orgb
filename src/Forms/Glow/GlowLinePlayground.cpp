//
//  LineSegmentGlow.cpp
//  orgb
//
//  Created by Kevin Katz on 4/29/22.
//

#include "GlowLinePlayground.hpp"

GlowLinePlayground::GlowLinePlayground(std::string name) : VisualForm(name) {
    parameters.add(glowIntensity.set("glowIntensity", 1.5, 0.5, 4.0));
    parameters.add(intensityAtEighthWidth.set("intensityAtEighthWidth", 0.01, 0, 0.05));

    parameters.add(aAlpha.set("aAlpha", 1, 0, 1));
    parameters.add(length.set("length", 4, 1, 64));
    parameters.add(slope.set("slope", 0, 0, 1));
    parameters.add(velocity.set("velocity", 0.1, 0, 4));

    parameters.add(blendMode.set("blendMode", 1, 0, 8));
    parameters.add(toneMap.set("toneMap", true));
}

void GlowLinePlayground::draw(KeyState & ks, ColorProvider & clr, DrawManager & dm) {
    ofPushStyle();

    float computedDampenRadius =
        getGlowDampenRatio(glowIntensity, intensityAtEighthWidth, std::min(ofGetWidth(), ofGetHeight()) / 8.0);
    ofPushMatrix();

    ofVec2f center = ofVec2f(ofGetWidth() / 2, ofGetHeight() / 2);

    //    dm.shadeGlowCircle(center + ofVec2f(-15, -200), a0, ofColor(255, 0, 0), glowIntensity, computedDampenRadius,
    //    blendMode, toneMap); dm.shadeGlowCircle(center + ofVec2f(15, -200), a0, ofColor(0, 255, 0), glowIntensity,
    //    computedDampenRadius, blendMode, toneMap);
    //
    //    dm.shadeGlowCircle(center + ofVec2f(-15, -150), a0, ofColor(0, 255, 0), glowIntensity, computedDampenRadius,
    //    blendMode, toneMap); dm.shadeGlowCircle(center + ofVec2f(15, -150), a0, ofColor(0, 0, 255), glowIntensity,
    //    computedDampenRadius, blendMode, toneMap);
    //
    //    dm.shadeGlowCircle(center + ofVec2f(-15, -100), a0, ofColor(0, 0, 255), glowIntensity, computedDampenRadius,
    //    blendMode, toneMap); dm.shadeGlowCircle(center + ofVec2f(15, -100), a0, ofColor(255, 0, 0), glowIntensity,
    //    computedDampenRadius, blendMode, toneMap);
    //
    //    dm.shadeGlowCircle(center + ofVec2f(-15, -50), a0, ofColor(255, 255, 0), glowIntensity, computedDampenRadius,
    //    blendMode, toneMap); dm.shadeGlowCircle(center + ofVec2f(15, -50), a0, ofColor(0, 255, 255), glowIntensity,
    //    computedDampenRadius, blendMode, toneMap);
    //
    //    dm.shadeGlowCircle(center + ofVec2f(-15, 0), a0, ofColor(0, 255, 255), glowIntensity, computedDampenRadius,
    //    blendMode, toneMap); dm.shadeGlowCircle(center + ofVec2f(15, 0), a0, ofColor(255, 0, 255), glowIntensity,
    //    computedDampenRadius, blendMode, toneMap);
    //
    //    dm.shadeGlowCircle(center + ofVec2f(-15, 50), a0, ofColor(255, 0, 255), glowIntensity, computedDampenRadius,
    //    blendMode, toneMap); dm.shadeGlowCircle(center + ofVec2f(15, 50), a0, ofColor(255, 255, 0), glowIntensity,
    //    computedDampenRadius, blendMode, toneMap);
    //
    //    dm.shadeGlowCircle(center + ofVec2f(-15, 100), a0, ofColor(128, 0, 255), glowIntensity, computedDampenRadius,
    //    blendMode, toneMap); dm.shadeGlowCircle(center + ofVec2f(15, 100), a0, ofColor(0, 128, 255), glowIntensity,
    //    computedDampenRadius, blendMode, toneMap);
    //
    //    dm.shadeGlowCircle(center + ofVec2f(-15, 150), a0, ofColor(0, 255, 128), glowIntensity, computedDampenRadius,
    //    blendMode, toneMap); dm.shadeGlowCircle(center + ofVec2f(15, 150), a0, ofColor(128, 255, 0), glowIntensity,
    //    computedDampenRadius, blendMode, toneMap);
    //
    //    dm.shadeGlowCircle(center + ofVec2f(-15, 200), a0, ofColor(255, 128, 0), glowIntensity, computedDampenRadius,
    //    blendMode, toneMap); dm.shadeGlowCircle(center + ofVec2f(15, 200), a0, ofColor(255, 0, 128), glowIntensity,
    //    computedDampenRadius, blendMode, toneMap);

    //    dm.shadeGlowCircle(center, slope, ofColor(255, 0, 0), glowIntensity, computedDampenRadius, blendMode,
    //    toneMap);
    ofVec3f center3(center.x, center.y, 0);
    ofVec3f direction(0, length, length * slope);
    dm.shadeGlowLine(center3, center3 + direction.getNormalized() * length, ofColor(255, 0, 0, 255 * aAlpha),
                     glowIntensity, computedDampenRadius, blendMode, toneMap);

    ofPopMatrix();

    ofPopStyle();
}
