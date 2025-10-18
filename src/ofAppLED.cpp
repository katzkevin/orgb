//
//  ofAppLED.cpp
//  orgb
//
//  Created by Kevin Katz on 3/23/22.
//

#include "ofApp.h"

int ofApp::initializeLEDMatrices() {
    // Return the number of clockwise rotations
#ifdef TARGET_RASPBERRY_PI
    std::string hardwareMapping = getEnv("HARDWARE_MAPPING");
    int columns = stoi(getEnv("LED_COLUMNS"));
    int rows = stoi(getEnv("LED_ROWS"));
    int chain = stoi(getEnv("LED_CHAIN"));
    int parallel = stoi(getEnv("LED_PARALLEL"));
    uint8_t ledBrightness =
        static_cast<uint8_t>(ofMap(ledBrightnessPct, 0, 1, 0, 100));  // LED API expects uint8_t [0,100]
    uint8_t idleLedBrightness =
        static_cast<uint8_t>(ofMap(idleLedBrightnessPct, 0, 1, 0, 100));  // LED API expects uint8_t [0,100]
    int gpioSlowdown = stoi(getEnv("GPIO_SLOWDOWN"));
    std::string pixelMapperConfig = getEnv("PIXEL_MAPPER_CONFIG");
    std::string ledRgbSequence = getEnv("LED_RGB_SEQUENCE");
    // Rotate 270 , chain 5, 32x32 -> 32x160
    // chain 5, 32x32 -> 160x32
    ofLogNotice("LED") << "Initializing Panel with mapping: " << hardwareMapping << " ledColumns:" << columns
                       << " ledRows:" << rows << " chain:" << chain << " parallel:" << parallel
                       << " brightness:" << static_cast<int>(ledBrightness)
                       << " idleLedBrightness:" << static_cast<int>(idleLedBrightness)
                       << " gpioSlowdown:" << gpioSlowdown << " pixelMapperConfig:" << pixelMapperConfig
                       << " ledRgbSequence:" << ledRgbSequence;
    led.setup(hardwareMapping, columns, rows, chain, parallel, ledBrightness, gpioSlowdown, pixelMapperConfig,
              ledRgbSequence);

    ofLogNotice("LED") << "led.setup() successful.";

    if (pixelMapperConfig.find("Rotate:90") != std::string::npos) {
        ofLogNotice() << "Right rotations: 1";
        return 1;
    } else if (pixelMapperConfig.find("Rotate:180") != std::string::npos) {
        ofLogNotice() << "Right rotations: 2";
        return 2;
    } else if (pixelMapperConfig.find("Rotate:270") != std::string::npos) {
        ofLogNotice() << "Right rotations: 3";
        return 3;
    } else {
        ofLogNotice() << "Right rotations: 0";
        return 0;
    }
#else
    ofLogNotice() << "LED Disabled. Right rotations: 0";
    return 0;
#endif
}

void ofApp::brightnessUpdateHandler() {
#ifdef TARGET_RASPBERRY_PI
    uint8_t ledBrightness =
        static_cast<uint8_t>(ofMap(ledBrightnessPct, 0, 1, 0, 100));  // LED API expects uint8_t [0,100]
    uint8_t idleLedBrightness =
        static_cast<uint8_t>(ofMap(idleLedBrightnessPct, 0, 1, 0, 100));  // LED API expects uint8_t [0,100]
    if (applicationIsIdle()) {
        if (led.getBrightness() != idleLedBrightness) {
            led.setBrightness(idleLedBrightness);
        }
    } else {
        if (led.getBrightness() != ledBrightness) {
            led.setBrightness(ledBrightness);
        }
    }
#endif
}
