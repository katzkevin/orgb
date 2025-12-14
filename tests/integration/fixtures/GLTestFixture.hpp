#pragma once

#include <gtest/gtest.h>

#include "ofAppGLFWWindow.h"
#include "ofMain.h"

/**
 * GLTestFixture provides an OpenGL context for integration tests.
 *
 * This fixture creates a hidden GLFW window with a valid GL context,
 * allowing tests to verify shader compilation, FBO operations, and
 * other GL-dependent functionality.
 *
 * Usage:
 *   class MyIntegrationTest : public GLTestFixture {
 *   protected:
 *       void SetUp() override {
 *           GLTestFixture::SetUp();
 *           // Your setup
 *       }
 *   };
 *
 * Note: Tests using this fixture will be slower than pure unit tests
 * and require a display/headless GL context (e.g., Xvfb in CI).
 */
class GLTestFixture : public ::testing::Test {
   protected:
    static std::shared_ptr<ofAppGLFWWindow> window;
    static bool isInitialized;

    static void SetUpTestSuite() {
        if (!isInitialized) {
            // Configure headless window
            ofGLFWWindowSettings settings;
            settings.setSize(1920, 1080);
            settings.windowMode = OF_WINDOW;

// On macOS, we can't truly hide the window, but we can minimize it
// On Linux with Xvfb, this creates a proper headless context
#ifdef TARGET_LINUX
            settings.visible = false;
#endif

            // Create window
            window = std::make_shared<ofAppGLFWWindow>();
            window->setup(settings);

            // Initialize OpenGL context
            ofSetupOpenGL(window, 1920, 1080, OF_WINDOW);

            // Set log level to reduce noise during tests
            ofSetLogLevel(OF_LOG_WARNING);

            isInitialized = true;
        }
    }

    static void TearDownTestSuite() {
        // Note: We don't destroy the GL context between tests
        // to avoid expensive context recreation. It's cleaned up
        // when the test process exits.
    }

    void SetUp() override {
        if (!isInitialized) {
            FAIL() << "GL context not initialized. Call SetUpTestSuite first.";
        }

        // Clear any GL errors from previous tests
        while (glGetError() != GL_NO_ERROR);

        // Reset GL state
        ofClear(0, 0, 0, 255);
        ofSetColor(255, 255, 255);

        // Ensure we're in a clean state
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void TearDown() override {
        // Verify no GL errors were left by the test
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            ADD_FAILURE() << "GL error detected after test: " << err;
        }

        // Clean up any bound resources
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);
    }

    /**
     * Helper to check if GL context is valid
     */
    bool hasValidGLContext() const { return glGetString(GL_VERSION) != nullptr; }

    /**
     * Helper to get GL version string
     */
    std::string getGLVersion() const {
        const GLubyte * version = glGetString(GL_VERSION);
        return version ? std::string(reinterpret_cast<const char *>(version)) : "UNKNOWN";
    }
};

// Static member initialization moved to GLTestFixture.cpp to avoid duplicate symbols
