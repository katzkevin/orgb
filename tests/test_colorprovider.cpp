/**
 * Unit tests for ColorProvider class
 *
 * Tests HSV palette generation, color mapping from MIDI notes, and interpolation
 */

#include <gtest/gtest.h>
#include "ColorProvider.hpp"
#include "Press.hpp"
#include "Utilities.hpp"

class ColorProviderTest : public ::testing::Test {
protected:
    ColorProvider cp;
    const float EPSILON = 0.01f;
    const int TOLERANCE = 5;  // Tolerance for HSV color values (out of 255)
};

// ============================================================================
// Test initialization
// ============================================================================

TEST_F(ColorProviderTest, DefaultInitialization) {
    // Should have default values
    EXPECT_GE(cp.baseHue.get(), 0.0f);
    EXPECT_LE(cp.baseHue.get(), 255.0f);

    EXPECT_GE(cp.baseSaturation.get(), 0.0f);
    EXPECT_LE(cp.baseSaturation.get(), 255.0f);

    EXPECT_GE(cp.baseValue.get(), 0.0f);
    EXPECT_LE(cp.baseValue.get(), 255.0f);
}

// ============================================================================
// Test palette generation
// ============================================================================

TEST_F(ColorProviderTest, SetPaletteMonochrome) {
    // All same hue, saturation, value = monochrome palette
    cp.setPalette(128.0f, 255.0f, 255.0f, 128.0f, 255.0f, 255.0f, true);

    // Create presses for different notes
    Press p1(60, 0.8f, 0.0, Press::PressType::PIANO, 1);  // C
    Press p2(64, 0.8f, 0.0, Press::PressType::PIANO, 2);  // E

    ofColor c1 = cp.color(p1);
    ofColor c2 = cp.color(p2);

    // Both should have roughly the same hue
    float h1, s1, v1, h2, s2, v2;
    c1.getHsb(h1, s1, v1);
    c2.getHsb(h2, s2, v2);

    EXPECT_NEAR(h1, h2, TOLERANCE);
}

TEST_F(ColorProviderTest, SetPaletteClockwiseHueProgression) {
    // Progress from red (0) to yellow (42) clockwise
    cp.setPalette(0.0f, 255.0f, 255.0f, 42.0f, 255.0f, 255.0f, true);

    Press pLow(60, 0.8f, 0.0, Press::PressType::PIANO, 1);   // Low note
    Press pHigh(71, 0.8f, 0.0, Press::PressType::PIANO, 2);  // High note (B)

    ofColor cLow = cp.color(pLow);
    ofColor cHigh = cp.color(pHigh);

    float hLow, sLow, vLow, hHigh, sHigh, vHigh;
    cLow.getHsb(hLow, sLow, vLow);
    cHigh.getHsb(hHigh, sHigh, vHigh);

    // High note should have higher hue (in clockwise direction)
    EXPECT_GT(hHigh, hLow);
}

TEST_F(ColorProviderTest, SetPaletteCounterClockwiseHueProgression) {
    // Progress counterclockwise
    cp.setPalette(42.0f, 255.0f, 255.0f, 0.0f, 255.0f, 255.0f, false);

    Press pLow(60, 0.8f, 0.0, Press::PressType::PIANO, 1);
    Press pHigh(71, 0.8f, 0.0, Press::PressType::PIANO, 2);

    ofColor cLow = cp.color(pLow);
    ofColor cHigh = cp.color(pHigh);

    // Colors should be different
    EXPECT_NE(cLow, cHigh);
}

TEST_F(ColorProviderTest, SetPaletteSaturationProgression) {
    // Test saturation changes
    cp.setPalette(128.0f, 100.0f, 255.0f, 128.0f, 255.0f, 255.0f, true);

    Press pLow(60, 0.8f, 0.0, Press::PressType::PIANO, 1);
    Press pHigh(71, 0.8f, 0.0, Press::PressType::PIANO, 2);

    ofColor cLow = cp.color(pLow);
    ofColor cHigh = cp.color(pHigh);

    float hLow, sLow, vLow, hHigh, sHigh, vHigh;
    cLow.getHsb(hLow, sLow, vLow);
    cHigh.getHsb(hHigh, sHigh, vHigh);

    // High note should have higher saturation
    EXPECT_GT(sHigh, sLow);
}

TEST_F(ColorProviderTest, SetPaletteValueProgression) {
    // Test value/brightness changes
    cp.setPalette(128.0f, 255.0f, 100.0f, 128.0f, 255.0f, 255.0f, true);

    Press pLow(60, 0.8f, 0.0, Press::PressType::PIANO, 1);
    Press pHigh(71, 0.8f, 0.0, Press::PressType::PIANO, 2);

    ofColor cLow = cp.color(pLow);
    ofColor cHigh = cp.color(pHigh);

    float hLow, sLow, vLow, hHigh, sHigh, vHigh;
    cLow.getHsb(hLow, sLow, vLow);
    cHigh.getHsb(hHigh, sHigh, vHigh);

    // High note should have higher value/brightness
    EXPECT_GT(vHigh, vLow);
}

// ============================================================================
// Test color mapping from notes
// ============================================================================

TEST_F(ColorProviderTest, ColorMappingCyclical) {
    cp.cyclical = true;
    cp.setPalette(0.0f, 255.0f, 255.0f, 255.0f, 255.0f, 255.0f, true);

    // C and C (octave higher) should have same color in cyclical mode
    Press p1(60, 0.8f, 0.0, Press::PressType::PIANO, 1);  // C4
    Press p2(72, 0.8f, 0.0, Press::PressType::PIANO, 2);  // C5

    ofColor c1 = cp.color(p1);
    ofColor c2 = cp.color(p2);

    float h1, s1, v1, h2, s2, v2;
    c1.getHsb(h1, s1, v1);
    c2.getHsb(h2, s2, v2);

    // Hues should be similar (same chromatic note)
    EXPECT_NEAR(h1, h2, TOLERANCE);
}

TEST_F(ColorProviderTest, ColorMappingNonCyclical) {
    cp.cyclical = false;
    cp.setPalette(0.0f, 255.0f, 255.0f, 255.0f, 255.0f, 255.0f, true);

    // C and C (octave higher) should have different colors
    Press p1(60, 0.8f, 0.0, Press::PressType::PIANO, 1);  // C4
    Press p2(72, 0.8f, 0.0, Press::PressType::PIANO, 2);  // C5

    ofColor c1 = cp.color(p1);
    ofColor c2 = cp.color(p2);

    // Colors should be different
    EXPECT_NE(c1, c2);
}

TEST_F(ColorProviderTest, ColorMappingChromaticScale) {
    cp.cyclical = true;
    cp.setPalette(0.0f, 255.0f, 255.0f, 255.0f, 255.0f, 255.0f, true);

    // All 12 chromatic notes should produce different colors
    std::vector<ofColor> colors;
    for (int note = 60; note < 72; note++) {
        Press p(note, 0.8f, 0.0, Press::PressType::PIANO, note);
        colors.push_back(cp.color(p));
    }

    // Check that we have 12 distinct hues (with some tolerance)
    std::set<int> uniqueHues;
    for (const auto& color : colors) {
        float h, s, v;
        color.getHsb(h, s, v);
        uniqueHues.insert(static_cast<int>(h / 10.0f));  // Group into 10-degree buckets
    }

    // Should have close to 12 unique hues (some may fall in same bucket)
    EXPECT_GE(uniqueHues.size(), 10);
}

// ============================================================================
// Test color with opacity
// ============================================================================

TEST_F(ColorProviderTest, ColorWithOpacityFull) {
    Press p(60, 0.8f, 0.0, Press::PressType::PIANO, 1);

    ofColor c = cp.color(p, 1.0);

    EXPECT_EQ(c.a, 255);
}

TEST_F(ColorProviderTest, ColorWithOpacityHalf) {
    Press p(60, 0.8f, 0.0, Press::PressType::PIANO, 1);

    ofColor c = cp.color(p, 0.5);

    EXPECT_NEAR(c.a, 127, 1);
}

TEST_F(ColorProviderTest, ColorWithOpacityZero) {
    Press p(60, 0.8f, 0.0, Press::PressType::PIANO, 1);

    ofColor c = cp.color(p, 0.0);

    EXPECT_EQ(c.a, 0);
}

TEST_F(ColorProviderTest, ColorWithOpacityPreservesRGB) {
    Press p(60, 0.8f, 0.0, Press::PressType::PIANO, 1);

    ofColor c1 = cp.color(p);
    ofColor c2 = cp.color(p, 0.5);

    // RGB should be the same, only alpha different
    EXPECT_EQ(c1.r, c2.r);
    EXPECT_EQ(c1.g, c2.g);
    EXPECT_EQ(c1.b, c2.b);
}

// ============================================================================
// Test hue wrapping
// ============================================================================

TEST_F(ColorProviderTest, HueWrappingClockwise) {
    // Test wrapping from 200 to 50 (crosses 0/255 boundary)
    cp.setPalette(200.0f, 255.0f, 255.0f, 50.0f, 255.0f, 255.0f, true);

    Press pLow(60, 0.8f, 0.0, Press::PressType::PIANO, 1);
    Press pHigh(71, 0.8f, 0.0, Press::PressType::PIANO, 2);

    ofColor cLow = cp.color(pLow);
    ofColor cHigh = cp.color(pHigh);

    // Both should have valid colors
    EXPECT_GT(cLow.r + cLow.g + cLow.b, 0);
    EXPECT_GT(cHigh.r + cHigh.g + cHigh.b, 0);
}

TEST_F(ColorProviderTest, HueWrappingCounterClockwise) {
    // Test wrapping counterclockwise
    cp.setPalette(50.0f, 255.0f, 255.0f, 200.0f, 255.0f, 255.0f, false);

    Press pLow(60, 0.8f, 0.0, Press::PressType::PIANO, 1);
    Press pHigh(71, 0.8f, 0.0, Press::PressType::PIANO, 2);

    ofColor cLow = cp.color(pLow);
    ofColor cHigh = cp.color(pHigh);

    // Both should have valid colors
    EXPECT_GT(cLow.r + cLow.g + cLow.b, 0);
    EXPECT_GT(cHigh.r + cHigh.g + cHigh.b, 0);
}

// ============================================================================
// Test edge cases
// ============================================================================

TEST_F(ColorProviderTest, BlackPalette) {
    // All black (value = 0)
    cp.setPalette(0.0f, 255.0f, 0.0f, 255.0f, 255.0f, 0.0f, true);

    Press p(60, 0.8f, 0.0, Press::PressType::PIANO, 1);
    ofColor c = cp.color(p);

    // Should be black
    EXPECT_EQ(c.r, 0);
    EXPECT_EQ(c.g, 0);
    EXPECT_EQ(c.b, 0);
}

TEST_F(ColorProviderTest, WhitePalette) {
    // All white (saturation = 0, value = 255)
    cp.setPalette(0.0f, 0.0f, 255.0f, 255.0f, 0.0f, 255.0f, true);

    Press p(60, 0.8f, 0.0, Press::PressType::PIANO, 1);
    ofColor c = cp.color(p);

    // Should be white or very close
    EXPECT_GT(c.r, 250);
    EXPECT_GT(c.g, 250);
    EXPECT_GT(c.b, 250);
}

TEST_F(ColorProviderTest, GrayscalePalette) {
    // Grayscale (saturation = 0)
    cp.setPalette(0.0f, 0.0f, 100.0f, 0.0f, 0.0f, 255.0f, true);

    Press p1(60, 0.8f, 0.0, Press::PressType::PIANO, 1);
    Press p2(71, 0.8f, 0.0, Press::PressType::PIANO, 2);

    ofColor c1 = cp.color(p1);
    ofColor c2 = cp.color(p2);

    // Should be grayscale (R == G == B)
    EXPECT_EQ(c1.r, c1.g);
    EXPECT_EQ(c1.g, c1.b);
    EXPECT_EQ(c2.r, c2.g);
    EXPECT_EQ(c2.g, c2.b);

    // Different brightness levels
    EXPECT_NE(c1.r, c2.r);
}

TEST_F(ColorProviderTest, ExtremeMIDINotes) {
    // Use a reasonable hue range that spans enough degrees for 12 distinct colors
    // Red (0) to green (85) clockwise = 85 degree range / 12 notes ≈ 7 degrees per note
    cp.setPalette(0.0f, 255.0f, 255.0f, 85.0f, 255.0f, 255.0f, true);

    // Test with MIDI note 0 and 127 (different chromatic positions)
    Press pMin(0, 0.8f, 0.0, Press::PressType::PIANO, 1);   // Note 0 % 12 = 0 (C)
    Press pMax(127, 0.8f, 0.0, Press::PressType::PIANO, 2); // Note 127 % 12 = 7 (G)

    ofColor cMin = cp.color(pMin);
    ofColor cMax = cp.color(pMax);

    // Both should be valid colors
    EXPECT_GT(cMin.r + cMin.g + cMin.b, 0);
    EXPECT_GT(cMax.r + cMax.g + cMax.b, 0);

    // With cyclical mode, note 0 (C) and note 127 (G) should have different colors
    EXPECT_NE(cMin, cMax);
}

// ============================================================================
// Test parameter changes
// ============================================================================

TEST_F(ColorProviderTest, ParameterModification) {
    cp.baseHue = 100.0f;
    cp.baseSaturation = 200.0f;
    cp.baseValue = 255.0f;

    EXPECT_FLOAT_EQ(cp.baseHue.get(), 100.0f);
    EXPECT_FLOAT_EQ(cp.baseSaturation.get(), 200.0f);
    EXPECT_FLOAT_EQ(cp.baseValue.get(), 255.0f);
}

TEST_F(ColorProviderTest, ParameterBoundsRespected) {
    // ofParameter doesn't auto-clamp on assignment, only on .set()
    // Test that parameters were initialized with correct bounds
    EXPECT_EQ(cp.baseHue.getMin(), 0.0f);
    EXPECT_EQ(cp.baseHue.getMax(), 255.0f);
    EXPECT_EQ(cp.baseSaturation.getMin(), 0.0f);
    EXPECT_EQ(cp.baseSaturation.getMax(), 255.0f);

    // Values within bounds should work
    cp.baseHue = 128.0f;
    cp.baseSaturation = 200.0f;
    EXPECT_FLOAT_EQ(cp.baseHue.get(), 128.0f);
    EXPECT_FLOAT_EQ(cp.baseSaturation.get(), 200.0f);
}

TEST_F(ColorProviderTest, ClockwiseParameterToggle) {
    cp.clockwise = true;
    EXPECT_TRUE(cp.clockwise.get());

    cp.clockwise = false;
    EXPECT_FALSE(cp.clockwise.get());
}

TEST_F(ColorProviderTest, CyclicalParameterToggle) {
    cp.cyclical = true;
    EXPECT_TRUE(cp.cyclical.get());

    cp.cyclical = false;
    EXPECT_FALSE(cp.cyclical.get());
}

// ============================================================================
// Test custom palette
// ============================================================================

TEST_F(ColorProviderTest, SetCustomPalette) {
    std::vector<ofColor> customColors = {
        ofColor::red,
        ofColor::green,
        ofColor::blue,
        ofColor::yellow,
        ofColor::cyan,
        ofColor::magenta,
        ofColor::white,
        ofColor::black,
        ofColor::red,  // Back to red
        ofColor::green,
        ofColor::blue,
        ofColor::yellow
    };

    cp.setPalette(customColors);

    // Test that colors are mapped correctly
    Press p(60, 0.8f, 0.0, Press::PressType::PIANO, 1);  // First note (C)
    ofColor c = cp.color(p);

    // Should be red or close to it
    EXPECT_GT(c.r, c.g);
    EXPECT_GT(c.r, c.b);
}
