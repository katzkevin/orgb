//
//  MeshGrid.cpp
//  orgb
//
//  Created by Kevin Katz on 8/17/22.
//

#include "MeshGrid.hpp"


#define PERIOD_S 1.0

MeshGrid::MeshGrid(std::string name) : Shape(name) {
    glowIntensity.set(2.59);
    intensityAtEighthWidth.set(0.01);
    toneMap.set(false);
    drawMode.set(0);
    blurOffset.set(0.5);
    parameters.add(scaleWavelength.set("scaleWaveLength", false));
    parameters.add(a4WaveLength.set("a4WaveLength", 6, 1, 10));
    // parameters.add(topToBottomWaveLengthRatio.set("topToBottomWaveLengthRatio", 2.0, 1.0, 4.0));
    parameters.add(minPeriodSeconds.set("minPeriodSeconds", 1.0, 1/60.0, 3.0));
    parameters.add(topToBottomPeriodRatio.set("topToBottomPeriodRatio", 16.0, 1.0, 32.0));
}

void MeshGrid::drawUnit(const ofColor &color, KeyState & ks, DrawManager &dm, Press &press) {
    ofPushStyle();
    ofPushMatrix();
    ofDisableDepthTest();
    ofDisableSmoothing();
    ofDisableAntiAliasing();
    
    double dt = getSystemTimeSecondsPrecise() - press.tSystemTimeSeconds;
    
    float w = ofGetWidth();
    float h = ofGetHeight();
    float longer = sqrt(w * w + h * h);
    
    // Higher note, faster period
    float period = exponentialMap(press.noteOverallPct(), 1, 0, minPeriodSeconds, minPeriodSeconds * topToBottomPeriodRatio, true, 2.0);
    
    // Higher note, shorter wavelengthsha
    // float waveLength = ofMap(press.noteOverallPct(), 1, 0, minWaveLength, minWaveLength * topToBottomWaveLengthRatio);
    float waveLength;
    
    if (scaleWavelength) {
        waveLength = 1.0 / midiToHz(press.note) * 440.0 * a4WaveLength; // A4 has a4WaveLength
    } else {
        waveLength = a4WaveLength;
    }
    
    ofRotateRad(deterministicRandomPct(press.id) * TWO_PI);
    // Translate up by offset
    float offsetPct = positive_modulo(static_cast<float>(dt), period) / period;
    ofTranslate(0, offsetPct * waveLength);
    // ofLogNotice() << "note:" << press.note << " pressOveallPct" << press.noteOverallPct() << " minPeriodSeconds:" << minPeriodSeconds << " maxPeriodSeconds:" << (minPeriodSeconds * topToBottomPeriodRatio) << " period:" << period << " offsetPct:" << offsetPct;
    
    ofSetColor(color);
    
    ofDrawLine(-longer, 0, longer, 0);
    float offset = waveLength;
    
    while (offset <= longer) {

        ofDrawLine(-longer, offset, longer, offset);
        ofDrawLine(-longer, -offset, longer, -offset);
    

//            float functionalGlowIntensity = glowIntensity * ks.arousalGain();
//            float computedDampenRadius = getGlowDampenRatio(glowIntensity, intensityAtEighthWidth, std::min(ofGetWidth(), ofGetHeight()) / 8.0);
//
//            dm.shadeGlowLine(ofVec2f(-longer, offset), ofVec2f(longer, offset), color, functionalGlowIntensity, computedDampenRadius, blendMode, toneMap);
//            dm.shadeGlowLine(ofVec2f(-longer, -offset), ofVec2f(longer, -offset), color, functionalGlowIntensity, computedDampenRadius, blendMode, toneMap);

        offset += waveLength;
    }
    
    ofPopMatrix();
    ofPopStyle();
}
