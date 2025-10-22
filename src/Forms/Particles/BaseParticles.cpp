//
//  BaseParticles.cpp
//  orgb
//
//  Created by Kevin Katz on 2/23/22.
//

#include "BaseParticles.hpp"

#include <math.h>

// If someone's pressing more than 4, then start relaxing the number of particles coming out
#define CONCURRENT_PRESS_PARTICLE_BRAKE 4

BaseParticles::BaseParticles(const std::string & name) : VisualForm(name) {
    canvas.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR_ALPHA);

    parameters.add(particleRate.set("Particle Rate", 1000, 10, 8000));
    parameters.add(maxParticles.set("Max Particles", 3000, 500, 4000));
    parameters.add(initialVelocityLowerBound.set("Initial Velocity Lower Bound", 18, 1, 300));
    parameters.add(topToBottomInitialVelocityRatio.set("Top To Bottom Initial Velocity Ratio", 4, 1, 10));

    parameters.add(blurLowerLimit.set("Blur Lower Limit", 0, 0, 4));
    parameters.add(blurUpperLimit.set("Blur Upper Limit", 2, 0, 4));
    parameters.add(blurGain.set("Blur Gain", 1, 0.0, 4.0));
    parameters.add(noiseTemporalRate.set("Noise Temporal Rate", 1.01, 0, 2));
    parameters.add(noiseSpatialFrequency.set("Noise Spatial Frequency", 1, 0, 2));
    parameters.add(noiseVisualize.set("Noise Visualize", false));
    parameters.add(noiseScale.set("Noise Scale", 10, -4, 10));
}

void BaseParticles::update(KeyState & ks, ColorProvider & clr) {
    float arousalPct = ks.arousalPct();
    float timeS = ofGetElapsedTimef();
    float particleMultiplier = stof(getEnv("PARTICLE_MULTIPLIER", "1.0"));
    int generatingPressCount = 0;
    for (auto press : ks.activePresses()) {
        if (ofIsFloatEqual(
                press.audibleAmplitudePct(ks.attackTimeS, ks.decayTimeS, ks.sustainLevelPct, ks.releaseTimeS), 0.0)) {
            generatingPressCount++;
        }
    }

    for (auto press : ks.activePresses()) {
        double audibleAmplitude =
            press.audibleAmplitudePct(ks.attackTimeS, ks.decayTimeS, ks.sustainLevelPct, ks.releaseTimeS);
        // Use squareRoot so we favor new presses over decaying ones. Particles should fall off more rapidly than, say,
        // shapes.
        double squareRootOfAudibleAmplitude = exponentialMap(audibleAmplitude, 0, 1, 0, 1, true, 0.5);
        int numberOfParticlesToCreate =
            particleRate * ofGetLastFrameTime() * squareRootOfAudibleAmplitude * particleMultiplier +
            ofRandom(-0.5, 0.5);
        if (generatingPressCount > CONCURRENT_PRESS_PARTICLE_BRAKE) {
            numberOfParticlesToCreate = numberOfParticlesToCreate *
                                        (CONCURRENT_PRESS_PARTICLE_BRAKE / generatingPressCount) * particleMultiplier;
        }
        if (particles.size() > maxParticles) {
            numberOfParticlesToCreate =
                numberOfParticlesToCreate / (particles.size() / static_cast<float>(maxParticles));
        }
        // ofColor color = clr.color(press);
        ofColor color = ofColor(255, 255, 255);
        float hue;
        float saturation;
        float value;
        color.getHsb(hue, saturation, value);
        createParticlesForPress(press, numberOfParticlesToCreate, color, arousalPct);
    }
    for (auto press : ks.allEphemeralPresses()) {
        int numberOfParticlesToCreate =
            particleRate * ofGetLastFrameTime() * pow(press.velocityPct, 0.2) * particleMultiplier +
            ofRandom(-0.5, 0.5);
        if (particles.size() > maxParticles) {
            numberOfParticlesToCreate =
                numberOfParticlesToCreate / (particles.size() / static_cast<float>(maxParticles));
        }
        createParticlesForPress(press, numberOfParticlesToCreate, clr.color(press, 1.0), arousalPct);
    }
    pruneParticles();

    for (auto it = particles.begin(); it != particles.end(); ++it) {
        ofVec3f noiseGradientAtCoordinate = noiseGradientForCoordinates(
            it->second.position.x, it->second.position.y, noiseSpatialFrequency * (1 - ks.arousalGain()),
            noiseTemporalRate, noiseScale * (1 + ks.valenceGain()) / 2, timeS);
        ofVec3f acceleration =
            noiseScale > 0 ? ofVec3f(noiseGradientAtCoordinate.x, noiseGradientAtCoordinate.y, 0) : ofVec3f(0, 0, 0);
        it->second.update(ofGetLastFrameTime(), acceleration);  // Add some noise to each frame to avoid banding
    }

    for (const auto & press : ks.allPresses()) {
        float alpha = opacityForPress(ks, press);
        ofColor color = clr.color(press, alpha);
        for (auto er = particles.equal_range(press.id); er.first != er.second; er.first++) {
            er.first->second.setColor(color);
        }
    }
    //    for (const auto & press: ks.allEphemeralPresses()) {
    //        float alpha = opacityForEphemeralPress(ks, press);
    //        ofColor color = clr.color(press, alpha);
    //        // auto er = particles.equal_range(press.id)
    //        for (auto er = particles.equal_range(press.id); er.first != er.second; er.first++) {
    //            er.first->second.setColor(color);
    //        }
    //    }
}

void BaseParticles::draw(KeyState & ks, ColorProvider & clr, DrawManager & dm) {
    if (noiseVisualize) {
        drawNoiseVisualize(noiseSpatialFrequency, noiseTemporalRate, noiseScale, ofGetElapsedTimef());
        return;
    }

    ofPushStyle();
    // kkDisableAlphaBlending(); // Speeds things up.
    ofPushMatrix();
    //    ofSetLineWidth(1.0);
    //    ofDisableDepthTest();
    // Standard draw

    if (!canvas.isAllocated()) {
        ofLogWarning() << "Why is canvas not allocated?";
    }

    canvas.setColor(ofColor(0, 0, 0, 0));

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
    for (const auto & kv : particles) {
        int x = floor(kv.second.position.x);
        int y = floor(kv.second.position.y);
        if (0 <= x && x < w && 0 <= y && y < h) {
            canvas.getPixels().setColor(x, y, kv.second.color);  // place your pixels
        }
    }

    canvas.update();

    ofSetColor(255);
    canvas.draw(0, 0);

    ofPopMatrix();
    ofPopStyle();
    float blurFactorPct = 1 - ks.arousalPct();
    float blurFactorScaledByParams = ofMap(blurFactorPct, 0, 1, blurLowerLimit, blurUpperLimit);
    // Blurrier at low arousal, sharper at high
    // Gain means an isolated sprite (blackground) will be full brightness, not 0.382928 brightness
    dm.shadeBlurX(blurFactorScaledByParams, blurGain);
    dm.shadeBlurY(blurFactorScaledByParams, blurGain);  // Blurrier at low arousal, sharper at high
}

void BaseParticles::pruneParticles() {
    // https://stackoverflow.com/a/800984

    for (auto iter = particles.begin(); iter != particles.end();) {
        if (iter->second.position.x > ofGetWidth() || iter->second.position.x < 0 ||
            iter->second.position.y > ofGetHeight() || iter->second.position.y < 0 || iter->second.color.a < 0.0001) {
            iter = particles.erase(iter);
        } else {
            ++iter;
        }
    }
}

ofVec3f BaseParticles::startPositionForPress(const Press & p) {
    ofLogWarning("This shouldn't be called hmm.");
    return ofVec3f(5, 5, 0);
}

// Override these two methods depending on if particles should disappear. For random, they will
// disappear based on audibleAmplitudePct
float BaseParticles::opacityForEphemeralPress(KeyState & ks, const Press & p) const { return 1.0; }

float BaseParticles::opacityForPress(KeyState & ks, const Press & p) const { return 1.0; }
