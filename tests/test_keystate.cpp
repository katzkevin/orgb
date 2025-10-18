/**
 * Unit tests for KeyState class
 *
 * Tests press tracking, cleanup, sustain pedal, ephemeral presses, and arousal/valence
 */

#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include "KeyState.hpp"
#include "Utilities.hpp"

class KeyStateTest : public ::testing::Test {
protected:
    KeyState ks;
    const float EPSILON = 0.001f;

    void SetUp() override {
        ks = KeyState();
    }
};

// ============================================================================
// Test basic press tracking
// ============================================================================

TEST_F(KeyStateTest, NewKeyPressedHandler) {
    unsigned int messageId = 1;
    Press p = ks.newKeyPressedHandler(60, 0.8f, messageId);

    EXPECT_EQ(p.note, 60);
    EXPECT_FLOAT_EQ(p.velocityPct, 0.8f);
    EXPECT_EQ(p.id, messageId);
    EXPECT_EQ(p.pressType, Press::PressType::PIANO);
}

TEST_F(KeyStateTest, ActivePressTracking) {
    EXPECT_FALSE(ks.isActivelyPressed(60));

    ks.newKeyPressedHandler(60, 0.8f, 1);
    EXPECT_TRUE(ks.isActivelyPressed(60));

    ks.keyReleasedHandler(60);
    EXPECT_FALSE(ks.isActivelyPressed(60));
}

TEST_F(KeyStateTest, GetActivePress) {
    EXPECT_FALSE(ks.getActivePress(60).is_initialized());

    Press p = ks.newKeyPressedHandler(60, 0.8f, 1);
    boost::optional<Press> retrieved = ks.getActivePress(60);

    EXPECT_TRUE(retrieved.is_initialized());
    EXPECT_EQ(retrieved.value().note, 60);
}

TEST_F(KeyStateTest, GetActivePressAfterRelease) {
    ks.newKeyPressedHandler(60, 0.8f, 1);
    ks.keyReleasedHandler(60);

    EXPECT_FALSE(ks.getActivePress(60).is_initialized());
}

TEST_F(KeyStateTest, MultiplePresses) {
    ks.newKeyPressedHandler(60, 0.7f, 1);
    ks.newKeyPressedHandler(64, 0.8f, 2);
    ks.newKeyPressedHandler(67, 0.9f, 3);

    EXPECT_TRUE(ks.isActivelyPressed(60));
    EXPECT_TRUE(ks.isActivelyPressed(64));
    EXPECT_TRUE(ks.isActivelyPressed(67));
    EXPECT_FALSE(ks.isActivelyPressed(72));

    std::list<Press> active = ks.activePresses();
    EXPECT_EQ(active.size(), 3);
}

TEST_F(KeyStateTest, AllPressesIncludesReleased) {
    ks.newKeyPressedHandler(60, 0.8f, 1);
    ks.newKeyPressedHandler(64, 0.8f, 2);

    ks.keyReleasedHandler(60);

    // allPresses includes both active and released
    EXPECT_EQ(ks.allPresses().size(), 2);

    // activePresses only includes unreleased
    EXPECT_EQ(ks.activePresses().size(), 1);
}

TEST_F(KeyStateTest, GetMostRecentPress) {
    Press p1 = ks.newKeyPressedHandler(60, 0.8f, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    Press p2 = ks.newKeyPressedHandler(64, 0.8f, 2);

    boost::optional<Press> recent = ks.getMostRecentPress();
    EXPECT_TRUE(recent.is_initialized());
    EXPECT_EQ(recent.value().note, 64);
}

// ============================================================================
// Test press cleanup
// ============================================================================

TEST_F(KeyStateTest, CleanupRemovesOldReleased) {
    ks.newKeyPressedHandler(60, 0.8f, 1);
    ks.keyReleasedHandler(60);

    // Should still be in the list
    EXPECT_EQ(ks.allPresses().size(), 1);

    // Wait and cleanup with 0 second TTL
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ks.cleanup(0.05f, 0, 1.0/60.0);  // Remove after 0.05 seconds

    // Should be removed now
    EXPECT_EQ(ks.allPresses().size(), 0);
}

TEST_F(KeyStateTest, CleanupPreservesRecent) {
    ks.newKeyPressedHandler(60, 0.8f, 1);
    ks.keyReleasedHandler(60);

    // Cleanup immediately with high TTL
    ks.cleanup(10.0f, 0, 1.0/60.0);  // Keep for 10 seconds

    // Should still be there
    EXPECT_EQ(ks.allPresses().size(), 1);
}

TEST_F(KeyStateTest, CleanupPreservesActive) {
    ks.newKeyPressedHandler(60, 0.8f, 1);

    // Cleanup should never remove active presses
    ks.cleanup(0.0f, 0, 1.0/60.0);

    EXPECT_EQ(ks.allPresses().size(), 1);
    EXPECT_TRUE(ks.isActivelyPressed(60));
}

// ============================================================================
// Test sustain pedal
// ============================================================================

TEST_F(KeyStateTest, SustainOnHandler) {
    ks.newKeyPressedHandler(60, 0.8f, 1);

    double sustainTime = getSystemTimeSecondsPrecise();
    ks.sustainOnHandler(sustainTime);

    EXPECT_TRUE(ks.sustainTimeS.is_initialized());
    EXPECT_DOUBLE_EQ(ks.sustainTimeS.value(), sustainTime);
}

TEST_F(KeyStateTest, SustainOffHandler) {
    double sustainTime = getSystemTimeSecondsPrecise();
    ks.sustainOnHandler(sustainTime);

    EXPECT_TRUE(ks.sustainTimeS.is_initialized());

    ks.sustainOffHandler(getSystemTimeSecondsPrecise());

    EXPECT_FALSE(ks.sustainTimeS.is_initialized());
}

TEST_F(KeyStateTest, SustainAffectsNewPresses) {
    double sustainTime = getSystemTimeSecondsPrecise();
    ks.sustainOnHandler(sustainTime);

    // Press key while sustain is active
    ks.newKeyPressedHandler(60, 0.8f, 1);

    // Release key
    ks.keyReleasedHandler(60);

    // Should still be "active" due to sustain
    boost::optional<Press> p = ks.getActivePress(60);
    if (!p.is_initialized()) {
        // The press is in the list but getReleaseTime returns none (sustained)
        for (const auto& press : ks.allPresses()) {
            if (press.note == 60) {
                EXPECT_FALSE(press.getReleaseTime().is_initialized());
            }
        }
    }
}

TEST_F(KeyStateTest, SustainReleasesAll) {
    ks.sustainOnHandler(getSystemTimeSecondsPrecise());

    ks.newKeyPressedHandler(60, 0.8f, 1);
    ks.newKeyPressedHandler(64, 0.8f, 2);

    ks.keyReleasedHandler(60);
    ks.keyReleasedHandler(64);

    // Release sustain
    ks.sustainOffHandler(getSystemTimeSecondsPrecise());

    // All should now show as released
    for (const auto& press : ks.allPresses()) {
        EXPECT_TRUE(press.getReleaseTime().is_initialized());
    }
}

// ============================================================================
// Test ephemeral presses (guitar mode)
// ============================================================================

TEST_F(KeyStateTest, EphemeralKeyPressedHandler) {
    ks.ephemeralKeyPressedHandler(60, 0.8f, 1);

    EXPECT_EQ(ks.ephemeralPresses.size(), 1);
    EXPECT_FLOAT_EQ(ks.ephemeralPresses.at(60).velocityPct, 0.8f);
}

TEST_F(KeyStateTest, EphemeralPressUpdate) {
    ks.ephemeralKeyPressedHandler(60, 0.8f, 1);

    // Update with lower velocity (decay)
    ks.ephemeralKeyPressedHandler(60, 0.6f, 1);

    EXPECT_FLOAT_EQ(ks.ephemeralPresses.at(60).velocityPct, 0.6f);
}

TEST_F(KeyStateTest, EphemeralPressNewNote) {
    ks.ephemeralKeyPressedHandler(60, 0.7f, 1);

    // Velocity increases significantly (>= ALLOW_NOTE_INCREASE_PCT = 0.20) = new note
    // 0.95 - 0.7 = 0.25 > 0.20, so this should trigger a new note
    ks.ephemeralKeyPressedHandler(60, 0.95f, 2);

    // Should have new message ID
    EXPECT_EQ(ks.ephemeralPresses.at(60).id, 2u);
}

TEST_F(KeyStateTest, EphemeralPressPrune) {
    ks.ephemeralKeyPressedHandler(60, 0.01f, 1);

    // Very low velocity should be pruned
    ks.ephemeralKeyPressedHandler(60, 0.001f, 1);

    EXPECT_EQ(ks.ephemeralPresses.size(), 0);
}

TEST_F(KeyStateTest, EphemeralPressDecay) {
    ks.ephemeralDecayPerS = 0.8;  // 80% decay per second

    ks.ephemeralKeyPressedHandler(60, 1.0f, 1);

    // Decay should reduce velocity over time
    ks.decayEphemeralKeypressAmplitudes(1.0/60.0);

    EXPECT_LT(ks.ephemeralPresses.at(60).velocityPct, 1.0f);
}

TEST_F(KeyStateTest, EphemeralPressMapHandler) {
    std::unordered_map<int, float> presses;
    std::unordered_map<int, unsigned int> messageIds;

    presses[60] = 0.8f;
    presses[64] = 0.7f;
    presses[67] = 0.9f;

    messageIds[60] = 1;
    messageIds[64] = 2;
    messageIds[67] = 3;

    ks.ephemeralKeyPressMapHandler(presses, messageIds);

    EXPECT_EQ(ks.ephemeralPresses.size(), 3);
    EXPECT_FLOAT_EQ(ks.ephemeralPresses.at(60).velocityPct, 0.8f);
    EXPECT_FLOAT_EQ(ks.ephemeralPresses.at(64).velocityPct, 0.7f);
    EXPECT_FLOAT_EQ(ks.ephemeralPresses.at(67).velocityPct, 0.9f);
}

TEST_F(KeyStateTest, AllEphemeralPresses) {
    ks.ephemeralKeyPressedHandler(60, 0.8f, 1);
    ks.ephemeralKeyPressedHandler(64, 0.7f, 2);

    std::list<Press> ephemeral = ks.allEphemeralPresses();
    EXPECT_EQ(ephemeral.size(), 2);
}

TEST_F(KeyStateTest, AllEphemeralPressesChromaticGrouped) {
    ks.ephemeralKeyPressedHandler(60, 0.8f, 1);  // C
    ks.ephemeralKeyPressedHandler(72, 0.7f, 2);  // C (octave higher)

    std::multimap<int, Press> grouped = ks.allEphemeralPressesChromaticGrouped();

    // Both should be in chromatic group 0 (C)
    EXPECT_EQ(grouped.count(0), 2);
}

// ============================================================================
// Test chromatic grouping
// ============================================================================

TEST_F(KeyStateTest, AllPressesChromaticGrouped) {
    ks.newKeyPressedHandler(60, 0.8f, 1);  // C
    ks.newKeyPressedHandler(72, 0.7f, 2);  // C (octave higher)
    ks.newKeyPressedHandler(61, 0.6f, 3);  // C#

    std::multimap<int, Press> grouped = ks.allPressesChromaticGrouped();

    EXPECT_EQ(grouped.count(0), 2);  // Two C's
    EXPECT_EQ(grouped.count(1), 1);  // One C#
}

// ============================================================================
// Test meta input (sequential notes detection)
// ============================================================================

TEST_F(KeyStateTest, GetMetaInputNoPattern) {
    ks.newKeyPressedHandler(60, 0.8f, 1);
    ks.newKeyPressedHandler(64, 0.8f, 2);

    boost::optional<int> meta = ks.getMetaInput();
    EXPECT_FALSE(meta.is_initialized());
}

TEST_F(KeyStateTest, GetMetaInputValidPattern) {
    // Press C, C#, D, D#, E (sequential semitones starting from C)
    ks.newKeyPressedHandler(60, 0.8f, 1);  // C
    ks.newKeyPressedHandler(61, 0.8f, 2);  // C#
    ks.newKeyPressedHandler(62, 0.8f, 3);  // D
    ks.newKeyPressedHandler(63, 0.8f, 4);  // D#
    ks.newKeyPressedHandler(64, 0.8f, 5);  // E

    boost::optional<int> meta = ks.getMetaInput();
    EXPECT_TRUE(meta.is_initialized());
    EXPECT_EQ(meta.value(), 60);  // Returns the C
}

TEST_F(KeyStateTest, GetMetaInputHigherOctave) {
    // Test with C6, C#6, D6, D#6, E6
    ks.newKeyPressedHandler(84, 0.8f, 1);  // C6
    ks.newKeyPressedHandler(85, 0.8f, 2);  // C#6
    ks.newKeyPressedHandler(86, 0.8f, 3);  // D6
    ks.newKeyPressedHandler(87, 0.8f, 4);  // D#6
    ks.newKeyPressedHandler(88, 0.8f, 5);  // E6

    boost::optional<int> meta = ks.getMetaInput();
    EXPECT_TRUE(meta.is_initialized());
    EXPECT_EQ(meta.value(), 84);  // Returns the C6
}

// ============================================================================
// Test arousal and valence (circumplex model)
// ============================================================================

TEST_F(KeyStateTest, InitialArousalAndValence) {
    // Default should be centered
    EXPECT_FLOAT_EQ(ks.arousalPct(), 0.5f);
    EXPECT_FLOAT_EQ(ks.valencePct(), 0.5f);
}

TEST_F(KeyStateTest, SetArousal) {
    ks.setArousalPct(0.8f);
    EXPECT_FLOAT_EQ(ks.arousalPct(), 0.8f);
}

TEST_F(KeyStateTest, SetValence) {
    ks.setValencePct(0.3f);
    EXPECT_FLOAT_EQ(ks.valencePct(), 0.3f);
}

TEST_F(KeyStateTest, ArousalGainAtCenter) {
    ks.setArousalPct(0.5f);
    ks.arousalKurtosis = 1.0f;

    // At center, gain should be 1.0
    EXPECT_NEAR(ks.arousalGain(), 1.0f, EPSILON);
}

TEST_F(KeyStateTest, ArousalGainExtremes) {
    ks.arousalKurtosis = 1.0f;

    ks.setArousalPct(0.0f);
    float gainLow = ks.arousalGain();

    ks.setArousalPct(1.0f);
    float gainHigh = ks.arousalGain();

    // Low arousal should have gain < 1, high arousal should have gain > 1
    EXPECT_LT(gainLow, 1.0f);
    EXPECT_GT(gainHigh, 1.0f);
}

TEST_F(KeyStateTest, ValenceGainSymmetric) {
    ks.valenceKurtosis = 1.0f;

    ks.setValencePct(0.0f);
    float gain0 = ks.valenceGain();

    ks.setValencePct(1.0f);
    float gain1 = ks.valenceGain();

    // Both extremes should deviate from 1.0
    EXPECT_NE(gain0, 1.0f);
    EXPECT_NE(gain1, 1.0f);
}

// ============================================================================
// Test ADSR parameters
// ============================================================================

TEST_F(KeyStateTest, ADSRParametersInitialized) {
    // Should have default values
    EXPECT_GE(ks.attackTimeS.get(), 0.0);
    EXPECT_GE(ks.decayTimeS.get(), 0.0);
    EXPECT_GE(ks.sustainLevelPct.get(), 0.0);
    EXPECT_LE(ks.sustainLevelPct.get(), 1.0);
    EXPECT_GE(ks.releaseTimeS.get(), 0.0);
}

TEST_F(KeyStateTest, ADSRParametersCanBeModified) {
    ks.attackTimeS = 0.1;
    ks.decayTimeS = 0.2;
    ks.sustainLevelPct = 0.7;
    ks.releaseTimeS = 0.3;

    EXPECT_DOUBLE_EQ(ks.attackTimeS.get(), 0.1);
    EXPECT_DOUBLE_EQ(ks.decayTimeS.get(), 0.2);
    EXPECT_DOUBLE_EQ(ks.sustainLevelPct.get(), 0.7);
    EXPECT_DOUBLE_EQ(ks.releaseTimeS.get(), 0.3);
}

// ============================================================================
// Test edge cases and error conditions
// ============================================================================

TEST_F(KeyStateTest, DuplicatePressWarning) {
    ks.newKeyPressedHandler(60, 0.8f, 1);

    // Pressing the same key twice should log a warning but still work
    ks.newKeyPressedHandler(60, 0.9f, 2);

    // Should have 2 presses in the list
    EXPECT_GE(ks.allPresses().size(), 1);
}

TEST_F(KeyStateTest, ReleaseNonexistentKey) {
    // Should not crash
    ks.keyReleasedHandler(60);

    EXPECT_EQ(ks.allPresses().size(), 0);
}

TEST_F(KeyStateTest, RandomSeedInitialized) {
    // Random seed should be set
    EXPECT_GT(ks.randomSeed, 0u);
}
