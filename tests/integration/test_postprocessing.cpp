/**
 * Integration tests for ShaderPipeline post-processing
 *
 * These tests verify the shader pipeline can correctly chain effects,
 * manage FBOs, and apply post-processing without GL errors.
 *
 * Tests require a valid OpenGL context.
 */

#include <gtest/gtest.h>

#include "DrawManager.hpp"
#include "Effects/AllEffects.hpp"
#include "ShaderPipeline.hpp"
#include "fixtures/GLTestFixture.hpp"

class PostProcessingTest : public GLTestFixture {
   protected:
    std::shared_ptr<ShaderPipeline> pipeline;

    void SetUp() override {
        GLTestFixture::SetUp();
        pipeline = std::make_shared<ShaderPipeline>(1920, 1080);
    }
};

// ============================================================================
// Pipeline Creation and Configuration Tests
// ============================================================================

TEST_F(PostProcessingTest, Pipeline_CreatesSuccessfully) {
    EXPECT_NE(pipeline, nullptr);
    EXPECT_EQ(pipeline->getWidth(), 1920);
    EXPECT_EQ(pipeline->getHeight(), 1080);
    EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

TEST_F(PostProcessingTest, Pipeline_AddEffect) {
    auto effect = std::make_shared<FilmGrainEffect>();

    pipeline->addEffect(effect);

    EXPECT_EQ(pipeline->getEffectCount(), 1);
    EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

TEST_F(PostProcessingTest, Pipeline_AddMultipleEffects) {
    auto effect1 = std::make_shared<FilmGrainEffect>();
    auto effect2 = std::make_shared<ScanlinesEffect>();
    auto effect3 = std::make_shared<ChromaticAberrationEffect>();

    pipeline->addEffect(effect1);
    pipeline->addEffect(effect2);
    pipeline->addEffect(effect3);

    EXPECT_EQ(pipeline->getEffectCount(), 3);
    EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

TEST_F(PostProcessingTest, Pipeline_RemoveEffect) {
    auto effect1 = std::make_shared<FilmGrainEffect>();
    auto effect2 = std::make_shared<ScanlinesEffect>();

    pipeline->addEffect(effect1);
    pipeline->addEffect(effect2);

    EXPECT_EQ(pipeline->getEffectCount(), 2);

    pipeline->removeEffect("Film Grain");

    EXPECT_EQ(pipeline->getEffectCount(), 1);
    EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

TEST_F(PostProcessingTest, Pipeline_GetEffect) {
    auto effect = std::make_shared<FilmGrainEffect>();
    pipeline->addEffect(effect);

    auto retrieved = pipeline->getEffect("Film Grain");

    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->getName(), "Film Grain");
}

TEST_F(PostProcessingTest, Pipeline_GetNonexistentEffect) {
    auto retrieved = pipeline->getEffect("Nonexistent Effect");

    EXPECT_EQ(retrieved, nullptr);
}

// ============================================================================
// Effect Ordering Tests
// ============================================================================

TEST_F(PostProcessingTest, Pipeline_MoveEffect) {
    auto effect1 = std::make_shared<FilmGrainEffect>();
    auto effect2 = std::make_shared<ScanlinesEffect>();
    auto effect3 = std::make_shared<ChromaticAberrationEffect>();

    pipeline->addEffect(effect1);
    pipeline->addEffect(effect2);
    pipeline->addEffect(effect3);

    // Move effect at index 2 to index 0
    pipeline->moveEffect(2, 0);

    auto firstEffect = pipeline->getEffectAt(0);
    ASSERT_NE(firstEffect, nullptr);
    EXPECT_EQ(firstEffect->getName(), "Chromatic Aberration");
}

TEST_F(PostProcessingTest, Pipeline_GetEffectAt) {
    auto effect1 = std::make_shared<FilmGrainEffect>();
    auto effect2 = std::make_shared<ScanlinesEffect>();

    pipeline->addEffect(effect1);
    pipeline->addEffect(effect2);

    auto firstEffect = pipeline->getEffectAt(0);
    auto secondEffect = pipeline->getEffectAt(1);

    EXPECT_EQ(firstEffect->getName(), "Film Grain");
    EXPECT_EQ(secondEffect->getName(), "Scanlines");
}

// ============================================================================
// Enable/Disable Tests
// ============================================================================

TEST_F(PostProcessingTest, Pipeline_EnableDisableAll) {
    auto effect1 = std::make_shared<FilmGrainEffect>();
    auto effect2 = std::make_shared<ScanlinesEffect>();

    pipeline->addEffect(effect1);
    pipeline->addEffect(effect2);

    pipeline->disableAll();
    EXPECT_EQ(pipeline->getEnabledEffectCount(), 0);
    EXPECT_FALSE(pipeline->hasEnabledEffects());

    pipeline->enableAll();
    EXPECT_EQ(pipeline->getEnabledEffectCount(), 2);
    EXPECT_TRUE(pipeline->hasEnabledEffects());
}

TEST_F(PostProcessingTest, Pipeline_EnableDisableIndividualEffect) {
    auto effect = std::make_shared<FilmGrainEffect>();
    pipeline->addEffect(effect);

    EXPECT_TRUE(effect->isEnabled());

    effect->setEnabled(false);
    EXPECT_FALSE(effect->isEnabled());
    EXPECT_EQ(pipeline->getEnabledEffectCount(), 0);

    effect->setEnabled(true);
    EXPECT_TRUE(effect->isEnabled());
    EXPECT_EQ(pipeline->getEnabledEffectCount(), 1);
}

// ============================================================================
// Rendering Tests
// ============================================================================

TEST_F(PostProcessingTest, Pipeline_ProcessAndDraw) {
    auto effect = std::make_shared<FilmGrainEffect>();
    pipeline->addEffect(effect);

    // Create source FBO
    ofFbo sourceFbo;
    sourceFbo.allocate(1920, 1080, GL_RGBA);

    sourceFbo.begin();
    ofClear(255, 0, 0, 255);  // Red
    ofSetColor(255, 255, 255);
    ofDrawCircle(960, 540, 200);
    sourceFbo.end();

    // Process and draw
    ASSERT_NO_THROW({ pipeline->processAndDraw(sourceFbo, 0, 0); });

    EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

TEST_F(PostProcessingTest, Pipeline_ProcessWithMultipleEffects) {
    pipeline->addEffect(std::make_shared<FilmGrainEffect>());
    pipeline->addEffect(std::make_shared<ScanlinesEffect>());
    pipeline->addEffect(std::make_shared<ChromaticAberrationEffect>());

    ofFbo sourceFbo;
    sourceFbo.allocate(1920, 1080, GL_RGBA);

    sourceFbo.begin();
    ofClear(0, 255, 0, 255);  // Green
    sourceFbo.end();

    ASSERT_NO_THROW({ pipeline->processAndDraw(sourceFbo, 0, 0); });

    EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

TEST_F(PostProcessingTest, Pipeline_ProcessWithDisabledEffects) {
    auto effect1 = std::make_shared<FilmGrainEffect>();
    auto effect2 = std::make_shared<ScanlinesEffect>();

    pipeline->addEffect(effect1);
    pipeline->addEffect(effect2);

    // Disable first effect
    effect1->setEnabled(false);

    ofFbo sourceFbo;
    sourceFbo.allocate(1920, 1080, GL_RGBA);

    sourceFbo.begin();
    ofClear(0, 0, 255, 255);  // Blue
    sourceFbo.end();

    // Should still process with only enabled effect
    ASSERT_NO_THROW({ pipeline->processAndDraw(sourceFbo, 0, 0); });

    EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

TEST_F(PostProcessingTest, Pipeline_ProcessWithNoEffects) {
    // Pipeline with no effects should still work (passthrough)
    ofFbo sourceFbo;
    sourceFbo.allocate(1920, 1080, GL_RGBA);

    sourceFbo.begin();
    ofClear(255, 255, 0, 255);  // Yellow
    sourceFbo.end();

    ASSERT_NO_THROW({ pipeline->processAndDraw(sourceFbo, 0, 0); });

    EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

// ============================================================================
// Resize Tests
// ============================================================================

TEST_F(PostProcessingTest, Pipeline_Resize) {
    pipeline->addEffect(std::make_shared<FilmGrainEffect>());

    pipeline->resize(1280, 720);

    EXPECT_EQ(pipeline->getWidth(), 1280);
    EXPECT_EQ(pipeline->getHeight(), 720);
    EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

TEST_F(PostProcessingTest, Pipeline_ResizeAndProcess) {
    pipeline->addEffect(std::make_shared<ScanlinesEffect>());
    pipeline->resize(1280, 720);

    ofFbo sourceFbo;
    sourceFbo.allocate(1280, 720, GL_RGBA);

    sourceFbo.begin();
    ofClear(128, 128, 128, 255);  // Gray
    sourceFbo.end();

    ASSERT_NO_THROW({ pipeline->processAndDraw(sourceFbo, 0, 0); });

    EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

// ============================================================================
// Integration with DrawManager Tests
// ============================================================================

class PipelineDrawManagerTest : public GLTestFixture {
   protected:
    DrawManager dm;
    std::shared_ptr<ShaderPipeline> pipeline;

    void SetUp() override {
        GLTestFixture::SetUp();
        dm = DrawManager();
        pipeline = std::make_shared<ShaderPipeline>(1920, 1080);
    }
};

TEST_F(PipelineDrawManagerTest, DrawManager_WithPipeline) {
    pipeline->addEffect(std::make_shared<FilmGrainEffect>());

    // Draw to DrawManager FBO
    dm.beginDraw();
    ofSetColor(255, 255, 255);
    ofDrawCircle(960, 540, 300);
    dm.endDraw();

    // Apply post-processing
    ASSERT_NO_THROW({ pipeline->processAndDraw(dm.getFboFront(), 0, 0); });

    EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

TEST_F(PipelineDrawManagerTest, FullRenderPipeline) {
    pipeline->addEffect(std::make_shared<FilmGrainEffect>());
    pipeline->addEffect(std::make_shared<ScanlinesEffect>());

    // Simulate full render cycle
    dm.beginDraw();
    ofBackground(0);
    ofSetColor(255, 255, 255);
    ofDrawCircle(960, 540, 200);
    ofDrawRectangle(700, 380, 520, 320);
    dm.endDraw();

    // Apply post-processing and draw
    ASSERT_NO_THROW({
        if (pipeline->hasEnabledEffects()) {
            pipeline->processAndDraw(dm.getFboFront(), 0, 0);
        } else {
            dm.getFboFront().draw(0, 0);
        }
    });

    EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

TEST_F(PipelineDrawManagerTest, MultipleFrames) {
    pipeline->addEffect(std::make_shared<ScanlinesEffect>());

    for (int frame = 0; frame < 10; frame++) {
        dm.beginDraw();
        ofBackground(0);
        ofSetColor(255, 255, 255);
        ofDrawCircle(960, 540, 100 + frame * 10);
        dm.endDraw();

        ASSERT_NO_THROW({ pipeline->processAndDraw(dm.getFboFront(), 0, 0); });

        EXPECT_EQ(glGetError(), GL_NO_ERROR);
    }
}
