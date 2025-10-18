//
//  Field.cpp
//  orgb
//
//  Created by Kevin Katz on 9/29/21.
//

#include "Field.hpp"

Field::Field(std::string name) : VisualForm(name) {
    width = 32;
    height = 160;
    depth = std::min(width, height);
}

void Field::spriteColorChanged(ofColor & c) {
    int i = 0;
    for (auto it = flock.l.begin(); it != flock.l.end(); ++it) {
        it->setColor(generateNoisySpriteColor(++i));
    }
}
void Field::spriteColorChangedFloatParam(float & f) {
    int i = 0;
    for (auto it = flock.l.begin(); it != flock.l.end(); ++it) {
        it->setColor(generateNoisySpriteColor(++i));
    }
}

void Field::spriteSizeChanged(float & s) {
    int i = 0;
    for (auto it = flock.l.begin(); it != flock.l.end(); ++it) {
        it->setSize(generateNoisySpriteSize(++i));
    }
}
void Field::spriteVariationChanged(float & v) {
    int i = 0;
    for (auto it = flock.l.begin(); it != flock.l.end(); ++it) {
        it->setSize(generateNoisySpriteSize(++i));
    }
}

void Field::setup() {
    parameters.add(drawStyle.set("drawStyle", 0, 0, 2));

    int maxPopulation = 512;

    parameters.add(population.set("population", 256, 10, maxPopulation));
    parameters.add(fastForward.set("fastForward", 1.0, 0.1, 4.0));

    parameters.add(minVelocity.set("minVelocity", 28, 10, 600));
    parameters.add(maxVelocity.set("maxVelocity", 46, 10, 600));

    parameters.add(visualRange.set("visualRange", 50, 10, 100));

    parameters.add(minDistance.set("minDistance", 11.875, 0, 30));

    parameters.add(centeringFactor.set("centeringFactor", 0.05, 0, 0.2));
    parameters.add(alignmentFactor.set("alignmentFactor", 0.0218, 0, 0.2));
    parameters.add(avoidFactor.set("avoidFactor", 0.03, 0, 0.6));
    parameters.add(turnFactor.set("turnFactor", 1, 0.25, 10));

    parameters.add(margin.set("margin", 0, -100, 300));

    parameters.add(spriteBaseSize.set("spriteBaseSize", 2.0, 0, 40));
    parameters.add(spriteVariation.set("spriteVariation", 0, 0, 8));

    parameters.add(spriteBaseColor.set("spriteBaseColor", ofColor(255, 0, 0), ofColor(0, 0), ofColor(255, 255)));

    parameters.add(spriteBaseAlphaPct.set("spriteBaseAlphaPct", 0.5, 0, 1));
    parameters.add(spriteHueNoiseMaximumPct.set("spriteHueNoiseMaximumPct", 0.01, 0, 1.0));
    parameters.add(spriteSaturationNoiseMaximumPct.set("spriteSaturationNoiseMaximumPct", 0, 0, 1.0));
    parameters.add(spriteValueNoiseMaximumPct.set("spriteValueNoiseMaximumPct", 0.1, 0, 1.0));

    parameters.add(glowIntensity.set("glowIntensity", 0.7, 0.1, 4.0));
    parameters.add(intensityAtEighthWidth.set("intensityAtEighthWidth", 0.0667, 0, 0.1));
    parameters.add(blendMode.set("blendMode", 1, 0, 8));
    parameters.add(toneMap.set("toneMap", true));

    parameters.add(noiseTemporalRate.set("noiseTemporalRate", 0.5, 0, 2));
    parameters.add(noiseSpatialFrequency.set("noiseSpatialFrequency", 1.7, 0, 2));
    parameters.add(noiseVisualize.set("noiseVisualize", false));
    parameters.add(noiseScale.set("noiseScale", 8.03, -4, 10));

    spriteBaseColor.addListener(this, &Field::spriteColorChanged);

    spriteBaseAlphaPct.addListener(this, &Field::spriteColorChangedFloatParam);
    spriteHueNoiseMaximumPct.addListener(this, &Field::spriteColorChangedFloatParam);
    spriteSaturationNoiseMaximumPct.addListener(this, &Field::spriteColorChangedFloatParam);
    spriteValueNoiseMaximumPct.addListener(this, &Field::spriteColorChangedFloatParam);

    spriteBaseSize.addListener(this, &Field::spriteSizeChanged);
    spriteVariation.addListener(this, &Field::spriteVariationChanged);
}

float Field::generateNoisySpriteSize(float salt) {
    float identity0 = deterministicRandomPct(salt - 1.0);
    return ofMap(identity0, 0, 1, pow(spriteBaseSize, 1 - spriteVariation), pow(spriteBaseSize, 1 + spriteVariation));
}

ofColor Field::generateNoisySpriteColor(float salt) {
    return ColorUtilities::generateNoisyColor(spriteBaseColor, spriteBaseAlphaPct, spriteHueNoiseMaximumPct,
                                              spriteSaturationNoiseMaximumPct, spriteBaseAlphaPct, salt);
}

void Field::newPressHandler(ColorProvider & clr, Press & p) {
    ofColor color = clr.color(p, 1.0);
    for (auto it = flock.l.begin(); it != flock.l.end(); ++it) {
        if (ofRandomuf() < 0.5) {
            it->color = color;
        }
    }
    for (auto it = flock.l.begin(); it != flock.l.end(); ++it) {
        it->velocity *= 1.2;
    }
}

void Field::update(KeyState & ks, ColorProvider & clr) {
    // float arousalShaped = math::tan(ks.arousalPct()) / 1.556;
    float arousalExponent = ofMap(ks.arousalPct(), 0, 1, -0.2, 2.2);
    float ffwModulation = pow(2, arousalExponent) / 2;  // [sqrt(x), x^2]
    flock.fastForward = fastForward * ffwModulation;
    flock.visualRange = visualRange;
    flock.minDistance = minDistance * pow(ffwModulation, 0.5);
    flock.avoidFactor = avoidFactor;
    flock.alignmentFactor = alignmentFactor;
    flock.centeringFactor = centeringFactor;
    flock.turnFactor = turnFactor * pow(ffwModulation, 0.5);
    flock.minVelocity = minVelocity * pow(ffwModulation, 0.5);
    flock.maxVelocity = maxVelocity * pow(ffwModulation, 0.5);
    flock.margin = margin;
    flock.xMin = 0;
    flock.xMax = width;
    flock.yMin = 0;
    flock.yMax = height;
    flock.zMin = 0;
    flock.zMax = depth;

    // rotateFlockHueOverTime();

    adjustFlockPopulation();

    // ofVec3f steer = steerAccordingToKeyPresses(ks);
    flock.update(ofGetLastFrameTime(), ofGetElapsedTimef());
}

void Field::translateField() {
    // Push to the right by the window width / 2 to center,
    // Push down by the window height / 2 to center
    // Push away 1/4 of depth to reduce in-your-face-ness
    ofTranslate(0.5 * ofGetWidth() - 0.5 * width, 0.5 * ofGetHeight() - 0.5 * height, 0);
}

void Field::draw(KeyState & ks, ColorProvider & clr, DrawManager & dm) {
    ofPushStyle();
    kkDisableAlphaBlending();  // Speeds things up if we're all opaque.
    ofSetCircleResolution(8);
    ofPushMatrix();
    ofDisableDepthTest();  // Speeds things up

    if (noiseVisualize) {
        drawNoiseVisualize(noiseSpatialFrequency, noiseTemporalRate, noiseScale, ofGetElapsedTimef());
        return;
    }

    float computedDampenRadius;
    float scale = std::min(ofGetWidth() / width, ofGetHeight() / height);
    for (auto it = flock.l.begin(); it != flock.l.end(); ++it) {
        float colinearityWithOpticalAxis = abs(glm::dot(glm::normalize(it->velocity), glm::vec3(0, 0, 1)));
        // float colinearityWithXAxis = abs(glm::dot(glm::normalize(it->velocity), glm::vec3(1,0,0)));
        // float colinearityWithYAxis = abs(glm::dot(glm::normalize(it->velocity), glm::vec3(0,1,0)));

        switch (drawStyle) {
            case 1:
                // TODO Get
                // drawWave(it->position.x, 0, it->size, 1.0, it->color);
                // dm.shadeGlowCircle resets the translation/rotation to origin because of fbo swap.
                //
                ofPushMatrix();
                ofTranslate(0.5 * ofGetWidth() - 0.5 * width, 0.5 * ofGetHeight() - 0.5 * height, 0);
                ofScale(scale);
                // colinearityWithOpticalAxis = abs(it->velocity.getNormalized().dot(ofVec3f(0,0,1)));
                computedDampenRadius = getGlowDampenRatio(glowIntensity, intensityAtEighthWidth,
                                                          std::min(ofGetWidth(), ofGetHeight()) / 8.0);
                dm.shadeGlowCircle(it->position, it->size * (1 - colinearityWithOpticalAxis), it->color, glowIntensity,
                                   computedDampenRadius, blendMode, toneMap);
                ofPopMatrix();

                break;
            default:
                ofPushMatrix();
                ofTranslate(0.5 * ofGetWidth() - 0.5 * width, 0.5 * ofGetHeight() - 0.5 * height, 0);
                ofScale(scale);
                ofSetColor(ofColor(255, 0, 0));
                ofDrawCircle(it->position.x, it->position.y, it->position.z / 4,
                             it->size * (1 - colinearityWithOpticalAxis));
                ofPopMatrix();
                break;
        }
    }
    dm.shadeBlurX(2 * scale, INVERSE_OF_GAUSSIAN_CENTER_PIXEL);
    dm.shadeBlurY(2 * scale, INVERSE_OF_GAUSSIAN_CENTER_PIXEL);

    ofPopMatrix();
    ofPopStyle();
}

void Field::seedSingleSprite(float salt) {
    float x = deterministicRandomPct(salt) * width;
    float y = deterministicRandomPct(salt * 11.1) * height;
    float z = deterministicRandomPct(salt * 22.2) * depth;
    float velocity = deterministicRandomPct(salt * 55.5) * maxVelocity;

    ofVec3f positionVector = ofVec3f(x, y, z);
    ofVec3f velocityVector = deterministicRandomUnitVector(salt) * velocity;

    float size = generateNoisySpriteSize(salt);
    ofColor color = generateNoisySpriteColor(salt);

    flock.l.push_back(SizedSprite(positionVector, velocityVector, color, size));
}

void Field::adjustFlockPopulation() {
    int numToCreate = population - static_cast<int>(flock.l.size());
    if (numToCreate < 0) {
        for (int i = -numToCreate; i > 0; i--) {
            flock.l.pop_back();
        }
    } else {
        for (int i = 0; i < numToCreate; i++) {
            seedSingleSprite(getSystemTimeSecondsPrecise() + i);
        }
    }
}

void Field::rotateFlockHueOverTime() {
    if (ofGetFrameNum() % 120 == 0) {
        float hue;
        float saturation;
        float value;
        spriteBaseColor.get().getHsb(hue, saturation, value);
        hue = fmod(hue + 1, 255.0);
        ofColor newColor = ofColor::fromHsb(hue, saturation, value);
        spriteBaseColor.set(newColor);
    }
}

ofVec3f Field::steerAccordingToKeyPresses(KeyState & ks) {
    ofVec3f steer = ofVec3f(0, 0, 0);
    for (const auto & press : ks.activePresses()) {
        //         Let's do (1-x)^5 for [0,1]
        float effect = press.audibleAmplitudePct(ks.attackTimeS, 0.2, 0, 0);
        steer += deterministicRandomUnitVector(press.id) * effect * 20;
    }
    return steer;
}
