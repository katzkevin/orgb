#include <gtest/gtest.h>

#include "../src/Forms/Flock.hpp"
#include "../src/Forms/SizedSprite.hpp"
#include "ofMain.h"

class FlockTest : public ::testing::Test {
   protected:
    void SetUp() override {
        flock = Flock();

        // Set up default parameters
        flock.fastForward = 1.0f;
        flock.visualRange = 100.0f;
        flock.minDistance = 20.0f;
        flock.avoidFactor = 0.05f;
        flock.alignmentFactor = 0.05f;
        flock.centeringFactor = 0.0005f;
        flock.turnFactor = 0.5f;
        flock.minVelocity = 3.0f;
        flock.maxVelocity = 6.0f;
        flock.margin = 100.0f;
        flock.xMin = 0.0f;
        flock.xMax = 1920.0f;
        flock.yMin = 0.0f;
        flock.yMax = 1080.0f;
        flock.zMin = -500.0f;
        flock.zMax = 500.0f;
    }

    Flock flock;
};

// ============================================================================
// Test initialization
// ============================================================================

TEST_F(FlockTest, DefaultConstruction) {
    Flock f;
    EXPECT_EQ(f.l.size(), 0);
}

TEST_F(FlockTest, AddSprites) {
    flock.l.push_back(SizedSprite(glm::vec3(100, 100, 0), glm::vec3(1, 0, 0), ofColor::red, 5.0f));
    flock.l.push_back(SizedSprite(glm::vec3(200, 200, 0), glm::vec3(-1, 0, 0), ofColor::blue, 5.0f));

    EXPECT_EQ(flock.l.size(), 2);
    EXPECT_FLOAT_EQ(flock.l[0].position.x, 100.0f);
    EXPECT_FLOAT_EQ(flock.l[1].position.x, 200.0f);
}

// ============================================================================
// Test update mechanics
// ============================================================================

TEST_F(FlockTest, UpdateMovesParticles) {
    glm::vec3 initialPos(500, 500, 0);
    glm::vec3 velocity(10, 0, 0);
    flock.l.push_back(SizedSprite(initialPos, velocity, ofColor::white, 5.0f));

    float dt = 1.0f / 60.0f;  // One frame at 60fps
    flock.update(dt, 0.0f);

    // Position should have changed based on velocity
    EXPECT_GT(flock.l[0].position.x, initialPos.x);
}

TEST_F(FlockTest, VelocityClampingMin) {
    // Create a sprite with very low velocity
    flock.l.push_back(SizedSprite(glm::vec3(500, 500, 0), glm::vec3(0.1f, 0, 0), ofColor::white, 5.0f));

    float dt = 1.0f / 60.0f;
    flock.update(dt, 0.0f);

    // Velocity should be clamped to minVelocity (3.0)
    float speed = glm::length(flock.l[0].velocity);
    EXPECT_GE(speed, flock.minVelocity - 0.01f);  // Allow small epsilon for floating point
}

TEST_F(FlockTest, VelocityClampingMax) {
    // Create a sprite with very high velocity
    flock.l.push_back(SizedSprite(glm::vec3(500, 500, 0), glm::vec3(100, 0, 0), ofColor::white, 5.0f));

    float dt = 1.0f / 60.0f;
    flock.update(dt, 0.0f);

    // Velocity should be clamped to maxVelocity (6.0)
    float speed = glm::length(flock.l[0].velocity);
    EXPECT_LE(speed, flock.maxVelocity + 0.01f);  // Allow small epsilon for floating point
}

// ============================================================================
// Test flocking behavior - Separation
// ============================================================================

TEST_F(FlockTest, SeparationBehavior) {
    // Create two sprites very close together
    glm::vec3 pos1(500, 500, 0);
    glm::vec3 pos2(510, 500, 0);  // 10 units apart, well within minDistance (20)

    flock.l.push_back(SizedSprite(pos1, glm::vec3(0, 0, 0), ofColor::white, 5.0f));
    flock.l.push_back(SizedSprite(pos2, glm::vec3(0, 0, 0), ofColor::white, 5.0f));

    float dt = 1.0f / 60.0f;
    flock.update(dt, 0.0f);

    // After update, sprites should be moving away from each other
    // First sprite should have negative x velocity (moving left, away from sprite 2)
    // Second sprite should have positive x velocity (moving right, away from sprite 1)
    EXPECT_LT(flock.l[0].velocity.x, 0.0f);
    EXPECT_GT(flock.l[1].velocity.x, 0.0f);
}

// ============================================================================
// Test flocking behavior - Alignment
// ============================================================================

TEST_F(FlockTest, AlignmentBehavior) {
    // Create two sprites within visual range with different velocities
    glm::vec3 pos1(500, 500, 0);
    glm::vec3 pos2(550, 500, 0);  // 50 units apart, within visualRange (100)
    glm::vec3 vel1(5, 0, 0);
    glm::vec3 vel2(0, 5, 0);

    flock.l.push_back(SizedSprite(pos1, vel1, ofColor::white, 5.0f));
    flock.l.push_back(SizedSprite(pos2, vel2, ofColor::white, 5.0f));

    glm::vec3 vel1_before = flock.l[0].velocity;
    glm::vec3 vel2_before = flock.l[1].velocity;

    float dt = 1.0f / 60.0f;
    flock.update(dt, 0.0f);

    // After update, velocities should be more aligned
    // Sprite 1 should gain some y component
    // Sprite 2 should gain some x component
    EXPECT_GT(fabs(flock.l[0].velocity.y), fabs(vel1_before.y));
    EXPECT_GT(fabs(flock.l[1].velocity.x), fabs(vel2_before.x));
}

// ============================================================================
// Test flocking behavior - Cohesion
// ============================================================================

TEST_F(FlockTest, CohesionBehavior) {
    // Create two sprites within visual range with initial velocities
    glm::vec3 pos1(400, 500, 0);
    glm::vec3 pos2(600, 500, 0);  // 200 units apart, but we'll increase visualRange

    flock.visualRange = 300.0f;     // Make sure they see each other
    flock.minDistance = 10.0f;      // But not so close they separate
    flock.centeringFactor = 0.05f;  // Increase to make effect more visible

    // Give them perpendicular initial velocities to see cohesion pull them together
    flock.l.push_back(SizedSprite(pos1, glm::vec3(0, 4, 0), ofColor::white, 5.0f));
    flock.l.push_back(SizedSprite(pos2, glm::vec3(0, 4, 0), ofColor::white, 5.0f));

    glm::vec3 vel1Before = flock.l[0].velocity;
    glm::vec3 vel2Before = flock.l[1].velocity;

    float dt = 1.0f / 60.0f;
    flock.update(dt, 0.0f);

    // After update, cohesion should pull them towards each other
    // Sprite 1 should gain positive x component (towards sprite 2)
    // Sprite 2 should gain negative x component (towards sprite 1)
    EXPECT_GT(flock.l[0].velocity.x, vel1Before.x);
    EXPECT_LT(flock.l[1].velocity.x, vel2Before.x);
}

// ============================================================================
// Test boundary avoidance
// ============================================================================

TEST_F(FlockTest, BoundaryAvoidanceLeft) {
    // Create a sprite near the left boundary heading towards it
    glm::vec3 pos(50, 500, 0);  // Within margin (100) of xMin (0)
    glm::vec3 initialVel(-4, 0, 0);
    flock.l.push_back(SizedSprite(pos, initialVel, ofColor::white, 5.0f));

    float dt = 1.0f / 60.0f;
    flock.update(dt, 0.0f);

    // Boundary avoidance should increase x velocity (turn right, away from left wall)
    EXPECT_GT(flock.l[0].velocity.x, initialVel.x);
}

TEST_F(FlockTest, BoundaryAvoidanceRight) {
    // Create a sprite near the right boundary heading towards it
    glm::vec3 pos(1850, 500, 0);  // Within margin (100) of xMax (1920)
    glm::vec3 initialVel(4, 0, 0);
    flock.l.push_back(SizedSprite(pos, initialVel, ofColor::white, 5.0f));

    float dt = 1.0f / 60.0f;
    flock.update(dt, 0.0f);

    // Boundary avoidance should decrease x velocity (turn left, away from right wall)
    EXPECT_LT(flock.l[0].velocity.x, initialVel.x);
}

// ============================================================================
// Test edge cases
// ============================================================================

TEST_F(FlockTest, EmptyFlockUpdate) {
    // Should not crash with empty flock
    EXPECT_NO_THROW(flock.update(1.0f / 60.0f, 0.0f));
}

TEST_F(FlockTest, SingleSpriteUpdate) {
    flock.l.push_back(SizedSprite(glm::vec3(500, 500, 0), glm::vec3(5, 0, 0), ofColor::white, 5.0f));

    // Single sprite should just move based on velocity, no flocking behavior
    EXPECT_NO_THROW(flock.update(1.0f / 60.0f, 0.0f));

    float speed = glm::length(flock.l[0].velocity);
    EXPECT_GE(speed, flock.minVelocity);
    EXPECT_LE(speed, flock.maxVelocity);
}

TEST_F(FlockTest, FastForwardParameter) {
    glm::vec3 initialPos(500, 500, 0);
    glm::vec3 velocity(10, 0, 0);

    // Test with fastForward = 1.0
    Flock flock1;
    flock1.fastForward = 1.0f;
    flock1.minVelocity = 3.0f;
    flock1.maxVelocity = 20.0f;
    flock1.xMin = 0;
    flock1.xMax = 1920;
    flock1.yMin = 0;
    flock1.yMax = 1080;
    flock1.zMin = -500;
    flock1.zMax = 500;
    flock1.l.push_back(SizedSprite(initialPos, velocity, ofColor::white, 5.0f));

    // Test with fastForward = 2.0
    Flock flock2;
    flock2.fastForward = 2.0f;
    flock2.minVelocity = 3.0f;
    flock2.maxVelocity = 20.0f;
    flock2.xMin = 0;
    flock2.xMax = 1920;
    flock2.yMin = 0;
    flock2.yMax = 1080;
    flock2.zMin = -500;
    flock2.zMax = 500;
    flock2.l.push_back(SizedSprite(initialPos, velocity, ofColor::white, 5.0f));

    float dt = 1.0f / 60.0f;
    flock1.update(dt, 0.0f);
    flock2.update(dt, 0.0f);

    // Flock with higher fastForward should move further
    float distance1 = glm::length(flock1.l[0].position - initialPos);
    float distance2 = glm::length(flock2.l[0].position - initialPos);
    EXPECT_GT(distance2, distance1);
}
