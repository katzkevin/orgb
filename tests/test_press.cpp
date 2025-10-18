/**
 * Unit tests for Press class
 *
 * Tests press lifecycle, sustain pedal logic, and ADSR envelope calculations
 */

#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "Press.hpp"
#include "Utilities.hpp"

class PressTest : public ::testing::Test {
   protected:
    const float EPSILON = 0.001f;
    const unsigned int TEST_MESSAGE_ID = 12345;
};

// ============================================================================
// Test Press construction
// ============================================================================

TEST_F(PressTest, ConstructorInitialization) {
    double pressTime = getSystemTimeSecondsPrecise();
    Press p(60, 0.8f, pressTime, Press::PressType::PIANO, TEST_MESSAGE_ID);

    EXPECT_EQ(p.note, 60);
    EXPECT_FLOAT_EQ(p.velocityPct, 0.8f);
    EXPECT_DOUBLE_EQ(p.tSystemTimeSeconds, pressTime);
    EXPECT_EQ(p.pressType, Press::PressType::PIANO);
    EXPECT_EQ(p.id, TEST_MESSAGE_ID);
    EXPECT_FALSE(p.getReleaseTime().is_initialized());
}

TEST_F(PressTest, GuitarPressType) {
    double pressTime = getSystemTimeSecondsPrecise();
    Press p(72, 0.5f, pressTime, Press::PressType::GUITAR, TEST_MESSAGE_ID);

    EXPECT_EQ(p.pressType, Press::PressType::GUITAR);
}

// ============================================================================
// Test Press lifecycle: Simple press and release
// ============================================================================

TEST_F(PressTest, SimpleRelease) {
    double pressTime = getSystemTimeSecondsPrecise();
    Press p(60, 0.8f, pressTime, Press::PressType::PIANO, TEST_MESSAGE_ID);

    // Initially not released
    EXPECT_FALSE(p.getReleaseTime().is_initialized());

    // Release after 0.1 seconds
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    double releaseTime = getSystemTimeSecondsPrecise();
    p.setReleased(releaseTime);

    EXPECT_TRUE(p.getReleaseTime().is_initialized());
    EXPECT_DOUBLE_EQ(p.getReleaseTime().value(), releaseTime);
}

TEST_F(PressTest, MultipleReleaseCallsLastWins) {
    double pressTime = getSystemTimeSecondsPrecise();
    Press p(60, 0.8f, pressTime, Press::PressType::PIANO, TEST_MESSAGE_ID);

    double releaseTime1 = pressTime + 0.1;
    p.setReleased(releaseTime1);

    double releaseTime2 = pressTime + 0.2;
    p.setReleased(releaseTime2);

    EXPECT_DOUBLE_EQ(p.getReleaseTime().value(), releaseTime2);
}

// ============================================================================
// Test sustain pedal logic
// ============================================================================

TEST_F(PressTest, SustainBeforeRelease) {
    double pressTime = getSystemTimeSecondsPrecise();
    Press p(60, 0.8f, pressTime, Press::PressType::PIANO, TEST_MESSAGE_ID);

    // Press sustain pedal
    double sustainTime = pressTime + 0.1;
    p.setSustained(sustainTime);

    // Release key (but sustain pedal still held)
    double keyReleaseTime = pressTime + 0.2;
    p.setReleased(keyReleaseTime);

    // Press should still be sustained (not released yet)
    EXPECT_FALSE(p.getReleaseTime().is_initialized());

    // Release sustain pedal
    double sustainReleaseTime = pressTime + 0.3;
    p.releaseSustain(sustainReleaseTime);

    // Now the press should be released at the sustain release time
    EXPECT_TRUE(p.getReleaseTime().is_initialized());
    EXPECT_DOUBLE_EQ(p.getReleaseTime().value(), sustainReleaseTime);
}

TEST_F(PressTest, SustainAfterKeyRelease) {
    double pressTime = getSystemTimeSecondsPrecise();
    Press p(60, 0.8f, pressTime, Press::PressType::PIANO, TEST_MESSAGE_ID);

    // Press and hold sustain pedal before key press
    double sustainTime = pressTime - 0.1;
    p.setSustained(sustainTime);

    // Release key while sustain pedal is held
    double keyReleaseTime = pressTime + 0.1;
    p.setReleased(keyReleaseTime);

    // Should not be released yet (sustained)
    EXPECT_FALSE(p.getReleaseTime().is_initialized());

    // Release sustain pedal
    double sustainReleaseTime = pressTime + 0.2;
    p.releaseSustain(sustainReleaseTime);

    EXPECT_TRUE(p.getReleaseTime().is_initialized());
    EXPECT_DOUBLE_EQ(p.getReleaseTime().value(), sustainReleaseTime);
}

TEST_F(PressTest, SustainReleasedBeforeKeyRelease) {
    double pressTime = getSystemTimeSecondsPrecise();
    Press p(60, 0.8f, pressTime, Press::PressType::PIANO, TEST_MESSAGE_ID);

    // Sustain pedal pressed
    p.setSustained(pressTime + 0.1);

    // Sustain pedal released
    double sustainReleaseTime = pressTime + 0.2;
    p.releaseSustain(sustainReleaseTime);

    // Key still held, then released
    double keyReleaseTime = pressTime + 0.3;
    p.setReleased(keyReleaseTime);

    // Should use key release time (sustain had no effect)
    EXPECT_TRUE(p.getReleaseTime().is_initialized());
    EXPECT_DOUBLE_EQ(p.getReleaseTime().value(), keyReleaseTime);
}

TEST_F(PressTest, SustainAfterAlreadyReleased) {
    double pressTime = getSystemTimeSecondsPrecise();
    Press p(60, 0.8f, pressTime, Press::PressType::PIANO, TEST_MESSAGE_ID);

    // Release key first
    double keyReleaseTime = pressTime + 0.1;
    p.setReleased(keyReleaseTime);

    // Try to sustain after release (should have no effect)
    p.setSustained(pressTime + 0.2);

    // Should still be released at original time
    EXPECT_TRUE(p.getReleaseTime().is_initialized());
    EXPECT_DOUBLE_EQ(p.getReleaseTime().value(), keyReleaseTime);
}

TEST_F(PressTest, MultipleSustainCycles) {
    double pressTime = getSystemTimeSecondsPrecise();
    Press p(60, 0.8f, pressTime, Press::PressType::PIANO, TEST_MESSAGE_ID);

    // First sustain cycle
    p.setSustained(pressTime + 0.1);
    p.releaseSustain(pressTime + 0.2);

    // Second sustain cycle (overwrites first)
    p.setSustained(pressTime + 0.3);

    // Release key
    p.setReleased(pressTime + 0.4);

    // Should still be sustained
    EXPECT_FALSE(p.getReleaseTime().is_initialized());

    // Release second sustain
    p.releaseSustain(pressTime + 0.5);

    EXPECT_TRUE(p.getReleaseTime().is_initialized());
    EXPECT_DOUBLE_EQ(p.getReleaseTime().value(), pressTime + 0.5);
}

// ============================================================================
// Test note percentage calculations
// ============================================================================

TEST_F(PressTest, NoteOverallPctPiano) {
    Press pLow(PIANO_MIDI_MIN, 0.5f, 0.0, Press::PressType::PIANO, 1);
    Press pHigh(PIANO_MIDI_MAX, 0.5f, 0.0, Press::PressType::PIANO, 2);
    Press pMid(54, 0.5f, 0.0, Press::PressType::PIANO, 3);  // F#3

    EXPECT_NEAR(pLow.noteOverallPct(), 0.0f, EPSILON);
    EXPECT_NEAR(pHigh.noteOverallPct(), 1.0f, EPSILON);
    EXPECT_GT(pMid.noteOverallPct(), 0.0f);
    EXPECT_LT(pMid.noteOverallPct(), 1.0f);
}

TEST_F(PressTest, NoteOverallPctGuitar) {
    Press pLow(GUITAR_MIDI_MIN, 0.5f, 0.0, Press::PressType::GUITAR, 1);
    Press pHigh(GUITAR_MIDI_MAX, 0.5f, 0.0, Press::PressType::GUITAR, 2);

    EXPECT_NEAR(pLow.noteOverallPct(), 0.0f, EPSILON);
    EXPECT_NEAR(pHigh.noteOverallPct(), 1.0f, EPSILON);
}

TEST_F(PressTest, NoteChromaticPct) {
    // C should be 0, B should be close to 1
    Press pC(60, 0.5f, 0.0, Press::PressType::PIANO, 1);       // C
    Press pCSharp(61, 0.5f, 0.0, Press::PressType::PIANO, 2);  // C#
    Press pB(71, 0.5f, 0.0, Press::PressType::PIANO, 3);       // B

    EXPECT_NEAR(pC.noteChromaticPct(), 0.0f, EPSILON);
    EXPECT_NEAR(pCSharp.noteChromaticPct(), 1.0f / 12.0f, EPSILON);
    EXPECT_NEAR(pB.noteChromaticPct(), 11.0f / 12.0f, EPSILON);
}

// ============================================================================
// Test ADSR envelope calculation
// ============================================================================

TEST_F(PressTest, ADSRDuringAttack) {
    double pressTime = getSystemTimeSecondsPrecise();
    Press p(60, 0.8f, pressTime, Press::PressType::PIANO, TEST_MESSAGE_ID);

    // Test during attack phase
    double attackTime = 0.1;
    double decayTime = 0.2;
    double sustainLevel = 0.7;
    double releaseTime = 0.3;

    // Wait a bit (but less than attack time)
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    double amplitude = p.audibleAmplitudePct(attackTime, decayTime, sustainLevel, releaseTime);

    // Should be between 0 and 1, rising
    EXPECT_GT(amplitude, 0.0);
    EXPECT_LT(amplitude, 1.0);
}

TEST_F(PressTest, ADSRFullAttack) {
    double pressTime = getSystemTimeSecondsPrecise();
    Press p(60, 0.8f, pressTime, Press::PressType::PIANO, TEST_MESSAGE_ID);

    double attackTime = 0.05;
    double decayTime = 0.2;
    double sustainLevel = 0.7;
    double releaseTime = 0.3;

    // Wait for full attack
    std::this_thread::sleep_for(std::chrono::milliseconds(60));

    double amplitude = p.audibleAmplitudePct(attackTime, decayTime, sustainLevel, releaseTime);

    // Should have reached peak and started decay
    EXPECT_GT(amplitude, sustainLevel);
    EXPECT_LE(amplitude, 1.0);
}

TEST_F(PressTest, ADSRInstantAttack) {
    double pressTime = getSystemTimeSecondsPrecise();
    Press p(60, 0.8f, pressTime, Press::PressType::PIANO, TEST_MESSAGE_ID);

    double attackTime = 0.0;  // Instant attack
    double decayTime = 0.2;
    double sustainLevel = 0.7;
    double releaseTime = 0.3;

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    double amplitude = p.audibleAmplitudePct(attackTime, decayTime, sustainLevel, releaseTime);

    // Should immediately reach peak and start decay
    EXPECT_GT(amplitude, sustainLevel);
    EXPECT_LE(amplitude, 1.0);
}

TEST_F(PressTest, ADSRAfterRelease) {
    double pressTime = getSystemTimeSecondsPrecise();
    Press p(60, 0.8f, pressTime, Press::PressType::PIANO, TEST_MESSAGE_ID);

    double attackTime = 0.01;
    double decayTime = 0.01;
    double sustainLevel = 0.7;
    double releaseTime = 0.1;

    // Wait to reach sustain
    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    // Release
    p.setReleased(getSystemTimeSecondsPrecise());

    // Wait during release
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    double amplitude = p.audibleAmplitudePct(attackTime, decayTime, sustainLevel, releaseTime);

    // Should be releasing (between 0 and sustain level)
    EXPECT_GE(amplitude, 0.0);
    EXPECT_LT(amplitude, sustainLevel);
}

TEST_F(PressTest, ADSRFullyReleased) {
    double pressTime = getSystemTimeSecondsPrecise();
    Press p(60, 0.8f, pressTime, Press::PressType::PIANO, TEST_MESSAGE_ID);

    double attackTime = 0.01;
    double decayTime = 0.01;
    double sustainLevel = 0.7;
    double releaseTime = 0.05;

    // Immediately release
    p.setReleased(pressTime + 0.03);

    // Wait for full release
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    double amplitude = p.audibleAmplitudePct(attackTime, decayTime, sustainLevel, releaseTime);

    // Should be silent
    EXPECT_NEAR(amplitude, 0.0, EPSILON);
}

TEST_F(PressTest, ADSRZeroReleaseTime) {
    double pressTime = getSystemTimeSecondsPrecise();
    Press p(60, 0.8f, pressTime, Press::PressType::PIANO, TEST_MESSAGE_ID);

    double attackTime = 0.01;
    double decayTime = 0.01;
    double sustainLevel = 0.7;
    double releaseTime = 0.0;  // Instant release

    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    // Release
    p.setReleased(getSystemTimeSecondsPrecise());

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    double amplitude = p.audibleAmplitudePct(attackTime, decayTime, sustainLevel, releaseTime);

    // Should drop to zero immediately
    EXPECT_NEAR(amplitude, 0.0, EPSILON);
}

// ============================================================================
// Test Press equality and hashing
// ============================================================================

TEST_F(PressTest, EqualityOperator) {
    Press p1(60, 0.8f, 1.0, Press::PressType::PIANO, 123);
    Press p2(60, 0.8f, 1.0, Press::PressType::PIANO, 123);
    Press p3(60, 0.8f, 1.0, Press::PressType::PIANO, 456);

    EXPECT_TRUE(p1 == p2);   // Same ID
    EXPECT_FALSE(p1 == p3);  // Different ID
}

TEST_F(PressTest, LessThanOperator) {
    Press p1(60, 0.8f, 1.0, Press::PressType::PIANO, 100);
    Press p2(60, 0.8f, 1.0, Press::PressType::PIANO, 200);

    EXPECT_TRUE(p1 < p2);
    EXPECT_FALSE(p2 < p1);
}

TEST_F(PressTest, HashConsistency) {
    Press p1(60, 0.8f, 1.0, Press::PressType::PIANO, 123);
    Press p2(60, 0.8f, 1.0, Press::PressType::PIANO, 123);

    std::hash<Press> hasher;
    EXPECT_EQ(hasher(p1), hasher(p2));
}
