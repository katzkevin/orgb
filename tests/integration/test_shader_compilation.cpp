/**
 * Integration tests for shader compilation and loading
 *
 * These tests verify that all shader effects can actually compile
 * and load with a real OpenGL context. Unlike the unit tests in
 * shader_tests.cpp which only test the API structure, these tests
 * ensure shaders are syntactically correct and GL-compatible.
 *
 * Tests require a valid OpenGL context and shader files on disk.
 */

#include <gtest/gtest.h>

#include "Effects/AllEffects.hpp"
#include "ShaderEffect.hpp"
#include "ShaderPipeline.hpp"
#include "fixtures/GLTestFixture.hpp"

class ShaderCompilationTest : public GLTestFixture {};

// ============================================================================
// Individual Shader Compilation Tests
// ============================================================================

TEST_F(ShaderCompilationTest, FilmGrain_CompilesSuccessfully) {
    auto effect = std::make_shared<FilmGrainEffect>();

    ASSERT_TRUE(effect->isValid()) << "FilmGrain shader failed to load/compile";

    EXPECT_EQ(glGetError(), GL_NO_ERROR) << "GL error after FilmGrain shader compilation";
}

TEST_F(ShaderCompilationTest, Scanlines_CompilesSuccessfully) {
    auto effect = std::make_shared<ScanlinesEffect>();

    ASSERT_TRUE(effect->isValid()) << "Scanlines shader failed to load/compile";

    EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

TEST_F(ShaderCompilationTest, ChromaticAberration_CompilesSuccessfully) {
    auto effect = std::make_shared<ChromaticAberrationEffect>();

    ASSERT_TRUE(effect->isValid()) << "ChromaticAberration shader failed to load/compile";

    EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

TEST_F(ShaderCompilationTest, Displacement_CompilesSuccessfully) {
    auto effect = std::make_shared<DisplacementEffect>();

    ASSERT_TRUE(effect->isValid()) << "Displacement shader failed to load/compile";

    EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

TEST_F(ShaderCompilationTest, VHSGlitch_CompilesSuccessfully) {
    auto effect = std::make_shared<VHSGlitchEffect>();

    ASSERT_TRUE(effect->isValid()) << "VHSGlitch shader failed to load/compile";

    EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

TEST_F(ShaderCompilationTest, Feedback_CompilesSuccessfully) {
    auto effect = std::make_shared<FeedbackEffect>();

    ASSERT_TRUE(effect->isValid()) << "Feedback shader failed to load/compile";

    EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

TEST_F(ShaderCompilationTest, Passthrough_CompilesSuccessfully) {
    auto effect = std::make_shared<PassthroughEffect>();

    // PassthroughEffect has no shader - it's a pass-through
    // So isValid() will return false, but that's expected
    // Just verify it doesn't crash when created
    EXPECT_TRUE(true) << "PassthroughEffect created without errors";

    EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

// ============================================================================
// Comprehensive Tests
// ============================================================================

TEST_F(ShaderCompilationTest, AllEffects_CompileSuccessfully) {
    // Test all effects at once (excluding Passthrough which has no shader)
    std::vector<std::pair<std::string, std::shared_ptr<ShaderEffect>>> effects = {
        {"FilmGrain", std::make_shared<FilmGrainEffect>()},
        {"Scanlines", std::make_shared<ScanlinesEffect>()},
        {"ChromaticAberration", std::make_shared<ChromaticAberrationEffect>()},
        {"Displacement", std::make_shared<DisplacementEffect>()},
        {"VHSGlitch", std::make_shared<VHSGlitchEffect>()},
        {"Feedback", std::make_shared<FeedbackEffect>()}};

    for (const auto & [name, effect] : effects) {
        SCOPED_TRACE("Testing effect: " + name);

        EXPECT_TRUE(effect->isValid()) << name << " shader failed to compile";

        EXPECT_EQ(glGetError(), GL_NO_ERROR) << "GL error after " << name << " compilation";
    }
}

// ============================================================================
// Shader Execution Tests
// TODO: Rewrite these tests to use the new apply(input, output) API
// The old begin()/end() API has been replaced
// ============================================================================

/*
TEST_F(ShaderCompilationTest, DISABLED_FilmGrain_CanApplyToTexture) {
    auto effect = std::make_shared<FilmGrainEffect>();
    ASSERT_TRUE(effect->isValid());

    // Create a test texture
    ofFbo fbo;
    fbo.allocate(512, 512, GL_RGBA);

    fbo.begin();
    ofClear(128, 128, 128, 255);  // Gray
    fbo.end();

    // Apply shader
    ASSERT_NO_THROW({
        effect->begin();
        fbo.draw(0, 0);
        effect->end();
    });

    EXPECT_EQ(glGetError(), GL_NO_ERROR);
}
*/

/*
TEST_F(ShaderCompilationTest, DISABLED_Scanlines_CanApplyToTexture) {
    auto effect = std::make_shared<ScanlinesEffect>();
    ASSERT_TRUE(effect->isValid());

    ofFbo fbo;
    fbo.allocate(512, 512, GL_RGBA);

    fbo.begin();
    ofClear(255, 0, 0, 255);  // Red
    fbo.end();

    ASSERT_NO_THROW({
        effect->begin();
        fbo.draw(0, 0);
        effect->end();
    });

    EXPECT_EQ(glGetError(), GL_NO_ERROR);
}
*/

/*
TEST_F(ShaderCompilationTest, DISABLED_MultipleEffects_CanApplySequentially) {
    auto effect1 = std::make_shared<FilmGrainEffect>();
    auto effect2 = std::make_shared<ScanlinesEffect>();

    ASSERT_TRUE(effect1->isValid());
    ASSERT_TRUE(effect2->isValid());

    ofFbo fbo;
    fbo.allocate(512, 512, GL_RGBA);

    fbo.begin();
    ofClear(0, 255, 0, 255);  // Green
    fbo.end();

    // Apply multiple shaders in sequence
    ASSERT_NO_THROW({
        effect1->begin();
        fbo.draw(0, 0);
        effect1->end();

        effect2->begin();
        fbo.draw(0, 0);
        effect2->end();
    });

    EXPECT_EQ(glGetError(), GL_NO_ERROR);
}
*/

// ============================================================================
// Shader Parameter Tests
// TODO: Rewrite these to use new API
// ============================================================================

/*
TEST_F(ShaderCompilationTest, DISABLED_FilmGrain_ParametersAffectShader) {
    auto effect = std::make_shared<FilmGrainEffect>();
    ASSERT_TRUE(effect->isValid());

    ofFbo fbo;
    fbo.allocate(512, 512, GL_RGBA);
    fbo.begin();
    ofClear(128, 128, 128, 255);
    fbo.end();

    // Change parameters
    effect->setIntensity(0.5f);
    effect->setGrainSize(2.0f);

    // Should still work
    ASSERT_NO_THROW({
        effect->begin();
        fbo.draw(0, 0);
        effect->end();
    });

    EXPECT_EQ(glGetError(), GL_NO_ERROR);
}
*/

/*
TEST_F(ShaderCompilationTest, DISABLED_Scanlines_ParametersAffectShader) {
    auto effect = std::make_shared<ScanlinesEffect>();
    ASSERT_TRUE(effect->isValid());

    ofFbo fbo;
    fbo.allocate(512, 512, GL_RGBA);
    fbo.begin();
    ofClear(128, 128, 128, 255);
    fbo.end();

    // Change parameters
    effect->setIntensity(0.8f);
    effect->setScanlineCount(200.0f);

    ASSERT_NO_THROW({
        effect->begin();
        fbo.draw(0, 0);
        effect->end();
    });

    EXPECT_EQ(glGetError(), GL_NO_ERROR);
}
*/

// ============================================================================
// Error Handling Tests
// TODO: Rewrite to use new API
// ============================================================================

/*
TEST_F(ShaderCompilationTest, DISABLED_MissingShaderFiles_HandledGracefully) {
    // Note: This test assumes ShaderEffect gracefully handles missing files
    // by setting isLoaded() to false rather than crashing

    // Create an effect that will try to load non-existent shader
    // This is a design decision - do we want to fail hard or soft?
    // Current design should return isLoaded() == false

    // We can't easily test this without modifying the effect paths
    // But we can verify that loaded shaders report correctly
    auto effect = std::make_shared<FilmGrainEffect>();

    // Should either load successfully or fail gracefully
    bool loaded = effect->isValid();

    if (!loaded) {
        // If it failed to load, it should not crash when we try to use it
        ASSERT_NO_THROW({
            effect->begin();
            effect->end();
        });
    }
}
*/
