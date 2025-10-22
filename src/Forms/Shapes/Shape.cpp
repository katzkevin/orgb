//
//  Shape.cpp
//  orgb
//
//  Created by Kevin Katz on 10/4/21.
//

#include "Shape.hpp"

#include <math.h>

#define MAX_SIDES 10
#define ATTACK_TIME 0
#define DECAY_TIME 0.5
#define SHAPE_PRIME 7757

Shape::Shape(const std::string & name) : VisualForm(name) {
    parameters.add(drawMode.set("drawMode", 1, 0, 1));
    parameters.add(blurOffset.set("blurOffset", 1.5, 0, 4.0));
    parameters.add(blurGain.set("blurGain", 1.5, 1.0, 4.0));
    parameters.add(glowIntensity.set("glowIntensity", 1.21, 0.1, 4.0));
    parameters.add(intensityAtEighthWidth.set("intensityAtEighthWidth", 0.15, 0, 1.0));
    parameters.add(blendMode.set("blendMode", 2, 0, 8));
    parameters.add(toneMap.set("toneMap", false));
}

void Shape::drawUnit(const ofColor & color, KeyState & ks, DrawManager & dm, Press & press) {
    ofPath shapeVertices;
    shapeVertices = getOrCreatePath(press);
    if (drawMode == 0) {
        shapeVertices.setColor(color);
        shapeVertices.draw();
    } else {
        auto v = shapeVertices.getOutline()[0].getVertices();
        float functionalGlowIntensity = glowIntensity * ks.arousalGain();
        float computedDampenRadius =
            getGlowDampenRatio(glowIntensity, intensityAtEighthWidth, std::min(ofGetWidth(), ofGetHeight()) / 8.0);

        for (int i = 0; i < v.size(); i++) {
            ofVec2f from(v[i].x, v[i].y);
            ofVec2f to(v[(i + 1) % v.size()].x, v[(i + 1) % v.size()].y);

            dm.shadeGlowLine(from, to, color, functionalGlowIntensity, computedDampenRadius, blendMode, toneMap);
        }
    }
}

void Shape::draw(KeyState & ks, ColorProvider & clr, DrawManager & dm) {
    for (auto press : ks.allPresses()) {
        double amplitude =
            press.audibleAmplitudePct(ks.attackTimeS, ks.decayTimeS, ks.sustainLevelPct, ks.releaseTimeS);
        ofColor color = clr.color(press, amplitude);
        if (ofIsFloatEqual(amplitude, 0.0)) {
            continue;
        }
        drawUnit(color, ks, dm, press);
        // shapeVertices.draw();
    }

    for (auto press : ks.allEphemeralPresses()) {
        if (ofIsFloatEqual(press.velocityPct, 0.0f)) {
            continue;
        }
        ofColor color = clr.color(press, press.velocityPct);
        drawUnit(color, ks, dm, press);
    }
    if (drawMode == 0) {
        // Only blur non-glow
        dm.shadeBlurX(blurOffset, blurGain);  // Blurrier at low arousal, sharper at high
        dm.shadeBlurY(blurOffset, blurGain);  // Blurrier at low arousal, sharper at high
    }
}

ofPath Shape::getOrCreatePath(Press & p) {
    // This used to cache shapes. Now it doesn't we want them to expand over time.
    float radius = calculateRadius(p);

    int numSides = floor(ofMap(deterministicRandomPct(p.id % SHAPE_PRIME + 0), 0, 1, 3,
                               MAX_SIDES + 1));  // [3,MAX_SIDES] sides (MAX_SIDES + 1 technically possible)
    float dx = ofMap(deterministicRandomPct(p.id % SHAPE_PRIME + 1), 0, 1, 0, ofGetWidth());
    float dy = ofMap(deterministicRandomPct(p.id % SHAPE_PRIME + 2), 0, 1, 0, ofGetHeight());
    float rotate = ofMap(deterministicRandomPct(p.id % SHAPE_PRIME + 3), 0, 1, 0, 2 * PI);
    ofVec3f rotateAxis = deterministicRandomUnitVector(p.id % SHAPE_PRIME + 4);
    //        numSides = 5;
    //        radius = 6;
    //        dx = 8;
    //        dy = 8;
    //        rotate = 0;

    // ofLogNotice() << p.note << " " << p.velocityPct << " " << p.id << " " << dx;
    ofPath computedShape = shape(numSides, radius);
    computedShape.rotateRad(rotate, rotateAxis);
    computedShape.translate(ofVec3f(dx, dy, 0));
    computedShape.setFilled(true);

    return computedShape;
}

// Expanding radius
float Shape::calculateRadius(Press & p) {
    float dt = NAN;
    if (p.getReleaseTime().has_value()) {
        // Key is released (from sustain or just plain released)
        dt = p.getReleaseTime().value() - p.tSystemTimeSeconds;
    } else {
        dt = getSystemTimeSecondsPrecise() - p.tSystemTimeSeconds;
    }
    // [0, 1)
    float noteUnit = ofMap(p.note, 0, MIDI_NOTE_MAX, 1, 0, true);
    // [6,32] were previous options

    int maxSide = std::min(ofGetWidth(), ofGetHeight());
    float initialRadius = exponentialMap(noteUnit, 0, 1, maxSide / 4, maxSide / 2, true, 0.5);

    float growVelocityPerSecond = ofMap(p.note, 0, MIDI_NOTE_MAX, 0, 5.0, true);

    return initialRadius + growVelocityPerSecond * dt;
}

ofPath Shape::shape(int sideCount, float radius) {
    float edgeLength = radius * 2 * sin(PI / sideCount);
    float sumOfInteriorAnglesRadians = (sideCount - 2) * PI;
    float r = sumOfInteriorAnglesRadians / sideCount;
    ofPath path;

    for (int i = 0; i < sideCount; ++i) {
        // This is fucking stupid, just let me use absolute coordinates.
        float comp = (PI - r) * i;
        float x = cos(comp) * edgeLength;
        float y = sin(comp) * edgeLength;
        path.lineTo(x, y, 0);
    }
    path.close();  // close the shape
    return path;
}
