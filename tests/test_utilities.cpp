/**
 * Unit tests for Utilities functions
 *
 * Tests mathematical utilities, coordinate transformations, and helper functions
 */

#include <gtest/gtest.h>
#include <cmath>
#include "Utilities.hpp"

// Test fixture for Utilities
class UtilitiesTest : public ::testing::Test {
protected:
    const float EPSILON = 0.0001f;
};

// ============================================================================
// Test positive_modulo
// ============================================================================

TEST_F(UtilitiesTest, PositiveModuloIntPositive) {
    EXPECT_EQ(positive_modulo(7, 3), 1);
    EXPECT_EQ(positive_modulo(9, 3), 0);
    EXPECT_EQ(positive_modulo(10, 3), 1);
}

TEST_F(UtilitiesTest, PositiveModuloIntNegative) {
    // Python-style wrapping modulo
    EXPECT_EQ(positive_modulo(-1, 3), 2);
    EXPECT_EQ(positive_modulo(-2, 3), 1);
    EXPECT_EQ(positive_modulo(-3, 3), 0);
    EXPECT_EQ(positive_modulo(-4, 3), 2);
}

TEST_F(UtilitiesTest, PositiveModuloFloatPositive) {
    EXPECT_NEAR(positive_modulo(7.5f, 3.0f), 1.5f, EPSILON);
    EXPECT_NEAR(positive_modulo(9.0f, 3.0f), 0.0f, EPSILON);
}

TEST_F(UtilitiesTest, PositiveModuloFloatNegative) {
    EXPECT_NEAR(positive_modulo(-1.5f, 3.0f), 1.5f, EPSILON);
    EXPECT_NEAR(positive_modulo(-2.0f, 3.0f), 1.0f, EPSILON);
}

TEST_F(UtilitiesTest, PositiveModuloHueWrapping) {
    // Common use case: wrapping hue values [0-255]
    EXPECT_NEAR(positive_modulo(256.0f, 255.0f), 1.0f, EPSILON);
    EXPECT_NEAR(positive_modulo(-1.0f, 255.0f), 254.0f, EPSILON);
}

// ============================================================================
// Test coordinate transformations
// ============================================================================

TEST_F(UtilitiesTest, PolarToRectangularZeroAngle) {
    glm::vec2 result = polarToRectangular(10.0f, 0.0f);
    EXPECT_NEAR(result.x, 10.0f, EPSILON);
    EXPECT_NEAR(result.y, 0.0f, EPSILON);
}

TEST_F(UtilitiesTest, PolarToRectangular90Degrees) {
    glm::vec2 result = polarToRectangular(10.0f, PI / 2.0f);
    EXPECT_NEAR(result.x, 0.0f, EPSILON);
    EXPECT_NEAR(result.y, 10.0f, EPSILON);
}

TEST_F(UtilitiesTest, PolarToRectangular180Degrees) {
    glm::vec2 result = polarToRectangular(10.0f, PI);
    EXPECT_NEAR(result.x, -10.0f, EPSILON);
    EXPECT_NEAR(result.y, 0.0f, EPSILON);
}

TEST_F(UtilitiesTest, SphericalToRectangularZeroAngles) {
    glm::vec3 result = sphericalToRectangular(10.0f, 0.0f, PI / 2.0f);
    EXPECT_NEAR(result.x, 10.0f, EPSILON);
    EXPECT_NEAR(result.y, 0.0f, EPSILON);
    EXPECT_NEAR(result.z, 0.0f, EPSILON);
}

// ============================================================================
// Test MIDI conversion functions
// ============================================================================

TEST_F(UtilitiesTest, MidiToHzMiddleC) {
    // Middle C (C4) = MIDI 60 = 261.63 Hz
    float result = midiToHz(60);
    EXPECT_NEAR(result, 261.63f, 0.01f);
}

TEST_F(UtilitiesTest, MidiToHzA440) {
    // A4 = MIDI 69 = 440 Hz (concert pitch)
    float result = midiToHz(69);
    EXPECT_NEAR(result, 440.0f, 0.01f);
}

TEST_F(UtilitiesTest, MidiToHzOctaveRelationship) {
    // Each octave doubles frequency
    float c3 = midiToHz(48);  // C3
    float c4 = midiToHz(60);  // C4
    float c5 = midiToHz(72);  // C5

    EXPECT_NEAR(c4 / c3, 2.0f, 0.01f);
    EXPECT_NEAR(c5 / c4, 2.0f, 0.01f);
}

TEST_F(UtilitiesTest, NoteRelativeFrequency) {
    // Test chromatic relationships
    EXPECT_NEAR(noteRelativeFrequency(0), 1.0f, EPSILON);  // C
    EXPECT_NEAR(noteRelativeFrequency(12), 1.0f, EPSILON); // C (next octave, same ratio)

    // Perfect fifth (7 semitones) should be ~1.5x frequency
    EXPECT_NEAR(noteRelativeFrequency(7), 1.498f, 0.01f);
}

// ============================================================================
// Test easing and transition functions
// ============================================================================

TEST_F(UtilitiesTest, TransitionEaseInBoundaries) {
    EXPECT_NEAR(transitionEaseIn(0.0f), 0.0f, EPSILON);
    EXPECT_NEAR(transitionEaseIn(1.0f), 1.0f, EPSILON);
}

TEST_F(UtilitiesTest, TransitionEaseInCurve) {
    // Cubic ease-in should be slow at start (below linear), accelerate at end
    EXPECT_LT(transitionEaseIn(0.25f), 0.25f);  // 0.25^3 = 0.015625 < 0.25
    EXPECT_LT(transitionEaseIn(0.5f), 0.5f);    // 0.5^3 = 0.125 < 0.5
    EXPECT_LT(transitionEaseIn(0.75f), 0.75f);  // 0.75^3 = 0.421875 < 0.75
    // But it should be accelerating (derivative increasing)
    float early = transitionEaseIn(0.25f);
    float late = transitionEaseIn(0.75f);
    EXPECT_GT(late - transitionEaseIn(0.5f), transitionEaseIn(0.5f) - early);
}

TEST_F(UtilitiesTest, TransitionEaseInReverseBoundaries) {
    EXPECT_NEAR(transitionEaseInReverse(0.0f), 1.0f, EPSILON);
    EXPECT_NEAR(transitionEaseInReverse(1.0f), 0.0f, EPSILON);
}

TEST_F(UtilitiesTest, TransitionEaseOutExponential) {
    EXPECT_NEAR(transitionEaseOutExponential(0.0f, 3.0f), 0.0f, EPSILON);
    EXPECT_NEAR(transitionEaseOutExponential(1.0f, 3.0f), 1.0f, EPSILON);

    // Should be fast at start (ease out)
    EXPECT_GT(transitionEaseOutExponential(0.25f, 3.0f), 0.25f);
    EXPECT_GT(transitionEaseOutExponential(0.5f, 3.0f), 0.5f);
}

TEST_F(UtilitiesTest, EaseOutCubicTransitionFunction) {
    EXPECT_NEAR(easeOutCubicTransitionFunction(0.0f), 0.0f, EPSILON);
    EXPECT_NEAR(easeOutCubicTransitionFunction(1.0f), 1.0f, EPSILON);

    // Fast at start, slow at end
    EXPECT_GT(easeOutCubicTransitionFunction(0.25f), 0.25f);
}

// ============================================================================
// Test exponentialMap
// ============================================================================

TEST_F(UtilitiesTest, ExponentialMapLinear) {
    // With shaper = 1.0, should be linear
    float result = exponentialMap(0.5f, 0.0f, 1.0f, 0.0f, 100.0f, true, 1.0f);
    EXPECT_NEAR(result, 50.0f, EPSILON);
}

TEST_F(UtilitiesTest, ExponentialMapSquare) {
    // With shaper = 2.0, should be quadratic
    float result = exponentialMap(0.5f, 0.0f, 1.0f, 0.0f, 100.0f, true, 2.0f);
    EXPECT_NEAR(result, 25.0f, EPSILON);
}

TEST_F(UtilitiesTest, ExponentialMapSquareRoot) {
    // With shaper = 0.5, should be square root
    float result = exponentialMap(0.25f, 0.0f, 1.0f, 0.0f, 100.0f, true, 0.5f);
    EXPECT_NEAR(result, 50.0f, EPSILON);
}

TEST_F(UtilitiesTest, ExponentialMapClamp) {
    // Test clamping behavior
    float result = exponentialMap(1.5f, 0.0f, 1.0f, 0.0f, 100.0f, true, 1.0f);
    EXPECT_NEAR(result, 100.0f, EPSILON);

    result = exponentialMap(-0.5f, 0.0f, 1.0f, 0.0f, 100.0f, true, 1.0f);
    EXPECT_NEAR(result, 0.0f, EPSILON);
}

// ============================================================================
// Test pctToGain (kurtosis function)
// ============================================================================

TEST_F(UtilitiesTest, PctToGainMidpoint) {
    // At 0.5, gain should be 1.0 regardless of kurtosis
    EXPECT_NEAR(pctToGain(0.5f, 1.0f), 1.0f, EPSILON);
    EXPECT_NEAR(pctToGain(0.5f, 2.0f), 1.0f, EPSILON);
    EXPECT_NEAR(pctToGain(0.5f, 3.0f), 1.0f, EPSILON);
}

TEST_F(UtilitiesTest, PctToGainExtremes) {
    // At 0, should be < 1
    EXPECT_LT(pctToGain(0.0f, 1.0f), 1.0f);

    // At 1, should be > 1
    EXPECT_GT(pctToGain(1.0f, 1.0f), 1.0f);
}

TEST_F(UtilitiesTest, PctToGainKurtosisEffect) {
    // Higher kurtosis = more extreme modulation
    float gain_low = pctToGain(0.0f, 1.0f);
    float gain_high = pctToGain(0.0f, 2.0f);

    EXPECT_LT(gain_high, gain_low);  // More extreme reduction
}

// ============================================================================
// Test deterministic random functions
// ============================================================================

TEST_F(UtilitiesTest, DeterministicRandomConsistency) {
    // Same salt should produce same result
    float salt = 42.0f;
    unsigned int r1 = deterministicRandom(salt);
    unsigned int r2 = deterministicRandom(salt);

    EXPECT_EQ(r1, r2);
}

TEST_F(UtilitiesTest, DeterministicRandomPctRange) {
    // Should return value in [0, 1]
    for (int i = 0; i < 100; i++) {
        float result = deterministicRandomPct(static_cast<float>(i));
        EXPECT_GE(result, 0.0f);
        EXPECT_LE(result, 1.0f);
    }
}

TEST_F(UtilitiesTest, DeterministicRandomPctDifferentSalts) {
    // Different salts should produce different results (with high probability)
    float r1 = deterministicRandomPct(1.0f);
    float r2 = deterministicRandomPct(2.0f);

    EXPECT_NE(r1, r2);
}

TEST_F(UtilitiesTest, DeterministicRandomUnitVectorLength) {
    // Unit vector should have length ~1
    glm::vec3 v = deterministicRandomUnitVector(42.0f);
    float length = glm::length(v);

    EXPECT_NEAR(length, 1.0f, 0.1f);  // Allow some tolerance for float math
}

// ============================================================================
// Test MIDI type string conversion
// ============================================================================

TEST_F(UtilitiesTest, StringToMidiTypeValid) {
    EXPECT_EQ(stringToMidiType("note_on"), MIDITYPE::NOTE_ON);
    EXPECT_EQ(stringToMidiType("note_off"), MIDITYPE::NOTE_OFF);
    EXPECT_EQ(stringToMidiType("control_change"), MIDITYPE::CONTROL_CHANGE);
    EXPECT_EQ(stringToMidiType("pitchwheel"), MIDITYPE::PITCHWHEEL);
}

TEST_F(UtilitiesTest, StringToMidiTypeInvalid) {
    EXPECT_EQ(stringToMidiType("invalid_type"), MIDITYPE::MIDITYPE_UNKNOWN);
    EXPECT_EQ(stringToMidiType(""), MIDITYPE::MIDITYPE_UNKNOWN);
}

// ============================================================================
// Test environment variable helpers
// ============================================================================

TEST_F(UtilitiesTest, GetEnvWithDefault) {
    // Test with non-existent key
    std::string result = getEnv("NONEXISTENT_KEY_12345", "default_value");
    EXPECT_EQ(result, "default_value");
}

TEST_F(UtilitiesTest, GetEnvOptionalMissing) {
    boost::optional<std::string> result = getEnvOptional("NONEXISTENT_KEY_12345");
    EXPECT_FALSE(result.is_initialized());
}
