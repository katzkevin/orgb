//
//  utilities.hpp
//  orgb
//
//  Created by Kevin Katz on 9/28/21.
//

#ifndef Utilities_hpp
#define Utilities_hpp

#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <optional>
#include <random>
#include <unordered_map>

#include "Press.hpp"
#include "core/MathUtils.hpp"
#include "core/Random.hpp"
#include "ofMain.h"  // Still needed for logging, drawing, etc.

#define NUM_NOTES 12
#define TWELFTH_ROOT_TWO 1.0594631
#define GOLDEN_RATIO 1.618033
#define PIANO_LOWEST_MIDI 21
#define MIDDLE_C_MIDI 60
#define PIANO_HIGHEST_MIDI 108
#define GUITAR_MIDI_MIN 44  // E1
#define GUITAR_MIDI_MAX 92  // E5
#define PIANO_MIDI_MIN 0
#define PIANO_MIDI_MAX 108
#define MIDI_NOTE_MAX 127

enum MIDITYPE {
    NOTE_ON = 0,
    NOTE_OFF,
    CONTROL_CHANGE,
    PITCHWHEEL,
    KEYBOARD_ON,
    KEYBOARD_OFF,
    GUI_PARAMETER_CHANGE,
    SYSEX,
    PROGRAMCHANGE,
    MIDITYPE_UNKNOWN
};
enum CLASSIFICATIONTYPE { AROUSAL = 0, CLASSIFICATION_TYPE_UNKNOWN };
enum PARAMTYPE { COLORS = 0, PARAM_TYPE_UNKNOWN };

double getSystemTimeSecondsPrecise();

// Performance monitoring - now accepts frame info as parameters
void warnOnSlow(std::string label, double t0, float thresholdSeconds, unsigned int frameNum, float elapsedTimeS,
                int perNFrames = 30, float warmupTimeS = 5.0);
void monitorFrameRate(float targetFrameRate, unsigned int frameNum, float elapsedTimeS, float currentFrameRate,
                      int perNFrames = 30, float warmupTimeS = 5.0);

int positive_modulo(int x, int y);
float positive_modulo(float x, float y);
glm::vec2 polarToRectangular(float r, float thetaRadians);
glm::vec3 sphericalToRectangular(float r, float thetaRadians, float phiRadians);
float exponentialMap(float value, float inputMin, float inputMax, float outputMin, float outputMax, bool clamp = true,
                     float shaper = 0.5);

float transitionEaseIn(float pct);
float transitionEaseInReverse(float pct);
float transitionEaseOutExponential(float pct, float exponent = 3);

std::mt19937 getRandomEngine(std::string seed);

unsigned int deterministicRandom(float salt);
float deterministicRandomPct(float salt);
glm::vec3 deterministicRandomUnitVector(float salt);
glm::vec3 getRandomlyRotatedVectorRad(glm::vec3 inVector, float radians);
glm::vec3 getRotatedAwayFromVectorRad(glm::vec3 inVector, glm::vec3 rotateAwayFrom, float radians);
glm::vec3 randomUnitVector();
glm::vec3 randomUnitVector2D();
std::pair<glm::vec3, glm::vec3> edgeToEdgeLineSegment(const Press & p, unsigned int seed);

float getGaussian();
float getGaussian(float mu, float sigma);

std::optional<std::string> getEnvOptional(std::string key);
std::string getEnv(std::string key);
std::string getEnv(std::string key, std::string defaultValue);

ofLogLevel getLogLevelEnum(std::string s);

float getGlowDampenRatio(float glowIntensity, float computedGlow, float distance);

void kkEnableAlphaBlending();
void kkDisableAlphaBlending();

glm::vec2 applyGlobalTransformation(glm::vec2 input);
glm::vec3 applyGlobalTransformation(glm::vec3 input);

float noteRelativeFrequency(int note);
float midiToHz(int note);
float easeOutCubicTransitionFunction(const float pct);

float pctToGain(float pct, float kurtosis);

float noiseForCoordinates(float x, float y, float spatialFrequency, float temporalRate, float timeS);
glm::vec3 noiseGradientForCoordinates(float x, float y, float spatialFrequency, float temporalRate, float noiseScale,
                                      float timeS);
void drawNoiseVisualize(float spatialFrequency, float temporalRate, float noiseScale, float elapsedTimeF);

namespace ColorUtilities {
ofColor generateNoisyColor(ofColor baseColor, float baseAlphaPct, float hueNoiseMaximumPct,
                           float saturationNoiseMaximumPct, float valueNoiseMaximumPct, float salt);
ofColor withSaturation(const ofColor & c, uint8_t newSaturation);
ofColor withValue(const ofColor & c, uint8_t newValue);
}  // namespace ColorUtilities

MIDITYPE stringToMidiType(std::string str);
CLASSIFICATIONTYPE stringToClassificationType(std::string str);
PARAMTYPE stringToParamType(std::string str);

#endif /* Utilities_hpp */
