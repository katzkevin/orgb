//
//  Drawing.cpp
//  orgb
//
//  Created by Kevin Katz on 10/19/21.
//

#include "Lotus.hpp"

struct {
    ofColor magenta = ofColor(216, 0, 151);
    ofColor cyan = ofColor(0, 145, 141);
    ofColor black = ofColor(14, 10, 22);
} magentaCyanBlackPalette;

Lotus::Lotus() : VisualForm("Lotus") {}

void Lotus::update(KeyState & ks, ColorProvider & clr) {}

void Lotus::draw(KeyState & ks, ColorProvider & clr, DrawManager & dm) { rotatingPastelLotus(); }

void Lotus::drawLeaf(float scale, ofColor color, float salt) {
    float x2 = 0.294;
    float y2 = 0.317;
    float x3 = 0.5;
    float y3 = 0.0;
    float x4 = 1.0;
    float y4 = 0.0;
    float bezierNoise = 0.3;

    x2 += ofSignedNoise(salt + 0.6) * bezierNoise;
    y2 += ofSignedNoise(salt + 10.6) * bezierNoise;

    //    x3 += ofSignedNoise(salt + 20.6) * bezierNoise;
    //    y3 += ofClamp(ofSignedNoise(salt + 30.6) * bezierNoise, 0.0, CGFLOAT_MAX); // Don't allow negative

    x4 += ofSignedNoise(salt + 20.6) * bezierNoise;
    y4 += ofSignedNoise(salt + 30.6) * bezierNoise;

    ofPath path;
    path.setFillColor(color);
    path.moveTo(0.0, 0.0);
    path.bezierTo(x2, y2, x3, y3, x4, y4);
    path.scale(scale, scale);
    path.close();
    path.draw();

    path.clear();
    path.setFillColor(color);
    path.moveTo(0.0, 0.0);
    path.bezierTo(x2, -y2, x3, -y3, x4, y4);
    path.scale(scale, scale);
    path.close();

    ofMesh m = path.getTessellation();
    path.draw();
}

void Lotus::rotatingPastelLotus() {
    ofPushStyle();

    int steps = 12;
    int rotateDegrees = 360.0 / steps;

    ofSetColor(0);

    //    glTranslatef(ofGetWidth() / 2, ofGetHeight() / 2, 0);

    int laps = 12;
    float scalePerLap = 1.61;
    float smallestScale = 200;

    float hue = 255;
    float hueJitter = 40.0;
    float saturation = 80.0;  // Pastel
    // float saturation = 200.0;
    float saturationJitter = 40.0;
    float value = 240.0;
    float valueJitter = 20.0;

    for (unsigned int lap = laps; lap > 0; --lap) {
        ofPushMatrix();
        int flip = lap % 2 ? -1.0 : 1.0;
        ofRotateZDeg(ofGetFrameNum() / 30.0 * flip);
        for (int i = 0; i < steps; i++) {
            hue = ofWrap(hue + (ofSignedNoise(i + 0.5) * hueJitter), 0.0, 255.0);
            saturation = ofWrap(saturation + (ofSignedNoise(i + 10.5) * saturationJitter), 0.0, 255.0);
            value = ofWrap(value + (ofSignedNoise(i + 20.5) * valueJitter), 0.0, 255.0);
            // ofColor color = ofColor::fromHsb(hue, saturation, value);

            ofColor color;
            switch (deterministicRandom(lap * steps + i) % 3) {
                case 0:
                    color = ofColor(magentaCyanBlackPalette.magenta);
                    break;
                case 1:
                    color = ofColor(magentaCyanBlackPalette.cyan);
                    break;
                case 2:
                    color = ofColor(magentaCyanBlackPalette.black);
                    break;
            }

            drawLeaf(lap * scalePerLap * smallestScale, color, static_cast<float>(lap * steps + i));
            ofRotateZDeg(rotateDegrees);
        }
        ofPopMatrix();
    }
    ofPopStyle();
}

void Lotus::circlesAllTheWayDown(float x, float y, float radius, int generation) {
    float xPos = std::sqrt(ofGetMouseX());
    int count = 4;
    if (radius > std::max((float)2.0, (float)xPos)) {
        ofSetColor(ofColor::fromHsb(0, 255, 255));
        ofNoFill();
        ofDrawCircle(x, y, radius);
        for (int i = 0; i < count; i++) {
            float angle = PI * 2.0 * i / count;
            circlesAllTheWayDown(x + cos(angle) * radius, y + sin(angle) * radius, 0.5 * radius, generation + 1);
        }
    }
}
