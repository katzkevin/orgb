//
//  ofAppLogo.cpp
//  orgb
//
//  Created by Kevin Katz on 4/25/22.
//

#include <stdio.h>

#include "ofApp.h"

#define LOGO_FADE_IN_FOR_SECONDS 0.05
#define LOGO_FADE_OUT_FOR_SECONDS 0.4

ofRectangle ofApp::userApparentDimensions() {
    float userApparentWidth = ofGetWidth();
    float userApparentHeight = ofGetHeight();
    return ofRectangle(0, 0, userApparentWidth, userApparentHeight);
}

void ofApp::drawLogo(uint8_t alpha, float verticalOffsetFromCenter) {
    float f1 = helveticaNeue.getStringBoundingBox("i", 0, 0).getWidth();
    float f2 = helveticaNeue.getStringBoundingBox("ii", 0, 0).getWidth();
    float intraLetterDistance = 2 * f1 - f2;

    ofRectangle r = helveticaNeue.getStringBoundingBox("superpiano", 0, 0);
    ofVec2f offset(floor(-r.x - r.width * 0.5f), floor(-r.y - r.height * 0.5f));

    ofPushMatrix();
    ofPushStyle();

    ofRectangle dim = userApparentDimensions();
    if (dim.width < dim.height) {
        // This is taller than it is wide, display on its side
        ofRotateDeg(90);                // TODO numClockwiseRotations
        ofTranslate(0, -ofGetWidth());  // Move origin to top left
        ofTranslate(ofGetHeight() / 2.0 + offset.x - intraLetterDistance / 2.0,
                    ofGetWidth() / 2.0 + offset.y + verticalOffsetFromCenter);  // To draw from top left
    } else {
        // If we don't have intraLetterDistance / 2.0, we include the tailing space in the calc.
        ofTranslate(ofGetWidth() / 2.0 + offset.x - intraLetterDistance / 2.0,
                    ofGetHeight() / 2.0 + offset.y + verticalOffsetFromCenter);  // To draw from top left
    }

    ofSetColor(0, 0, 0, static_cast<uint8_t>(0.01 * 255 * alpha));
    float extraW = r.width * 0.06125 * (0.5 + ofNoise(getSystemTimeSecondsPrecise() / 2.0 + 50.0));
    float extraH = r.height * 0.25 * (0.5 + ofNoise(getSystemTimeSecondsPrecise() / 2.0 + 50.0));
    ofDrawRectangle(-extraW, -r.height - extraH, r.width + extraW * 2, r.height + extraH * 2);

    ofRectangle bounds;

    ofSetColor(238, 34, 13, alpha);  // Red
    bounds = helveticaNeue.getStringBoundingBox("s", 0, 0);
    helveticaNeue.drawString("s", 0, 0);

    ofTranslate(bounds.width, 0);
    ofSetColor(255, 147, 0, alpha);  // Orange
    bounds = helveticaNeue.getStringBoundingBox("u", 0, 0);
    helveticaNeue.drawString("u", 0, 0);

    ofTranslate(bounds.width, 0);
    ofSetColor(252, 226, 50, alpha);  // Yellow
    bounds = helveticaNeue.getStringBoundingBox("p", 0, 0);
    helveticaNeue.drawString("p", 0, 0);

    ofTranslate(bounds.width, 0);
    ofSetColor(97, 216, 53, alpha);  // Gree
    bounds = helveticaNeue.getStringBoundingBox("e", 0, 0);
    helveticaNeue.drawString("e", 0, 0);

    ofTranslate(bounds.width, 0);
    ofSetColor(1, 162, 255, alpha);  // Blue
    bounds = helveticaNeue.getStringBoundingBox("r", 0, 0);
    helveticaNeue.drawString("r", 0, 0);

    ofTranslate(bounds.width, 0);
    ofSetColor(255, 255, 255, alpha);
    bounds = helveticaNeue.getStringBoundingBox("piano", 0, 0);
    helveticaNeue.drawString("piano", 0, 0);

    ofPopStyle();
    ofPopMatrix();
}

void ofApp::drawWebsite(uint8_t alpha, float verticalOffsetFromCenter, std::string s) {
    float f1 = helveticaNeueTiny.getStringBoundingBox("i", 0, 0).getWidth();
    float f2 = helveticaNeueTiny.getStringBoundingBox("ii", 0, 0).getWidth();
    float intraLetterDistance = 2 * f1 - f2;

    ofRectangle r = helveticaNeueTiny.getStringBoundingBox(s, 0, 0);
    ofVec2f offset(floor(-r.x - r.width * 0.5f), floor(-r.y - r.height * 0.5f));

    ofPushMatrix();
    ofPushStyle();

    ofRectangle dim = userApparentDimensions();
    if (dim.width < dim.height) {
        // This is taller than it is wide, display on its side
        ofRotateDeg(90);                // TODO numClockwiseRotations
        ofTranslate(0, -ofGetWidth());  // Move origin to top left
        ofTranslate(ofGetHeight() / 2.0 + offset.x - intraLetterDistance / 2.0,
                    ofGetWidth() / 2.0 + offset.y + verticalOffsetFromCenter);  // To draw from top left
    } else {
        // If we don't have intraLetterDistance / 2.0, we include the tailing space in the calc.
        ofTranslate(ofGetWidth() / 2.0 + offset.x - intraLetterDistance / 2.0,
                    ofGetHeight() / 2.0 + offset.y + verticalOffsetFromCenter);  // To draw from top left
    }

    ofRectangle bounds;

    ofSetColor(0, 0, 0, static_cast<uint8_t>(0.01 * 255 * alpha));
    float extraW = r.width * 0.06125 * (0.5 + ofNoise(getSystemTimeSecondsPrecise() / 2.0));
    float extraH = r.height * 0.25 * (0.5 + ofNoise(getSystemTimeSecondsPrecise() / 2.0));
    ofDrawRectangle(-extraW, -r.height - extraH, r.width + extraW * 2, r.height + extraH * 2);

    ofSetColor(255, 255, 255, alpha);
    helveticaNeueTiny.drawString(s, 0, 0);

    ofPopStyle();
    ofPopMatrix();
}

void ofApp::applicationLogoHandler() {
    uint8_t alpha;
    float showLogoForSeconds = stof(getEnv("SHOW_LOGO_FOR_SECONDS", "2.0"));

    if (ofGetElapsedTimef() > showLogoForSeconds) {
        return;
    } else if (ofGetElapsedTimef() > showLogoForSeconds - LOGO_FADE_OUT_FOR_SECONDS) {
        alpha = exponentialMap(ofGetElapsedTimef(), showLogoForSeconds - LOGO_FADE_OUT_FOR_SECONDS, showLogoForSeconds,
                               255, 0, 0.75);
    } else if (ofGetElapsedTimef() > LOGO_FADE_IN_FOR_SECONDS) {
        alpha = 255;
    } else {
        alpha = exponentialMap(ofGetElapsedTimef(), 0, LOGO_FADE_IN_FOR_SECONDS, 0, 255, 0.75);
    }
    // Intra-letter distance in pixels
    // Linear equation, y=mx+b, --> 2 * f(1) - f(2)
    drawLogo(alpha, 0);
}

void ofApp::applicationLogoAndWebsiteHandler() {
    uint8_t alpha;
    float showLogoForSeconds = stof(getEnv("SHOW_LOGO_FOR_SECONDS", "2.0"));

    if (ofGetElapsedTimef() > showLogoForSeconds) {
        return;
    } else if (ofGetElapsedTimef() > showLogoForSeconds - LOGO_FADE_OUT_FOR_SECONDS) {
        alpha = exponentialMap(ofGetElapsedTimef(), showLogoForSeconds - LOGO_FADE_OUT_FOR_SECONDS, showLogoForSeconds,
                               255, 0, 0.75);
    } else if (ofGetElapsedTimef() > LOGO_FADE_IN_FOR_SECONDS) {
        alpha = 255;
    } else {
        alpha = exponentialMap(ofGetElapsedTimef(), 0, LOGO_FADE_IN_FOR_SECONDS, 0, 255, 0.75);
    }

    // Intra-letter distance in pixels
    // Linear equation, y=mx+b, --> 2 * f(1) - f(2)

    // float overallOffset = (static_cast<int>((ofGetElapsedTimef() + 2400) / 6) % 6) - 4;
    float overallOffset = 0.0;

    float state = sin(getSystemTimeSecondsPrecise() / 5.0);
    float instComponent = ofMap(state, 0.45, 0.55, 0, 1, true);
    float siteComponent = ofMap(state, 0.45, 0.55, 1, 0, true);

    drawLogo(alpha, stof(getEnv("LOGO_OFFSET", "9")) + overallOffset);
    drawWebsite(static_cast<uint8_t>(alpha * instComponent), stof(getEnv("WEBSITE_OFFSET", "-5")) + overallOffset,
                "@kevinkatz.io");
    drawWebsite(static_cast<uint8_t>(alpha * siteComponent), stof(getEnv("WEBSITE_OFFSET", "-5")) + overallOffset,
                "www.kevinkatz.io");
}
