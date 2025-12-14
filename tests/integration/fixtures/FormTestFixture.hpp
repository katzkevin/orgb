#pragma once

#include "ColorProvider.hpp"
#include "DrawManager.hpp"
#include "Forms/VisualForm.hpp"
#include "GLTestFixture.hpp"
#include "KeyState.hpp"

/**
 * FormTestFixture provides common utilities for testing VisualForm implementations.
 *
 * This fixture extends GLTestFixture and adds helper methods and instances
 * of the core systems that forms depend on (ColorProvider, KeyState, DrawManager).
 *
 * Usage:
 *   class RadialParticlesTest : public FormTestFixture {
 *   protected:
 *       void SetUp() override {
 *           FormTestFixture::SetUp();
 *           form = std::make_shared<RadialParticles>("Test");
 *       }
 *
 *       std::shared_ptr<RadialParticles> form;
 *   };
 */
class FormTestFixture : public GLTestFixture {
   protected:
    ColorProvider clr;
    KeyState ks;
    DrawManager dm;

    void SetUp() override {
        GLTestFixture::SetUp();

        // Initialize with sensible defaults
        clr = ColorProvider();
        ks = KeyState();
        dm = DrawManager();

        // Clear any initial state
        ks.cleanup(0, 0, 0);
    }

    void TearDown() override {
        // Clean up any active notes
        ks.cleanup(0, 0, 0);

        GLTestFixture::TearDown();
    }

    /**
     * Helper: Trigger a note and run one update cycle
     */
    void triggerNote(int midiNote, float velocity = 0.8f) { ks.newKeyPressedHandler(midiNote, velocity, 1); }

    /**
     * Helper: Release a note
     */
    void releaseNote(int midiNote) { ks.keyReleasedHandler(midiNote); }

    /**
     * Helper: Simulate one frame of update/draw for a form
     */
    void updateAndDraw(VisualForm & form) {
        form.update(ks, clr);

        dm.beginDraw();
        form.draw(ks, clr, dm);
        dm.endDraw();
    }

    /**
     * Helper: Verify a form can setup without GL errors
     */
    void expectValidSetup(VisualForm & form) {
        ASSERT_NO_THROW(form.setup()) << "Form setup threw exception";
        EXPECT_EQ(glGetError(), GL_NO_ERROR) << "GL error after form setup";
    }

    /**
     * Helper: Verify a form can update/draw without GL errors
     */
    void expectValidUpdateDraw(VisualForm & form) {
        ASSERT_NO_THROW(updateAndDraw(form)) << "Form update/draw threw exception";
        EXPECT_EQ(glGetError(), GL_NO_ERROR) << "GL error after form update/draw";
    }

    /**
     * Helper: Run multiple frames
     */
    void runFrames(VisualForm & form, int frameCount) {
        for (int i = 0; i < frameCount; i++) {
            updateAndDraw(form);
        }
    }

    /**
     * Helper: Check if FBO was drawn to (has non-zero content)
     */
    bool fboHasContent(const ofFbo & fbo) {
        ofPixels pixels;
        fbo.readToPixels(pixels);

        // Check if any non-black pixels exist
        for (size_t i = 0; i < pixels.size(); i++) {
            if (pixels[i] > 0) {
                return true;
            }
        }
        return false;
    }

    /**
     * Helper: Get pixel count above threshold (for verifying rendering)
     */
    int countPixelsAboveThreshold(const ofFbo & fbo, int threshold = 10) {
        ofPixels pixels;
        fbo.readToPixels(pixels);

        int count = 0;
        for (size_t i = 0; i < pixels.size(); i++) {
            if (pixels[i] > threshold) {
                count++;
            }
        }
        return count;
    }
};
