/**
 * Integration tests for VisualForm rendering
 *
 * These tests verify that forms can properly initialize GL resources,
 * run update/draw cycles without crashes, and produce visual output.
 *
 * Tests require a valid OpenGL context (provided by GLTestFixture).
 */

#include <gtest/gtest.h>

#include "fixtures/FormTestFixture.hpp"

// Import all form types
#include "Forms/Particles/EdgeParticles.hpp"
#include "Forms/Particles/RadialParticles.hpp"
#include "Forms/Shapes/MeshGrid.hpp"
#include "Forms/Shapes/NoiseGrid.hpp"
#include "Forms/Waves/EdgeLasers.hpp"

// ============================================================================
// Individual Form Tests
// ============================================================================

class RadialParticlesTest : public FormTestFixture {
   protected:
    std::shared_ptr<RadialParticles> form;

    void SetUp() override {
        FormTestFixture::SetUp();
        form = std::make_shared<RadialParticles>("RadialParticles");
    }
};

TEST_F(RadialParticlesTest, SetupWithoutGLErrors) { expectValidSetup(*form); }

TEST_F(RadialParticlesTest, UpdateDrawWithoutCrash) {
    form->setup();
    expectValidUpdateDraw(*form);
}

TEST_F(RadialParticlesTest, UpdateDrawWithActiveNote) {
    form->setup();
    triggerNote(60, 0.8f);
    expectValidUpdateDraw(*form);
}

TEST_F(RadialParticlesTest, MultipleFrames) {
    form->setup();
    triggerNote(60, 0.8f);

    ASSERT_NO_THROW({ runFrames(*form, 10); });

    EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

// ============================================================================

class NoiseGridTest : public FormTestFixture {
   protected:
    std::shared_ptr<NoiseGrid> form;

    void SetUp() override {
        FormTestFixture::SetUp();
        form = std::make_shared<NoiseGrid>("NoiseGrid");
    }
};

TEST_F(NoiseGridTest, SetupWithoutGLErrors) { expectValidSetup(*form); }

TEST_F(NoiseGridTest, UpdateDrawWithoutCrash) {
    form->setup();
    expectValidUpdateDraw(*form);
}

TEST_F(NoiseGridTest, RendersWithActiveNotes) {
    form->setup();

    // Trigger multiple notes
    triggerNote(60, 0.8f);
    triggerNote(64, 0.7f);
    triggerNote(67, 0.9f);

    expectValidUpdateDraw(*form);
}

// ============================================================================

class EdgeParticlesTest : public FormTestFixture {
   protected:
    std::shared_ptr<EdgeParticles> form;

    void SetUp() override {
        FormTestFixture::SetUp();
        form = std::make_shared<EdgeParticles>("EdgeParticles");
    }
};

TEST_F(EdgeParticlesTest, SetupWithoutGLErrors) { expectValidSetup(*form); }

TEST_F(EdgeParticlesTest, UpdateDrawWithoutCrash) {
    form->setup();
    expectValidUpdateDraw(*form);
}

// ============================================================================

class MeshGridTest : public FormTestFixture {
   protected:
    std::shared_ptr<MeshGrid> form;

    void SetUp() override {
        FormTestFixture::SetUp();
        form = std::make_shared<MeshGrid>("MeshGrid");
    }
};

TEST_F(MeshGridTest, SetupWithoutGLErrors) { expectValidSetup(*form); }

TEST_F(MeshGridTest, UpdateDrawWithoutCrash) {
    form->setup();
    expectValidUpdateDraw(*form);
}

// ============================================================================

class EdgeLasersTest : public FormTestFixture {
   protected:
    std::shared_ptr<EdgeLasers> form;

    void SetUp() override {
        FormTestFixture::SetUp();
        form = std::make_shared<EdgeLasers>("EdgeLasers");
    }
};

TEST_F(EdgeLasersTest, SetupWithoutGLErrors) { expectValidSetup(*form); }

TEST_F(EdgeLasersTest, UpdateDrawWithoutCrash) {
    form->setup();
    expectValidUpdateDraw(*form);
}

// ============================================================================
// Comprehensive Tests
// ============================================================================

class AllFormsTest : public FormTestFixture {};

TEST_F(AllFormsTest, AllFormsCanInitialize) {
    std::vector<std::shared_ptr<VisualForm>> forms;

    forms.push_back(std::make_shared<RadialParticles>("RadialParticles"));
    forms.push_back(std::make_shared<NoiseGrid>("NoiseGrid"));
    forms.push_back(std::make_shared<EdgeParticles>("EdgeParticles"));
    forms.push_back(std::make_shared<MeshGrid>("MeshGrid"));
    forms.push_back(std::make_shared<EdgeLasers>("EdgeLasers"));

    for (auto & form : forms) {
        SCOPED_TRACE("Testing form: " + form->name);
        expectValidSetup(*form);
    }
}

TEST_F(AllFormsTest, AllFormsCanRender) {
    std::vector<std::shared_ptr<VisualForm>> forms;

    forms.push_back(std::make_shared<RadialParticles>("RadialParticles"));
    forms.push_back(std::make_shared<NoiseGrid>("NoiseGrid"));
    forms.push_back(std::make_shared<EdgeParticles>("EdgeParticles"));
    forms.push_back(std::make_shared<MeshGrid>("MeshGrid"));
    forms.push_back(std::make_shared<EdgeLasers>("EdgeLasers"));

    for (auto & form : forms) {
        SCOPED_TRACE("Testing form: " + form->name);
        form->setup();
        expectValidUpdateDraw(*form);
    }
}

TEST_F(AllFormsTest, FormSwitching) {
    std::vector<std::shared_ptr<VisualForm>> forms;

    forms.push_back(std::make_shared<RadialParticles>("RadialParticles"));
    forms.push_back(std::make_shared<NoiseGrid>("NoiseGrid"));
    forms.push_back(std::make_shared<EdgeParticles>("EdgeParticles"));

    // Setup all forms
    for (auto & form : forms) {
        form->setup();
    }

    // Simulate switching between forms
    triggerNote(60, 0.8f);

    for (size_t i = 0; i < forms.size(); i++) {
        SCOPED_TRACE("Switching to form: " + forms[i]->name);
        expectValidUpdateDraw(*forms[i]);
    }

    // Switch back to first form
    expectValidUpdateDraw(*forms[0]);
}

TEST_F(AllFormsTest, FormSurvivesNoteFlood) {
    auto form = std::make_shared<RadialParticles>("RadialParticles");
    form->setup();

    // Trigger many notes rapidly
    for (int note = 60; note < 72; note++) {
        triggerNote(note, 0.8f);
    }

    // Should still render without errors
    for (int i = 0; i < 5; i++) {
        expectValidUpdateDraw(*form);
    }

    // Release all notes
    for (int note = 60; note < 72; note++) {
        releaseNote(note);
    }
}

// ============================================================================
// DrawManager Integration Tests
// ============================================================================

class DrawManagerTest : public GLTestFixture {
   protected:
    DrawManager dm;
};

TEST_F(DrawManagerTest, BeginEndDrawCycle) {
    ASSERT_NO_THROW({
        dm.beginDraw();
        ofClear(255, 0, 0, 255);  // Red
        dm.endDraw();
    });

    EXPECT_EQ(glGetError(), GL_NO_ERROR);
}

TEST_F(DrawManagerTest, FBOContainsDrawnContent) {
    dm.beginDraw();
    ofSetColor(255, 255, 255);
    ofDrawCircle(ofGetWidth() / 2, ofGetHeight() / 2, 100);
    dm.endDraw();

    // Verify FBO has content
    ofPixels pixels;
    dm.getFboFront().readToPixels(pixels);

    bool hasNonBlackPixels = false;
    for (size_t i = 0; i < pixels.size(); i++) {
        if (pixels[i] > 0) {
            hasNonBlackPixels = true;
            break;
        }
    }

    EXPECT_TRUE(hasNonBlackPixels) << "FBO should contain drawn content";
}

TEST_F(DrawManagerTest, MultipleDrawCycles) {
    for (int i = 0; i < 10; i++) {
        ASSERT_NO_THROW({
            dm.beginDraw();
            ofClear(i * 25, 0, 0, 255);
            dm.endDraw();
        });
    }

    EXPECT_EQ(glGetError(), GL_NO_ERROR);
}
