//
//  utilities.cpp
//  orgb
//
//  Created by Kevin Katz on 9/28/21.
//

#include "Utilities.hpp"

#define COMPILE_TIME_SIZE_T_MAX numeric_limits<size_t>::max()
// 2^31
#define MAX_HASH 2147483648
#define GEOMETRY_PRIME 7823
#define NOISE_GRADIENT_EPSILON 0.1

double getSystemTimeSecondsPrecise() { return ofGetSystemTimeMicros() / 1000000.0; }

void warnOnSlow(std::string label, double t0, float thresholdSeconds, int perNFrames, float warmupTimeS) {
    float tNowSeconds = ofGetElapsedTimeMillis() / 1000.0;
    if (ofGetFrameNum() % perNFrames != 0 || tNowSeconds < warmupTimeS) {
        return;
    }
    float t1 = getSystemTimeSecondsPrecise();
    float dt = t1 - t0;
    
    if (dt > thresholdSeconds) {
        ofLogWarning("warnOnSlow") << label << " runnng slowly: " << (t1 - t0) * 1000.0
                       << "ms (Target: " << thresholdSeconds * 1000.0 << "ms)";
    } else if (dt > thresholdSeconds / 2.0) {
        ofLogNotice("warnOnSlow") << label << " runnng slowly: " << (t1 - t0) * 1000.0
                       << "ms (Target: " << thresholdSeconds * 1000.0 << "ms)";
    }
}

void monitorFrameRate(float targetFrameRate, int perNFrames, float warmupTimeS) {
    if (ofGetFrameNum() % perNFrames != 0 || ofGetElapsedTimeMillis() / 1000.0 < warmupTimeS) {
        return;
    }
    float frameRate = ofGetFrameRate();
    bool frameRateViolation = frameRate < targetFrameRate - 3;
    if (frameRateViolation) {
        ofLogWarning() << "Running slowly @ " << frameRate;
    }
}

int positive_modulo(int x, int y) {
    // Wrapping modulus. As in python.
    return (x % y + y) % y;
}

float positive_modulo(float x, float y) {
    // Wrapping modulus. As in python.
    return fmod(fmod(x, y) + y, y);
}

ofVec2f polarToRectangular(float r, float thetaRadians) {
    return ofVec2f(cos(thetaRadians) * r, sin(thetaRadians) * r);
}

ofVec3f sphericalToRectangular(float r, float thetaRadians, float phiRadians) {
    return ofVec3f(cos(thetaRadians) * sin(phiRadians) * r, sin(thetaRadians) * sin(phiRadians) * r,
                   cos(phiRadians) * r);
}

float exponentialMap(float value, float inputMin, float inputMax, float outputMin, float outputMax, bool clamp,
                     float shaper) {
    float pct = ofMap(value, inputMin, inputMax, 0, 1, clamp);
    float shaped = powf(pct, shaper);
    return ofMap(shaped, 0, 1, outputMin, outputMax, clamp);
}

float transitionEaseIn(float pct) { return pow(ofClamp(pct, 0.0, 1.0), 3); }

float transitionEaseInReverse(float pct) {
    // Return 0 if pct >= 1.0
    return pow(1 - ofClamp(pct, 0.0, 1.0), 3);
}

float transitionEaseOutExponential(float pct, float exponent) {
    // Return 1 if pct >= 1.0
    return 1 - pow(1 - ofClamp(pct, 0, 1), exponent);
}

boost::optional<std::string> getEnvOptional(std::string key) {
    try {
        return boost::optional<std::string>(getEnv(key));
    } catch (...) {
        return boost::none;
    }
}

std::string getEnv(std::string key) {
    if (const char* env_p = std::getenv(key.c_str())) {
        return std::string(env_p);
    } else {
        throw std::invalid_argument("Could not find key in environment");
    }
}

std::string getEnv(std::string key, std::string defaultValue) {
    try {
        return getEnv(key);
    } catch (...) {  // TODO Specify
        return defaultValue;
    }
}

std::unordered_map<std::string, ofLogLevel> static const logLevels = {
    {"OF_LOG_VERBOSE", OF_LOG_VERBOSE},         {"OF_LOG_NOTICE", OF_LOG_NOTICE},
    {"OF_LOG_WARNING", OF_LOG_WARNING},         {"OF_LOG_ERROR", OF_LOG_ERROR},
    {"OF_LOG_FATAL_ERROR", OF_LOG_FATAL_ERROR}, {"OF_LOG_SILENT", OF_LOG_SILENT}};

ofLogLevel getLogLevelEnum(std::string s) {
    return logLevels.at(s);
}

std::random_device rd;   // Will be used to obtain a seed for the random number engine
std::mt19937 gen(rd());  // Standard mersenne_twister_engine seeded with rd()

std::uniform_int_distribution<> disi(0.0, UINT_MAX);
std::uniform_real_distribution<> disr(0.0, 1.0);
std::normal_distribution<> disGaussian(0.0, 1.0);

std::mt19937 getRandomEngine(std::string seed) {
    size_t randomSeed = boost::hash_value(seed);
    std::mt19937 gen(randomSeed);
    return gen;
}

unsigned int deterministicRandom(float salt) {
    size_t hash1ui = boost::hash_value(std::to_string(salt));
    return hash1ui;
}

float deterministicRandomPct(float salt) {
    // This is not cryptographically secure. Mapping into 1-space not necessarily a good call.
    // % 2^31, Low 31 bits
    size_t hash1ui = boost::hash_value(std::to_string(salt)) % MAX_HASH;
    return ofMap(hash1ui, 0, MAX_HASH, 0, 1.0, true);
}

ofVec3f deterministicRandomUnitVector(float salt) {
    float theta = deterministicRandomPct(salt * 33.3) * 2 * PI;
    float phi = deterministicRandomPct(salt * 44.4) * 2 * PI;
    return ofVec3f(sin(theta) * cos(phi), sin(phi) * cos(theta), cos(theta));
}

ofVec3f getRandomlyRotatedVectorRad(ofVec3f inVector, float radians) {
    // Rotate this vector randomly in a direction, for conical sampling.
    return getRotatedAwayFromVectorRad(inVector, randomUnitVector(), radians);
}

ofVec3f getRotatedAwayFromVectorRad(ofVec3f inVector, ofVec3f rotateAwayFrom, float radians) {
    ofVec3f rotationAxis = inVector.getCrossed(inVector + rotateAwayFrom);
    return inVector.getRotatedRad(radians, rotationAxis);
}

ofVec3f randomUnitVector() {
    float theta = ofRandomuf() * 2 * PI;
    float phi = ofRandomuf() * 2 * PI;
    return ofVec3f(sin(theta) * cos(phi), sin(phi) * cos(theta), cos(theta));
}

ofVec3f randomUnitVector2D() {
    float angle = ofRandomuf() * 2 * PI;
    return ofVec3f(cos(angle), sin(angle), 0);
}

std::pair<ofVec3f, ofVec3f> edgeToEdgeLineSegment(const Press &p, unsigned int seed) {
    float x0 = ofMap(deterministicRandomPct(seed % GEOMETRY_PRIME + 1), 0, 1, 0, ofGetWidth());
    float y0 = ofMap(deterministicRandomPct(seed % GEOMETRY_PRIME + 2), 0, 1, 0, ofGetHeight());
    float z0 = 0;
    
    float x1 = ofMap(deterministicRandomPct(seed % GEOMETRY_PRIME + 4), 0, 1, 0, ofGetWidth());
    float y1 = ofMap(deterministicRandomPct(seed % GEOMETRY_PRIME + 5), 0, 1, 0, ofGetHeight());
    float z1 = 0;
    
    float t = -0.05 * ofGetWidth();
    float r = 1.05 * ofGetWidth();
    float b = 1.05 * ofGetHeight();
    float l = -0.05 * ofGetWidth();
    
    int c = static_cast<int>(ofMap(deterministicRandomPct(p.id % GEOMETRY_PRIME), 0, 1, 0, 12));
    
    ofVec3f from;
    ofVec3f to;
        
    switch (c) {
            // Top to
        case 0:
            from = ofVec3f(x0, t, z0);
            to = ofVec3f(r, y1, z1);
            break;
        case 1:
            from = ofVec3f(x0, t, z0);
            to = ofVec3f(x1, b, z1);
            break;
        case 2:
            from = ofVec3f(x0, t, z0);
            to = ofVec3f(l, y1, z1);
            break;
            
            // Right to
        case 3:
            from = ofVec3f(r, y0, z0);
            to = ofVec3f(x1, b, z1);
            break;
        case 4:
            from = ofVec3f(r, y0, z0);
            to = ofVec3f(l, y1, z1);
            break;
        case 5:
            from = ofVec3f(r, y0, z0);
            to = ofVec3f(x1, t, z1);
            break;
            
            // Bottom to
        case 6:
            from = ofVec3f(x0, b, z0);
            to = ofVec3f(l, y1, z1);
            break;
        case 7:
            from = ofVec3f(x0, b, z0);
            to = ofVec3f(x1, t, z1);
            break;
        case 8:
            from = ofVec3f(x0, b, z0);
            to = ofVec3f(r, y1, z1);
            break;
            
            // Left to
        case 9:
            from = ofVec3f(l, y0, z0);
            to = ofVec3f(x1, t, z0);
            break;
        case 10:
            from = ofVec3f(l, y0, z0);
            to = ofVec3f(r, y1, z1);
            break;
        case 11:
            from = ofVec3f(l, y0, z0);
            to = ofVec3f(x1, b, z1);
            break;
    }
    return std::pair<ofVec3f, ofVec3f>(from, to);
    
}

float getGaussian() { return disGaussian(gen); }

float getGaussian(float mu, float sigma) { return getGaussian() * sigma + mu; }

// Paired with shaderGlowLine. Makes Consistent parameters for glows easier. Check for usage
float getGlowDampenRatio(float glowIntensity, float computedGlow, float distance) {
    return pow(-log(1 - computedGlow), 1 / glowIntensity) * distance;
}

void kkEnableAlphaBlending() {
    if (ofGetStyle().blendingMode == OF_BLENDMODE_ALPHA) {
        ofLogWarning("kkEnableAlphaBlending") << "Redundant alpha enable.";
    }
    ofEnableAlphaBlending();
}

void kkDisableAlphaBlending() {
    if (ofGetStyle().blendingMode == OF_BLENDMODE_DISABLED) {
        ofLogWarning("kkDisableAlphaBlending") << "Redundant alpha disable.";
    }
    ofDisableAlphaBlending();
}

ofVec2f applyGlobalTransformation(ofVec2f input) { return ofVec2f(applyGlobalTransformation(ofVec3f(input))); }

ofVec3f applyGlobalTransformation(ofVec3f input) {
    // https://forum.openframeworks.cc/t/how-to-find-out-translated-coordinate-in-the-middle-of-draw/24866/10?u=kevinkatz
    glm::mat4 modelMatrix = glm::inverse(ofGetCurrentViewMatrix()) * ofGetCurrentMatrix(OF_MATRIX_MODELVIEW);
    auto transformed = modelMatrix * ofVec4f(input.x, input.y, input.z, 1);
    return ofVec3f(transformed.x, transformed.y, transformed.z);
}

float noteRelativeFrequency(int note) { return pow(TWELFTH_ROOT_TWO, note % NUM_NOTES); }

float midiToHz(int note) {
    // return pow(2, (note − 69) / 12) * 440.0;
    return pow(2, (note - 69) / NUM_NOTES) * 440.0;
}

float easeOutCubicTransitionFunction(const float pct) {
    // https://easings.net/#easeOutCubic
    // Good for velocityPct to lightness
    return 1 - pow(1 - pct, 3);
}

float pctToGain(float pct, float kurtosis) {
    // [kurtosis 1] -> [0.7, 1.41]
    // [kurtosis 2] -> [0.5, 2]
    // [0,1] -> 1.0 +/- modulation, modulation: [-1,1] (linear)
    //    float modulation = (pct - 0.5) * kurtosis;
    // float modulation = pow(pct / 2 + 0.5, 3)
    float result = pow(pow(2, pct - 0.5), kurtosis);
    return result;
}

float noiseForCoordinates(float x, float y, float spatialFrequency, float temporalRate, float timeS) {
    return ofSignedNoise((x - (ofGetWidth() / 2.0)) * spatialFrequency / 100,
                         (y - (ofGetHeight() / 2.0)) * spatialFrequency / 100, temporalRate * timeS);
}

ofVec3f noiseGradientForCoordinates(float x, float y, float spatialFrequency, float temporalRate, float noiseScale,
                                    float timeS) {
    float noiseHere = noiseForCoordinates(x, y, spatialFrequency, temporalRate, timeS);
    float dx = noiseForCoordinates(x + NOISE_GRADIENT_EPSILON, y, spatialFrequency, temporalRate, timeS) - noiseHere;
    float dy = noiseForCoordinates(x, y + NOISE_GRADIENT_EPSILON, spatialFrequency, temporalRate, timeS) - noiseHere;
    ofVec3f gradient = ofVec3f(dx, dy, 0) / NOISE_GRADIENT_EPSILON;
    return gradient * exp(noiseScale);
}

void drawNoiseVisualize(float spatialFrequency, float temporalRate, float noiseScale) {
    ofPushStyle();
    ofPushMatrix();
    ofSetLineWidth(1.0);
    float timeS = ofGetElapsedTimef();
    for (int x = 0; x < ofGetWidth(); x += 8) {
        for (int y = 0; y < ofGetHeight(); y += 8) {
            float noise = noiseForCoordinates(x, y, spatialFrequency, temporalRate, timeS);
            ofSetColor(ofMap(noise, -1, 1, 0, 255));
            ofVec3f gradient = noiseGradientForCoordinates(x, y, spatialFrequency, temporalRate, noiseScale, timeS);
            // ofDrawCircle(ofVec3f(x,y,0), 1.0);
            ofDrawLine(ofVec3f(x, y, 0), ofVec3f(x, y, 0) + gradient);
        }
    }
    ofPopMatrix();
    ofPopStyle();
}

namespace ColorUtilities {

ofColor withSaturation(const ofColor& c, uint8_t newSaturation) {
    float hue;
    float saturation;
    float value;
    c.getHsb(hue, saturation, value);
    return ofColor::fromHsb(hue, newSaturation, value);
}

ofColor withValue(const ofColor& c, uint8_t newValue) {
    float hue;
    float saturation;
    float value;
    c.getHsb(hue, saturation, value);
    return ofColor::fromHsb(hue, saturation, newValue);
}

ofColor generateNoisyColor(ofColor baseColor, float baseAlphaPct, float hueNoiseMaximumPct,
                           float saturationNoiseMaximumPct, float valueNoiseMaximumPct, float salt) {
    float hue;
    float saturation;
    float value;
    float hueNoiseMaximumOutOf255 = ofMap(hueNoiseMaximumPct, 0, 1, 0, 255);
    float saturationNoiseMaximumOutOf255 = ofMap(saturationNoiseMaximumPct, 0, 1, 0, 255);
    float valueNoiseMaximumOutOf255 = ofMap(valueNoiseMaximumPct, 0, 1, 0, 255);

    baseColor.getHsb(hue, saturation, value);

    float identity0 = deterministicRandomPct(salt + 0.0);
    float huePreModulus = ofMap(identity0, 0, 1.0, hue - hueNoiseMaximumOutOf255, hue + hueNoiseMaximumOutOf255);
    int h = static_cast<int>(positive_modulo(huePreModulus, 255.0));

    float identity1 = deterministicRandomPct(salt + 1.0);
    int s = ofMap(identity1, 0, 1.0, ofClamp(saturation - saturationNoiseMaximumOutOf255, 0, 255),
                  ofClamp(saturation + saturationNoiseMaximumOutOf255, 0, 255));

    float identity2 = deterministicRandomPct(salt + 2.0);
    int v = ofMap(identity2, 0, 1.0, ofClamp(value - valueNoiseMaximumOutOf255, 0, 255),
                  ofClamp(value + valueNoiseMaximumOutOf255, 0, 255));

    //    ofColor color = ofColor::fromHsb(h, s, v, a);
    ofColor color = ofColor::fromHsb(h, s, v);
    return color;
}

}  // namespace ColorUtilities

static std::unordered_map<std::string, MIDITYPE> const midiTypeTable = {
    {"note_on", MIDITYPE::NOTE_ON},
    {"note_off", MIDITYPE::NOTE_OFF},
    {"control_change", MIDITYPE::CONTROL_CHANGE},
    {"pitchwheel", MIDITYPE::PITCHWHEEL},
    {"keyboard_on", MIDITYPE::KEYBOARD_ON},
    {"keyboard_off", MIDITYPE::KEYBOARD_OFF},
    {"gui_parameter_change", MIDITYPE::GUI_PARAMETER_CHANGE},
    {"sysex", MIDITYPE::SYSEX},
    {"program_change", MIDITYPE::PROGRAMCHANGE}};

MIDITYPE stringToMidiType(std::string str) {
   auto it = midiTypeTable.find(str);
   if (it != midiTypeTable.end()) {
       return it->second;
   } else {
       ofLogError() << "Invalid Midi type string: " << str;
       return MIDITYPE::MIDITYPE_UNKNOWN;
   }
}
static std::unordered_map<std::string, CLASSIFICATIONTYPE> const classificationTypeTable = {
    {"arousal", CLASSIFICATIONTYPE::AROUSAL}};

CLASSIFICATIONTYPE stringToClassificationType(std::string str) {
    auto it = classificationTypeTable.find(str);
    if (it != classificationTypeTable.end()) {
        return it->second;
    } else {
        ofLogError() << "Invalid Classification type string: " << str;
        return CLASSIFICATIONTYPE::CLASSIFICATION_TYPE_UNKNOWN;
    }
}

static std::unordered_map<std::string, PARAMTYPE> const paramTypeTable = {{"colors", PARAMTYPE::COLORS}};

PARAMTYPE stringToParamType(std::string str) {
    auto it = paramTypeTable.find(str);
    if (it != paramTypeTable.end()) {
        return it->second;
    } else {
        ofLogError() << "Invalid Param type string: " << str;
        return PARAMTYPE::PARAM_TYPE_UNKNOWN;
    }
}
