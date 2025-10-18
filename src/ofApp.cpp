#include <boost/filesystem.hpp>

#include "ofApp.h"

#define KEYSTATE_CLEANUP_TIME 10
void ofApp::initializeForms() {
    // To be invoked on resize / startup. Initialize forms (makes resizes easier).
    forms.clear();

    // clang-format off
    forms = {
        std::shared_ptr<VisualForm>(new RadialParticles("RadialParticles")),
        
        std::shared_ptr<VisualForm>(new NoiseGrid("NoiseGrid")),
        std::shared_ptr<VisualForm>(new EdgeParticles("EdgeParticles")),
        std::shared_ptr<VisualForm>(new MeshGrid("MeshGrid")),
        std::shared_ptr<VisualForm>(new EdgeLasers("EdgeLasers")),
        
        
        
        
//        std::shared_ptr<VisualForm>(new LaserWaves("LaserWaves")),
        // std::shared_ptr<VisualForm>(new GravityParticles("GravityParticles")),
        // std::shared_ptr<VisualForm>(new Field("Field")),
        std::shared_ptr<VisualForm>(new RandomParticles("RandomParticles")),
        // std::shared_ptr<VisualForm>(new GlowLinePlayground("GlowLinePlayground")),
        std::shared_ptr<VisualForm>(new Shape("Shape")),
        std::shared_ptr<VisualForm>(new GlowShape("GlowShape")),
//        std::shared_ptr<VisualForm>(new FatGlowShape("FatGlowShape")),
        
        // std::shared_ptr<VisualForm>(new Orbit("Orbit")),
//        std::shared_ptr<VisualForm>(new BaseWaves("BaseWaves")),
                
        
        std::shared_ptr<VisualForm>(new Thunder("Thunder")),
        std::shared_ptr<VisualForm>(new RapidThunder("StableThunder")),
        

        // std::shared_ptr<VisualForm>(new ImageSprocket()),
        std::shared_ptr<VisualForm>(new PointWaves("PointWaves")),
        

        // std::shared_ptr<VisualForm>(new Drawer()),
//        std::shared_ptr<VisualForm>(new March()),  // TODO Is this a memory leak?
// std::shared_ptr<VisualForm>(new ImageSprocket()),
//        std::shared_ptr<VisualForm>(new BlackDot()),
//        std::shared_ptr<VisualForm>(new Lotus()),
//        std::shared_ptr<VisualForm>(new BlackLight())};
        
    };
    // clang-format on

    // Clear every time because resize should just reinitialize everything from scratch.
    gui.clear();
    gui.setup("panel");

    metaParameterGroup.clear();
    metaParameterGroup.add(trail.set("Trail", 0, 0, 1));
    adsrParameterGroup.add(ks.arousalKurtosis);
    adsrParameterGroup.add(ks.valenceKurtosis);
    //    metaParameterGroup.add(dm.blurOffsetStepPixels.set("blurOffsetStepPixels", 1, 0, 4));
    //    metaParameterGroup.add(dm.aberrationOpacity.set("aberrationOpacity", 2.5, 0, 10));
    //    metaParameterGroup.add(dm.aberrationMinUnits.set("aberrationMinUnits", 2, 0, 10));
    //    metaParameterGroup.add(dm.aberrationMaxUnits.set("aberrationMaxUnits", 2, 0, 10));
    //    metaParameterGroup.add(dm.aberrationTremorDurationS.set("aberrationTremorDurationS", 0.05, 0, 0.5));
    //    for (auto& param : dm.shaderToggles) {
    //        metaParameterGroup.add(param);
    //    }

    metaParameterGroup.setName("Meta Parameters");
    gui.add(metaParameterGroup);
    gui.getGroup("Meta Parameters").disableHeader();

    colorParameterGroup.clear();
    colorParameterGroup.add(clr.baseHue);
    colorParameterGroup.add(clr.baseSaturation);
    colorParameterGroup.add(clr.baseValue);
    colorParameterGroup.add(clr.maxHue);
    colorParameterGroup.add(clr.maxSaturation);
    colorParameterGroup.add(clr.maxValue);
    colorParameterGroup.add(clr.clockwise);
    colorParameterGroup.add(clr.cyclical);

    colorParameterGroup.setName("Color");
    gui.add(colorParameterGroup);

    adsrParameterGroup.clear();
    adsrParameterGroup.add(ks.attackTimeS);
    adsrParameterGroup.add(ks.decayTimeS);
    adsrParameterGroup.add(ks.sustainLevelPct);
    adsrParameterGroup.add(ks.releaseTimeS);
    adsrParameterGroup.add(ks.ephemeralDecayPerS);
    adsrParameterGroup.setName("ADSR");
    gui.add(adsrParameterGroup);

    for (auto form : forms) {
        form->setup();

        if (form->parameters.size()) {
            ofParameterGroup parameterGroup;
            parameterGroup.add(form->parameters);
            parameterGroup.setName(form->name);
            gui.add(parameterGroup);
            gui.getGroup(form->name).disableHeader();
        }
    }

    gui.minimizeAll();
    gui.getGroup("Meta Parameters").maximize();
    // Individual maximize happens in switch to form happens at end of setup
}

//--------------------------------------------------------------
void ofApp::setup() {
    
    ofSetLogLevel(getLogLevelEnum(getEnv("OFW_LOG_LEVEL", "OF_LOG_NOTICE")));
    ofSetLogLevel("NDI", getLogLevelEnum(getEnv("OFW_LOG_LEVEL_NDI", "OF_LOG_NOTICE")));
    ofSetLogLevel("OSC", getLogLevelEnum(getEnv("OFW_LOG_LEVEL_NDI", "OF_LOG_NOTICE")));
    ofSetLogLevel("MQTT", getLogLevelEnum(getEnv("OFW_LOG_LEVEL_NDI", "OF_LOG_NOTICE")));
    ofSetLogLevel("warnOnSlow", getLogLevelEnum(getEnv("OFW_LOG_LEVEL_WARN_ON_SLOW", "OF_LOG_WARNING")));
    monitorFrameRateMode = getEnv("MONITOR_FRAME_RATE", "false") == "true";
    
    enableNDI = getEnv("ENABLE_NDI", "true") == "true";
    enableMQTT = getEnv("ENABLE_MQTT", "true") == "true";
    requireMQTT = getEnv("REQUIRE_MQTT", "true") == "true";
    
    try {
        exitAfterFrames = boost::optional<int>(stoi(getEnv("EXIT_AFTER_FRAMES")));
        ofLogWarning() << "EXIT_AFTER_FRAMES present. Will exit after " << exitAfterFrames.value() << " frames.";
        //    } catch (const std::invalid_argument&) {
    } catch (...) {
        exitAfterFrames = boost::none;
    }


#ifdef TARGET_RASPBERRY_PI
    metaParameterGroup.add(ledBrightnessPct.set("LED Brightness", stof(getEnv("LED_BRIGHTNESS")), 0, 1));
    metaParameterGroup.add(idleLedBrightnessPct.set("Idle LED Brightness", stof(getEnv("IDLE_LED_BRIGHTNESS")), 0, 1));
    
    uint8_t ledBrightness = static_cast<uint8_t>(ofMap(ledBrightnessPct, 0, 1, 0, 100)); // LED API expects uint8_t [0,100]
    uint8_t idleLedBrightness = static_cast<uint8_t>(ofMap(idleLedBrightnessPct, 0, 1, 0, 100)); // LED API expects uint8_t [0,100]
    
    clockwiseRotations = initializeLEDMatrices();
#endif

    ks = KeyState();
    clr = ColorProvider();

    
    fontSize = stoi(getEnv("APPLICATION_LOGO_FONT_SIZE", "13"));
    if (!helveticaNeue.load("Inter-Bold.ttf", fontSize, true, true)) {
        throw std::runtime_error("Couldn't load font.");
    }
    helveticaNeue.setLetterSpacing(stof(getEnv("FONT_SIZE_SPACING", "0.98")));

    if (!helveticaNeueSmall.load("Inter-Bold.ttf", stoi(getEnv("FONT_SIZE_SMALL", "10")), true, true)) {
        throw std::runtime_error("Couldn't load font.");
    }
    helveticaNeueSmall.setLetterSpacing(stof(getEnv("FONT_SIZE_SMALL_SPACING", "0.98")));

    if (!helveticaNeueTiny.load("Inter-Regular.ttf", stof(getEnv("FONT_SIZE_TINY", "9")), true, true)) {
        throw std::runtime_error("Couldn't load font.");
    }
    helveticaNeueTiny.setLetterSpacing(stof(getEnv("FONT_SIZE_TINY_SPACING", "0.9")));
        
    

    guiShow = false;
    debugShow = false;

    // OSC
    ofLogNotice("OSC") << "Setting up OSC receiver...";
    receiver.setup(OSC_PORT);
    ofLogNotice("OSC") << "Online. Port: " << OSC_PORT;

    if (enableMQTT) {
        // MQTT
        ofLogNotice("MQTT") << "Setting up MQTT receiver.";
        mqttConnectHandler();
        ofLogNotice("MQTT") << "Set up.";
    }
    
    if (enableNDI) {
        ofLogNotice("NDI") << "Initializing NDI and watching sources.";
        NDIlib_initialize();
        finder_.watchSources();
    }
    
#ifdef TARGET_RASPBERRY_PI
    ofLogVerbose() << "VerticalSync disabled; FPS Target: " << ofGetTargetFrameRate();
    ofSetFrameRate(TARGET_FRAME_RATE);
    // Do not use setFrameRate if we're not bound by a display. Leave to LED to limit.
#else
    ofSetVerticalSync(true);
    ofSetFrameRate(TARGET_FRAME_RATE);
    ofLogVerbose() << "VerticalSync enabled; FPS Target: " << ofGetTargetFrameRate();
    // TODO Does this impact Pi / LED ?
#endif
    
    updateLastInteractionMoment();
    initializeForms();
    switchToForm(0);
    
}

//--------------------------------------------------------------
void ofApp::update() {
    noteDebugHandler();
    startupTimeHandler();
    exitAfterFramesHandler();
    
    if (monitorFrameRateMode) {
        monitorFrameRate(ofGetTargetFrameRate());
    }

    if (enableNDI) {
        ndiUpdateHandler(); // Returns true when we have an active frame
    }
    

    double t0 = getSystemTimeSecondsPrecise();
    forms[currentFormIndex]->update(ks, clr);
    if (monitorFrameRateMode) {
        warnOnSlow("Form Update", t0, TARGET_FRAME_TIME_S / WARN_INTERVAL_DENOMINATOR_UPDATE_FORM);
    }

#ifndef TARGET_RASPBERRY_PI
    std::stringstream strm;
    strm << "FPS: " << ofGetFrameRate();
    ofSetWindowTitle(strm.str());
#endif

    // Clean all keys that have been released for more than 5 seconds.
    ks.cleanup(KEYSTATE_CLEANUP_TIME);

    // NOTE: Disable homeostasis
    // ks.circumplexHomeostasis();

    // check for waiting messages
    pollForOSCMessages();

    if (enableMQTT) {
        pollForMQTTMessages();
    }
    
}

//--------------------------------------------------------------
void ofApp::draw() {
    double t0 = getSystemTimeSecondsPrecise();

    bool ndiPreempt;
    
    
    dm.beginDraw();  // Sets a new main frame Context

    //    ofEnableAlphaBlending();
    ofBackground(0, 0, 0, 255 * (1 - trail));
    //    ofDisableAlphaBlending();

    
    if (enableNDI) {
    // Will draw screen if NDI is connected. Will return false otherwise.
        ndiDrawHandler();
    }

    ofPushStyle();
    forms[currentFormIndex]->draw(ks, clr, dm);
    ofPopStyle();
    dm.endAndDrawFbo();
    
    if (guiShow) {
        gui.draw();
    }
    
    if (debugShow) {
        std::stringstream stream;
        stream << "Arousal: ";
        stream << std::fixed << std::setprecision(2) << ks.arousalPct();
        std::string s = stream.str();
        helveticaNeue.drawString(s, 5, ofGetHeight() - helveticaNeue.getStringBoundingBox(s, 0, 0).getHeight() + 5);

        ofPushStyle();
        ofSetColor(ofColor::fromHsb(ofMap(ks.arousalPct(), 0, 1, 255, 170), 255, 255));
        ofDrawRectangle(0, ofGetHeight() - 5, ofGetWidth() * ks.arousalPct(), 5);
        ofPopStyle();
    }
    
    bool showWebsite = getEnv("SHOW_WEBSITE", "false") == "true";
    
    showWebsite ? applicationLogoAndWebsiteHandler() : applicationLogoHandler();  // Draws logo if necessary
    amperageTestHandler();     // Brightens everything if necessary
    debugModeHandler();

    
#ifdef TARGET_RASPBERRY_PI
    brightnessUpdateHandler();
    image.clear();
    image.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
    led.draw(image);
#endif
    if (monitorFrameRateMode) {
        warnOnSlow("Draw", t0, TARGET_FRAME_TIME_S / WARN_INTERVAL_DENOMINATOR_DRAW);
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
    dm = DrawManager();
    initializeForms();
    switchToForm(currentFormIndex);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {}

void ofApp::switchToForm(int formIndex) {
    gui.minimizeAll();
    gui.getGroup("Meta Parameters").maximize();
    currentFormIndex = formIndex;
    ofLogNotice() << "Switched to " << forms[formIndex]->name;
    if (forms[formIndex]->parameters.size()) {
        gui.getGroup(forms[formIndex]->name).maximize();
    }
}

void ofApp::exit() {
    if (enableMQTT && mqttClientConnectedSuccessfully) {
        ofLogVerbose() << "MQTT disconnecting...";
        client.disconnect();
        ofLogNotice() << "Mqtt disconnected.";
    }

    ofLogNotice("ofApp") << "Cleanly exiting.";
}

void ofApp::previousForm() { switchToForm(positive_modulo(currentFormIndex - 1, static_cast<int>(forms.size()))); }

void ofApp::nextForm() { switchToForm(positive_modulo(currentFormIndex + 1, static_cast<int>(forms.size()))); }
