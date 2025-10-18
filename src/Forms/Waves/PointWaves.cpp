//
//  PointWaves.cpp
//  orgb
//
//  Created by Kevin Katz on 5/4/22.
//

#include "PointWaves.hpp"

PointWaves::PointWaves(std::string name) : BaseWaves(name) {
    glowIntensity.set(1.68);
    intensityAtEighthWidth.set(0.68);
    blendMode.set(1);
}

void PointWaves::drawUnit(ColorProvider &clr, KeyState & ks, DrawManager &dm, const Press &p, float alpha) {
    int index = p.note % NUM_NOTES;
    float numOctaves = ceil((PIANO_MIDI_MAX - PIANO_MIDI_MIN) / 12.0); // (vertical columns)
    float intensityModulator = exponentialMap(alpha, 0, 1, 1 / GOLDEN_RATIO, GOLDEN_RATIO);
    float modulatedIntensity = intensityModulator * glowIntensity;
    float computedDampenRadius = getGlowDampenRatio(modulatedIntensity, intensityAtEighthWidth, std::min(ofGetWidth(), ofGetHeight()) / 8.0);
    float halfZoneHeight = (ofGetHeight() / NUM_NOTES) / 2.0;
    
    float halfZoneWidth = (ofGetWidth() / NUM_NOTES) / 2.0;
    int centerY = ofMap(index, 0, NUM_NOTES, 0, ofGetHeight()) + halfZoneHeight;
    int ithColumn = (p.note - PIANO_MIDI_MIN) / 12;
    float xCoord = ofMap(ithColumn, 0, numOctaves, 0, ofGetWidth()) + halfZoneWidth;
    ofColor pressColor = clr.color(p, alpha);
    dm.shadeGlowCircle(ofVec2f(xCoord, centerY), std::min(halfZoneWidth, halfZoneHeight), pressColor, modulatedIntensity, computedDampenRadius, blendMode, toneMap);
    // ofDrawPoint(horizontalCenterOfNotesZone - halfOfTheWidthOfNotesZone, ofGetHeight() / 2.0, halfOfTheWidthOfNotesZone, 10);
}
