//
//  ofAppKeyboardComms.cpp
//  orgb
//
//  Created by Kevin Katz on 9/5/22.
//

#include "ofApp.h"



//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    ofLogVerbose("IO") << "Keyboard Pressed: " << key;
    
    noteOnHandler(key, ofRandom(0, 1.0), static_cast<unsigned int>(ofGetSystemTimeMicros() % UINT_MAX), false);
    // Keyboard changes
    switch (key) {
        case 'h':
            guiShow = !guiShow;
            break;
        case '?':
            debugShow = !debugShow;
            break;
        case ',':
            previousForm();
            break;
        case '.':
            nextForm();
            break;
        case '/':
            switchToForm(0);
            break;
        case '`':
            dumpSettingsToJsonFile();
            if (enableMQTT) {
                dumpSettingsToMqtt();
            }
            break;
        case 'q':
            ofSetWindowShape(160, 32);
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
    noteOffHandler(key);
}
