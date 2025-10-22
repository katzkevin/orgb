//
//  NoiseGrid.cpp
//  orgb
//
//  Created by Kevin Katz on 8/17/22.
//

#include "NoiseGrid.hpp"

#include <math.h>

#define PERIOD_S 1.0
#define NOISE_GRID_PRESS_OFFSET_PRIME 7919

NoiseGrid::NoiseGrid(std::string name) : Shape(name) {
    canvas.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR_ALPHA);

    glowIntensity.set(2.59);
    intensityAtEighthWidth.set(0.01);
    toneMap.set(false);
    drawMode.set(0);
    blurOffset.set(0);
    parameters.add(scaleFrequency.set("Scale Frequency", false));
    parameters.add(a4Frequency.set("A4 Frequency", 1.0, 0.1, 8));  // Per Shortside
    // parameters.add(topToBottomWaveLengthRatio.set("topToBottomWaveLengthRatio", 2.0, 1.0, 4.0));
    parameters.add(baseZVelocityPerSecond.set("Base Z Velocity / Second", 0.15, 0.001, 0.3));
    parameters.add(topToBottomZVelocityRatio.set("topToBottomPeriodRatio", 16.0, 1.0, 32.0));
    parameters.add(noiseAmplitude.set("Noise Amplitude", 0.5, 0.01, 1.0));
    parameters.add(resolution.set("resolution", 2, 1, 8));
}

void NoiseGrid::drawUnit(const ofColor & color, KeyState & ks, DrawManager & dm, Press & press) {
    ofPushStyle();
    ofPushMatrix();
    ofDisableDepthTest();
    ofDisableSmoothing();
    ofDisableAntiAliasing();

    double dt = getSystemTimeSecondsPrecise() - press.tSystemTimeSeconds;

    float shortSide = std::min(ofGetWidth(), ofGetHeight());
    // Higher note, faster zVelocity
    float zVelocityPerSecond = exponentialMap(press.noteOverallPct(), 0, 1, baseZVelocityPerSecond,
                                              baseZVelocityPerSecond * topToBottomZVelocityRatio, true, 2.0);

    // Higher note, shorter wavelengthsha
    // float waveLength = ofMap(press.noteOverallPct(), 1, 0, minWaveLength, minWaveLength *
    // topToBottomWaveLengthRatio);
    float frequency = NAN;

    if (scaleFrequency) {
        frequency = midiToHz(press.note) * a4Frequency;  // A4 has a4WaveLength
    } else {
        frequency = a4Frequency;
    }

    ofSetColor(color);

    if (!canvas.isAllocated()) {
        ofLogWarning() << "Why is canvas not allocated?";
    }

    canvas.setColor(ofColor(0));

    // WARNING: Do not treat particles' keys as up to date. They're saved at the moment they're inserted, so they never
    // see, for example, the invocation of a t_released. This is why shapes' release ADSR worked but particles' did not.
    //    while (it != allPressesEnd) {
    //        renderPixelsForPress(clr, ks, *it);
    //        it++;
    //    }
    //

    // WARNING: Do not treat particles' keys as up to date. They're saved at the moment they're inserted, so they never
    // see, for example, the invocation of a t_released. This is why shapes' release ADSR worked but particles' did not.
    int w = canvas.getWidth();
    int h = canvas.getHeight();
    float z = dt * zVelocityPerSecond;
    float pressOffset = press.id % NOISE_GRID_PRESS_OFFSET_PRIME;
    for (int i = 0; i < w; i += resolution) {
        for (int j = 0; j < h; j += resolution) {
            // Translate Z by p.note * 100 to collect different noise per note, you would never see the pattern
            // Scale by less-than-one noise amplitude to make space more black than colorful.
            float noise =
                ofNoise(i / shortSide * frequency + pressOffset, j / shortSide * frequency, z + press.note * 100.0) *
                noiseAmplitude;
            ofColor c = ofColor(color.r, color.g, color.b, color.a * noise);
            for (int k = 0; k < resolution; k++) {
                for (int l = 0; l < resolution; l++) {
                    canvas.getPixels().setColor(i + k, j + l, c);  // place your pixels
                }
            }
        }
    }

    canvas.update();

    ofSetColor(255);
    canvas.draw(0, 0);

    ofPopMatrix();
    ofPopStyle();
}
