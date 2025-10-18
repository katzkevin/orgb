//
//  ofAppUtilities.cpp
//  orgb
//
//  Created by Kevin Katz on 2/2/22.
//

#include <stdio.h>

#include "ofApp.h"

#define EXIT_AFTER_FRAME_PRESS_INTERVAL 20

bool ofApp::applicationIsIdle() { return getSystemTimeSecondsPrecise() - lastInteractionMoment > IDLE_DURATION_S; }

// Update the second at which the app was last interacted with. After a duration, the led will idle-dim
void ofApp::updateLastInteractionMoment() { lastInteractionMoment = getSystemTimeSecondsPrecise(); }

void ofApp::amperageTestHandler() {
    bool amperageTestMode = getSystemTimeSecondsPrecise() < amperageTestModeMoment + AMPERAGE_TEST_DURATION_S;
    if (amperageTestMode) {
        // Full white
        ofPushStyle();
        ofSetColor(255);
        ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
        ofPopStyle();
    }
}

void ofApp::debugModeHandler() {
    bool debugTestMode = getSystemTimeSecondsPrecise() < debugModeMoment + DEBUG_MODE_DURATION_S;
    if (!debugTestMode) {
        return;
    }

    ofPushStyle();

    ofSetColor(ofColor::fromHsb(255 / 4.0 * 0, 255, 255));
    ofDrawRectangle(0, 0, ofGetWidth(), 2);

    ofSetColor(ofColor::fromHsb(255 / 4.0 * 1, 255, 255));
    ofDrawRectangle(ofGetWidth() - 2, 0, 2, ofGetHeight());

    ofSetColor(ofColor::fromHsb(255 / 4.0 * 2, 255, 255));
    ofDrawRectangle(0, ofGetHeight() - 2, ofGetWidth(), 2);

    ofSetColor(ofColor::fromHsb(255 / 4.0 * 3, 255, 255));
    ofDrawRectangle(0, 2, 2, ofGetHeight());

    if (ofGetWidth() < ofGetHeight()) {
        ofRotateDeg(90);
        ofTranslate(0, -ofGetWidth());
    }
    std::string hostname = getEnv("SYSTEM_HOSTNAME", "N/A");
    ofSetColor(255);
    helveticaNeueSmall.drawString(getEnv("SYSTEM_HOSTNAME", "N/A"), 2,
                                  helveticaNeueSmall.getStringBoundingBox(hostname, 0, 0).getHeight() + 2);

    ofPopStyle();
}

void ofApp::exitAfterFramesHandler() {
    if (exitAfterFrames.is_initialized()) {
        if (ofGetFrameNum() % EXIT_AFTER_FRAME_PRESS_INTERVAL == 0) {
            // If in test mode, include three dummy presses.
            ks.newKeyPressedHandler(40, 1.0, static_cast<unsigned int>(ofGetSystemTimeMicros() % UINT_MAX));
            ks.newKeyPressedHandler(42, 0.8, static_cast<unsigned int>(ofGetSystemTimeMicros() % UINT_MAX));
            ks.newKeyPressedHandler(44, 0.6, static_cast<unsigned int>(ofGetSystemTimeMicros() % UINT_MAX));
        } else if (ofGetFrameNum() % EXIT_AFTER_FRAME_PRESS_INTERVAL == EXIT_AFTER_FRAME_PRESS_INTERVAL / 2) {
            ks.keyReleasedHandler(40);
            ks.keyReleasedHandler(42);
            ks.keyReleasedHandler(44);
        }
    }

    if (exitAfterFrames.is_initialized() && ofGetFrameNum() > exitAfterFrames.value()) {
        ofLogWarning() << "exitAfterFramesHandler triggering exit after " << exitAfterFrames.value() << " frames.";
        ofExit(0);
    }
}

void ofApp::startupTimeHandler() {
    if (ofGetFrameNum() == 0) {
        ofLogNotice() << "Startup time: " << ofGetElapsedTimef();
        ofResetElapsedTimeCounter();
    }
}

void ofApp::noteDebugHandler() {
    if (!getEnvOptional("NOTE_DEBUG").is_initialized()) {
        return;
    }

    switch (ofGetFrameNum() % 200) {
        case 0:
            ks.newKeyPressedHandler(30, 20, static_cast<unsigned int>(ofGetSystemTimeMicros() % UINT_MAX));
            break;
        case 10:
            ks.newKeyPressedHandler(31, 60, static_cast<unsigned int>(ofGetSystemTimeMicros() % UINT_MAX));
            break;
        case 20:
            ks.newKeyPressedHandler(34, 100, static_cast<unsigned int>(ofGetSystemTimeMicros() % UINT_MAX));
            break;
        case 30:
            ks.newKeyPressedHandler(36, 120, static_cast<unsigned int>(ofGetSystemTimeMicros() % UINT_MAX));
            break;
        case 40:
            ks.newKeyPressedHandler(38, 127, static_cast<unsigned int>(ofGetSystemTimeMicros() % UINT_MAX));
            break;

        case 50:
            ks.newKeyPressedHandler(90, 20, static_cast<unsigned int>(ofGetSystemTimeMicros() % UINT_MAX));
            break;
        case 60:
            ks.newKeyPressedHandler(92, 60, static_cast<unsigned int>(ofGetSystemTimeMicros() % UINT_MAX));
            break;
        case 70:
            ks.newKeyPressedHandler(94, 100, static_cast<unsigned int>(ofGetSystemTimeMicros() % UINT_MAX));
            break;
        case 80:
            ks.newKeyPressedHandler(97, 120, static_cast<unsigned int>(ofGetSystemTimeMicros() % UINT_MAX));
            break;
        case 90:
            ks.newKeyPressedHandler(98, 127, static_cast<unsigned int>(ofGetSystemTimeMicros() % UINT_MAX));
            break;

        case 100:
            ks.keyReleasedHandler(30);
            break;
        case 110:
            ks.keyReleasedHandler(31);
            break;
        case 120:
            ks.keyReleasedHandler(34);
            break;
        case 130:
            ks.keyReleasedHandler(36);
            break;
        case 140:
            ks.keyReleasedHandler(38);
            break;

        case 150:
            ks.keyReleasedHandler(90);
            break;
        case 160:
            ks.keyReleasedHandler(92);
            break;
        case 170:
            ks.keyReleasedHandler(94);
            break;
        case 180:
            ks.keyReleasedHandler(97);
            break;
        case 190:
            ks.keyReleasedHandler(98);
            break;
    }
}
