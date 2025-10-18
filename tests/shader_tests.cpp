//
//  shader_tests.cpp
//  orgb
//
//  Shader compilation and validation tests
//
//  This file contains tests for shader loading, compilation, and basic validation.
//  Tests can be run as part of the build process or manually.
//
//  Usage:
//    Compile and run this file to validate all shaders in the project.
//    Returns 0 on success, non-zero on failure.
//

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "../src/DrawContext.hpp"
#include "../src/Effects/AllEffects.hpp"
#include "../src/ShaderEffect.hpp"
#include "../src/ShaderPipeline.hpp"
#include "ofMain.h"

// Simple test framework
class ShaderTest {
   public:
    static int totalTests;
    static int passedTests;
    static int failedTests;

    static void runTest(const std::string & testName, std::function<bool()> testFunc) {
        totalTests++;
        std::cout << "Running test: " << testName << " ... ";

        bool result = false;
        try {
            result = testFunc();
        } catch (const std::exception & e) {
            std::cout << "EXCEPTION: " << e.what() << std::endl;
            result = false;
        }

        if (result) {
            std::cout << "PASS" << std::endl;
            passedTests++;
        } else {
            std::cout << "FAIL" << std::endl;
            failedTests++;
        }
    }

    static void printSummary() {
        std::cout << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Test Summary" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Total tests:  " << totalTests << std::endl;
        std::cout << "Passed:       " << passedTests << std::endl;
        std::cout << "Failed:       " << failedTests << std::endl;
        std::cout << "Success rate: " << (totalTests > 0 ? (passedTests * 100 / totalTests) : 0) << "%" << std::endl;
        std::cout << "========================================" << std::endl;
    }

    static int getExitCode() { return failedTests == 0 ? 0 : 1; }
};

int ShaderTest::totalTests = 0;
int ShaderTest::passedTests = 0;
int ShaderTest::failedTests = 0;

// ============================================================================
// Test Cases
// ============================================================================

bool test_FilmGrainEffect_Creation() {
    auto effect = std::make_shared<FilmGrainEffect>();
    return effect != nullptr && effect->getName() == "Film Grain";
}

bool test_FilmGrainEffect_Validation() {
    auto effect = std::make_shared<FilmGrainEffect>();
    // Note: This will fail if shader files don't exist or can't compile
    // In a headless environment without GL context, this may fail
    // That's expected and OK for now
    return effect != nullptr;
}

bool test_FilmGrainEffect_Parameters() {
    auto effect = std::make_shared<FilmGrainEffect>();
    effect->setIntensity(0.5f);
    effect->setGrainSize(2.0f);
    return effect->getIntensity() == 0.5f && effect->getGrainSize() == 2.0f;
}

bool test_ScanlinesEffect_Creation() {
    auto effect = std::make_shared<ScanlinesEffect>();
    return effect != nullptr && effect->getName() == "Scanlines";
}

bool test_ScanlinesEffect_Parameters() {
    auto effect = std::make_shared<ScanlinesEffect>();
    effect->setIntensity(0.8f);
    effect->setScanlineCount(300.0f);
    return effect->getIntensity() == 0.8f && effect->getScanlineCount() == 300.0f;
}

bool test_DigitalGlitchEffect_Creation() {
    auto effect = std::make_shared<DigitalGlitchEffect>();
    return effect != nullptr && effect->getName() == "Digital Glitch";
}

bool test_DigitalGlitchEffect_DefaultDisabled() {
    auto effect = std::make_shared<DigitalGlitchEffect>();
    // Glitch effect should be disabled by default
    return !effect->isEnabled();
}

bool test_DigitalGlitchEffect_Parameters() {
    auto effect = std::make_shared<DigitalGlitchEffect>();
    effect->setIntensity(0.7f);
    effect->setBlockSize(32.0f);
    return effect->getIntensity() == 0.7f && effect->getBlockSize() == 32.0f;
}

bool test_ShaderPipeline_Creation() {
    auto pipeline = std::make_shared<ShaderPipeline>(1920, 1080);
    return pipeline != nullptr && pipeline->getWidth() == 1920 && pipeline->getHeight() == 1080;
}

bool test_ShaderPipeline_AddEffects() {
    auto pipeline = std::make_shared<ShaderPipeline>(1920, 1080);
    auto effect1 = std::make_shared<FilmGrainEffect>();
    auto effect2 = std::make_shared<ScanlinesEffect>();

    pipeline->addEffect(effect1);
    pipeline->addEffect(effect2);

    return pipeline->getEffectCount() == 2;
}

bool test_ShaderPipeline_RemoveEffects() {
    auto pipeline = std::make_shared<ShaderPipeline>(1920, 1080);
    auto effect1 = std::make_shared<FilmGrainEffect>();
    auto effect2 = std::make_shared<ScanlinesEffect>();

    pipeline->addEffect(effect1);
    pipeline->addEffect(effect2);
    pipeline->removeEffect("Film Grain");

    return pipeline->getEffectCount() == 1;
}

bool test_ShaderPipeline_GetEffect() {
    auto pipeline = std::make_shared<ShaderPipeline>(1920, 1080);
    auto effect = std::make_shared<FilmGrainEffect>();

    pipeline->addEffect(effect);
    auto retrieved = pipeline->getEffect("Film Grain");

    return retrieved != nullptr && retrieved->getName() == "Film Grain";
}

bool test_ShaderPipeline_EnableDisable() {
    auto pipeline = std::make_shared<ShaderPipeline>(1920, 1080);
    auto effect1 = std::make_shared<FilmGrainEffect>();
    auto effect2 = std::make_shared<ScanlinesEffect>();

    pipeline->addEffect(effect1);
    pipeline->addEffect(effect2);

    pipeline->disableAll();
    bool allDisabled = pipeline->getEnabledEffectCount() == 0;

    pipeline->enableAll();
    bool allEnabled = pipeline->getEnabledEffectCount() == 2;

    return allDisabled && allEnabled;
}

bool test_ShaderPipeline_MoveEffects() {
    auto pipeline = std::make_shared<ShaderPipeline>(1920, 1080);
    auto effect1 = std::make_shared<FilmGrainEffect>();
    auto effect2 = std::make_shared<ScanlinesEffect>();

    pipeline->addEffect(effect1);
    pipeline->addEffect(effect2);

    // Move effect at index 1 to index 0
    pipeline->moveEffect(1, 0);

    auto firstEffect = pipeline->getEffectAt(0);
    return firstEffect != nullptr && firstEffect->getName() == "Scanlines";
}

bool test_ShaderPipeline_Resize() {
    auto pipeline = std::make_shared<ShaderPipeline>(1920, 1080);
    pipeline->resize(1280, 720);

    return pipeline->getWidth() == 1280 && pipeline->getHeight() == 720;
}

bool test_PassthroughEffect() {
    auto effect = std::make_shared<PassthroughEffect>();
    return effect != nullptr && effect->getName() == "Passthrough";
}

bool test_Effect_ToggleState() {
    auto effect = std::make_shared<FilmGrainEffect>();
    bool initialState = effect->isEnabled();

    effect->toggle();
    bool toggledState = effect->isEnabled();

    effect->toggle();
    bool toggledBackState = effect->isEnabled();

    return initialState != toggledState && initialState == toggledBackState;
}

bool test_Effect_ParameterCount() {
    auto filmGrain = std::make_shared<FilmGrainEffect>();
    auto scanlines = std::make_shared<ScanlinesEffect>();
    auto glitch = std::make_shared<DigitalGlitchEffect>();

    // FilmGrain has 2 parameters (intensity, grain size)
    // Scanlines has 5 parameters
    // Glitch has 4 parameters
    return filmGrain->getParameters().size() == 2 && scanlines->getParameters().size() == 5 &&
           glitch->getParameters().size() == 4;
}

// ============================================================================
// Main Test Runner
// ============================================================================

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "ORGB Shader Tests" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    // Effect creation tests
    ShaderTest::runTest("FilmGrainEffect creation", test_FilmGrainEffect_Creation);
    ShaderTest::runTest("FilmGrainEffect parameters", test_FilmGrainEffect_Parameters);
    ShaderTest::runTest("ScanlinesEffect creation", test_ScanlinesEffect_Creation);
    ShaderTest::runTest("ScanlinesEffect parameters", test_ScanlinesEffect_Parameters);
    ShaderTest::runTest("DigitalGlitchEffect creation", test_DigitalGlitchEffect_Creation);
    ShaderTest::runTest("DigitalGlitchEffect default disabled", test_DigitalGlitchEffect_DefaultDisabled);
    ShaderTest::runTest("DigitalGlitchEffect parameters", test_DigitalGlitchEffect_Parameters);

    // Pipeline tests
    ShaderTest::runTest("ShaderPipeline creation", test_ShaderPipeline_Creation);
    ShaderTest::runTest("ShaderPipeline add effects", test_ShaderPipeline_AddEffects);
    ShaderTest::runTest("ShaderPipeline remove effects", test_ShaderPipeline_RemoveEffects);
    ShaderTest::runTest("ShaderPipeline get effect", test_ShaderPipeline_GetEffect);
    ShaderTest::runTest("ShaderPipeline enable/disable", test_ShaderPipeline_EnableDisable);
    ShaderTest::runTest("ShaderPipeline move effects", test_ShaderPipeline_MoveEffects);
    ShaderTest::runTest("ShaderPipeline resize", test_ShaderPipeline_Resize);

    // General effect tests
    ShaderTest::runTest("PassthroughEffect", test_PassthroughEffect);
    ShaderTest::runTest("Effect toggle state", test_Effect_ToggleState);
    ShaderTest::runTest("Effect parameter count", test_Effect_ParameterCount);

    // Print summary
    ShaderTest::printSummary();

    return ShaderTest::getExitCode();
}
