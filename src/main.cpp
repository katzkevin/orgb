#include "ofApp.h"
#include "ofMain.h"

//========================================================================
int main() {
    int width;
    int height;
#ifdef TARGET_RASPBERRY_PI
    width = stoi(getEnv("OFW_WIDTH"));
    height = stoi(getEnv("OFW_HEIGHT"));
#else
    width = 160;
    height = 32;
#endif

    //	ofSetupOpenGL(1024,768,OF_WINDOW);			// <-------- setup the GL contex
    ofSetupOpenGL(width, height, OF_WINDOW);  // <-------- setup the GL contex
    // this kicks off the running of my app
    // can be OF_WINDOW or OF_FULLSCREEN
    // pass in width and height too:
    ofRunApp(new ofApp());
}
