#pragma once

#include "ofxMQTT.h"
#include "ofxOsc.h"

// NDI
#include "ofxNDIFinder.h"
#include "ofxNDIReceiver.h"
#include "ofxNDIRecvStream.h"

#ifdef TARGET_RASPBERRY_PI
#include "ofxRpiLED.h"
#endif

#include "BaseWaves.hpp"
#include "ColorUtilities.hpp"
#include "DrawManager.hpp"
#include "EdgeLasers.hpp"
#include "EdgeParticles.hpp"
#include "Effects/AllEffects.hpp"
#include "FatGlowShape.hpp"
#include "Field.hpp"
#include "GlowLinePlayground.hpp"
#include "GlowShape.hpp"
#include "GravityParticles.hpp"
#include "ImageSprocket.hpp"
#include "KeyState.hpp"
#include "LaserWaves.hpp"
#include "MeshGrid.hpp"
#include "NoiseGrid.hpp"
#include "Orbit.hpp"
#include "PointWaves.hpp"
#include "Press.hpp"
#include "RadialParticles.hpp"
#include "RandomParticles.hpp"
#include "RapidThunder.hpp"
#include "ShaderPipeline.hpp"
#include "Shape.hpp"
#include "Thunder.hpp"
#include "Utilities.hpp"
#include "VisualForm.hpp"
#include "ofxOscParameterSync.h"

// #include "FieldWithTrails.hpp"
#include "Lotus.hpp"

#define MIDI_GUITAR_MQTT_TOPIC "midi-guitar"
#define SETTINGS_MQTT_TOPIC "ofw-settings"
#define CLASSIFIER_MQTT_TOPIC "midi-cxd"
#define ORGB_PARAM_MQTT_TOPIC "orgb-param"
#define ORGB_OFPARAMETER_MQTT_TOPIC "orgb-ofparameter"

#define OSC_PORT 19811
#define OSC_SLASH "/"

#define MIDI_KEYBOARD_OSC_ADDRESS "/midi/keyboard"
#define MIDI_GUITAR_OSC_ADDRESS "/midi-guitar"
#define MIC0_PITCH_OSC_ADDRESS "/pitch/mic0"
#define SETTINGS_ADDRESS OSC_SLASH SETTINGS_MQTT_TOPIC

#define AMPERAGE_TEST_DURATION_S 3
#define DEBUG_MODE_DURATION_S 5
#define IDLE_DURATION_S (30 * 60)

// TODO x2 to shut warnings up
#define TARGET_FRAME_RATE 60
#define TARGET_FRAME_TIME_S (1.0 / TARGET_FRAME_RATE)
#define WARN_INTERVAL_DENOMINATOR_UPDATE_FORM 1
#define WARN_INTERVAL_DENOMINATOR_OSC_IO 4   // TODO Speed this up, or diff't thread
#define WARN_INTERVAL_DENOMINATOR_MQTT_IO 4  // TODO Speed this up, or diff't thread
#define WARN_INTERVAL_DENOMINATOR_DRAW 1
#define WARN_INTERVAL_DENOMINATOR_NDI_SCAN 64

class ofApp : public ofBaseApp {
   public:
    void setup() override;
    void update() override;
    void draw() override;

    void keyPressed(int key) override;
    void keyReleased(int key) override;
    void mouseMoved(int x, int y) override;
    void mouseDragged(int x, int y, int button) override;
    void mousePressed(int x, int y, int button) override;
    void mouseReleased(int x, int y, int button) override;
    void mouseEntered(int x, int y) override;
    void mouseExited(int x, int y) override;
    void windowResized(int w, int h) override;
    void dragEvent(ofDragInfo dragInfo) override;
    void gotMessage(ofMessage msg) override;
    void exit() override;

    // Utility for AppIO
    ofParameterGroup * getParameterGroup(std::string groupName);

    /*
     * MQTT Comms
     */
    ofxMQTT client;
    bool mqttClientConnectedSuccessfully;
    void mqttConnectHandler();
    void pollForMQTTMessages();
    void mqttOnMessage(ofxMQTTMessage & msg);
    void mqttOnOnline();
    void mqttOnOffline();

    void dumpSettingsToMqtt();
    bool mqttMessageEncountered;  // TODO Document. This was one message per frame before this for some reason.

    bool enableMQTT;
    bool requireMQTT;
    bool enableNDI;

    /*
     * OSC Comms
     */
    ofxOscReceiver receiver;
    void pollForOSCMessages();

    /*
     * Root IO
     */
    void jsonHandlerEphemeralNote(nlohmann::basic_json<> & j, unsigned int messageId);
    void jsonHandlerMidiMessage(nlohmann::basic_json<> & j);
    void jsonHandlerParamMessage(nlohmann::basic_json<> & j);
    void jsonHandlerOfParamMessage(nlohmann::basic_json<> & j);
    void jsonHandlerClassification(nlohmann::basic_json<> & j);
    std::string dumpSettingsToJsonFile();
    void loadSettingsFromJsonString(std::string payloadString);
    void noteOnHandler(int key, float velocityPct, unsigned int messageId, bool ephemeral = false);
    void noteOffHandler(int key);

    /*
     * LED Matrices
     */
#ifdef TARGET_RASPBERRY_PI
    ofxRpiLED led;
#endif
    static int initializeLEDMatrices();
    void brightnessUpdateHandler();
    ofParameter<float> ledBrightnessPct;      // Out of 100%, cast to uint8_t
    ofParameter<float> idleLedBrightnessPct;  // Out of 100%
    int matrixWidth;
    int matrixHeight;
    ofImage image;

    int clockwiseRotations;

    /*
     * Fonts
     */
    ofTrueTypeFont helveticaNeue;
    ofTrueTypeFont helveticaNeueSmall;
    ofTrueTypeFont helveticaNeueTiny;

    void applicationLogoHandler();
    void applicationLogoAndWebsiteHandler();

    void drawLogo(uint8_t alpha, float verticalOffsetFromCenter);
    void drawWebsite(uint8_t alpha, float verticalOffsetFromCenter, std::string s);

    static ofRectangle userApparentDimensions();  // TODO incorporate into logo
    int fontSize;

    /*
     * Forms
     */
    std::vector<std::shared_ptr<VisualForm>> forms;
    int currentFormIndex;
    void initializeForms();
    void switchToForm(int formIndex);
    void previousForm();
    void nextForm();

    /*
     * GPU
     */
    DrawManager dm;

    /*
     * Post-Processing Shaders
     */
    std::shared_ptr<ShaderPipeline> postProcessing;
    std::shared_ptr<FilmGrainEffect> filmGrainEffect;
    std::shared_ptr<ScanlinesEffect> scanlinesEffect;
    ofParameterGroup postProcessingParameterGroup;

    /*
     * Moments
     */
    bool monitorFrameRateMode;

    // Self-destruct (useful for monitoring)
    boost::optional<int> exitAfterFrames;
    void exitAfterFramesHandler();

    uint64_t amperageTestModeMoment;  // Used for voltage test
    void amperageTestHandler();

    uint64_t debugModeMoment;
    void debugModeHandler();

    void startupTimeHandler();  // Used to reset elapsedTimeTimer in case setup takes a while.
    void noteDebugHandler();

    double lastInteractionMoment;  // Used for screensaver
    void updateLastInteractionMoment();
    bool applicationIsIdle();

    boost::optional<double> lastNdiReconnectAttempt;

    /*
     * NDI
     */
    ofxNDIFinder finder_;
    ofxNDIReceiver receiver_;
    ofxNDIRecvVideoFrameSync video_;
    ofPixels pixels_;

    bool ndiUpdateHandler();
    bool ndiDrawHandler();
    bool ndiScanForVideoSources();
    bool ndiSwitchToVideoSource(ofxNDI::Source & source);
    uint64_t ndiMoment;

    bool guiShow;
    bool debugShow;
    bool enableShader;

    ofxPanel gui;

    ofParameterGroup metaParameterGroup;
    ofParameterGroup colorParameterGroup;
    ofParameterGroup adsrParameterGroup;

    ofParameter<float> trail;

    KeyState ks;
    ColorProvider clr;
};
