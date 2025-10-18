//
//  DrawManager.cpp
//  orgb
//
//  Created by Kevin Katz on 4/4/22.
//

#include "DrawManager.hpp"

#include "DrawContext.hpp"

// TODO This is defined elsewhere, search for it
#define MULTISAMPLE_COUNT 0

DrawManager::DrawManager()
    : fboFront(getConfiguredFrameBuffer(ofGetWidth(), ofGetHeight())),
      fboBack(getConfiguredFrameBuffer(ofGetWidth(), ofGetHeight())),
      activeCanvas(boost::none),

      shaderPackageBlurX("shaderBlurX"),
      shaderPackageBlurY("shaderBlurY"),
      shaderPackageGlowLine("shaderGlowLine"),
      shaderPackageGlowCircle("shaderGlowCircle"),
      shaderPackageGlowRectangularPrism("shaderGlowRectangularPrism") {
    //    shaderPackageVHS("shaderVhs") {
    //    initializeFrameBuffers(ofGetWidth(), ofGetHeight());
}

void DrawManager::beginDraw() {
    // Extremely stateful, see existing examples
    if (activeCanvas.is_initialized()) {
        throw std::runtime_error("Can't begin draw with an already active canvas");
    }
    fboFront.begin();
    activeCanvas = boost::optional<ofFbo>(fboFront);
}

void DrawManager::endDraw() {
    // End drawing without drawing to screen (for post-processing pipeline)
    if (activeCanvas.get().getId() != fboFront.getId()) {
        throw std::runtime_error("Canvas must be front canvas.");
    }
    if (!activeCanvas.is_initialized()) {
        throw std::runtime_error("Can't end draw without an active canvas");
    }
    activeCanvas.get().end();
    activeCanvas = boost::none;
}

void DrawManager::endAndDrawFbo() {
    // Extremely stateful, see existing examples
    if (activeCanvas.get().getId() != fboFront.getId()) {
        throw std::runtime_error("Canvas must be front canvas.");
    }
    if (!activeCanvas.is_initialized()) {
        throw std::runtime_error("Can't end draw without an active canvas");
    }
    activeCanvas.get().end();
    drawFboAtZeroZero(activeCanvas.get());
    activeCanvas = boost::none;
}

void DrawManager::shaderEpilogue(ShaderPackage& sp) {
    // Extremely stateful, see existing examples
    if (activeCanvas.get().getId() != fboFront.getId()) {
        throw std::runtime_error("Canvas must be front canvas.");
    }
    if (!activeCanvas.is_initialized()) {
        throw std::runtime_error("Can't end shader without an active canvas");
    }

    // Now the fboBack is rendered, we want to draw it to the foreground.

    // Assuming a shader is active and its uniforms are set at this point here
    fboFront.end();
    fboBack.begin();
    drawFboAtZeroZero(fboFront);  // Draw back to
    sp.shader.end();

    std::swap(fboFront, fboBack);
    activeCanvas = boost::optional<ofFbo>(fboFront);
}

void DrawManager::shadeBlurX(float delta, float gain) {
    shaderPackageBlurX.shader.begin();
    shaderPackageBlurX.shader.setUniform1f("blurOffsetStepPixels", delta);
    shaderPackageBlurX.shader.setUniform1f("gain", gain);
    shaderPackageBlurX.shader.setUniform2f("screenDimensions", ofGetWidth(), ofGetHeight());
    shaderEpilogue(shaderPackageBlurX);
}

void DrawManager::shadeBlurY(float delta, float gain) {
    shaderPackageBlurY.shader.begin();
    shaderPackageBlurY.shader.setUniform1f("blurOffsetStepPixels", delta);
    shaderPackageBlurY.shader.setUniform1f("gain", gain);
    shaderPackageBlurY.shader.setUniform2f("screenDimensions", ofGetWidth(), ofGetHeight());
    shaderEpilogue(shaderPackageBlurY);
}

void DrawManager::shadeGlowLine(ofVec3f from, ofVec3f to, ofColor c, float glowIntensity, float glowDampenRadius,
                                int blendMode, bool toneMap) {
    // NOTE: Because this changes FBO, this resets the OF_MATRIX_MODELVIEW matrix.
    ofVec3f fromGlobal = applyGlobalTransformation(from);
    ofVec3f toGlobal = applyGlobalTransformation(to);
    shaderPackageGlowLine.shader.begin();
    shaderPackageGlowLine.shader.setUniform3f("from", fromGlobal);
    shaderPackageGlowLine.shader.setUniform3f("to", toGlobal);
    shaderPackageGlowLine.shader.setUniform1f("r", c.r / 255.0);
    shaderPackageGlowLine.shader.setUniform1f("g", c.g / 255.0);
    shaderPackageGlowLine.shader.setUniform1f("b", c.b / 255.0);
    shaderPackageGlowLine.shader.setUniform1f("a", c.a / 255.0);
    shaderPackageGlowLine.shader.setUniform1f("glowIntensity", glowIntensity);
    shaderPackageGlowLine.shader.setUniform1f("glowDampenRadius", glowDampenRadius);
    shaderPackageGlowLine.shader.setUniform1i("blendMode", blendMode);
    shaderPackageGlowLine.shader.setUniform1i("toneMap", toneMap);
    shaderPackageGlowLine.shader.setUniform2f("screenDimensions", ofGetWidth(), ofGetHeight());
    shaderEpilogue(shaderPackageGlowLine);
}

void DrawManager::shadeGlowCircle(ofVec3f center, float radius, ofColor c, float glowIntensity, float glowDampenRadius,
                                  int blendMode, bool toneMap) {
    // NOTE: Because this changes FBO, this resets the OF_MATRIX_MODELVIEW matrix.
    ofVec3f centerGlobal = applyGlobalTransformation(center);
    shaderPackageGlowCircle.shader.begin();
    shaderPackageGlowCircle.shader.setUniform3f("center", centerGlobal);
    shaderPackageGlowCircle.shader.setUniform1f("radius", radius);  // Size-invariant w.r.t short edge (st)
    shaderPackageGlowCircle.shader.setUniform1f("r", c.r / 255.0);
    shaderPackageGlowCircle.shader.setUniform1f("g", c.g / 255.0);
    shaderPackageGlowCircle.shader.setUniform1f("b", c.b / 255.0);
    shaderPackageGlowCircle.shader.setUniform1f("a", c.a / 255.0);
    shaderPackageGlowCircle.shader.setUniform1f("glowIntensity", glowIntensity);
    shaderPackageGlowCircle.shader.setUniform1f("glowDampenRadius", glowDampenRadius);
    shaderPackageGlowCircle.shader.setUniform1i("blendMode", blendMode);
    shaderPackageGlowCircle.shader.setUniform1i("toneMap", toneMap);
    shaderPackageGlowCircle.shader.setUniform2f("screenDimensions", ofGetWidth(), ofGetHeight());
    shaderEpilogue(shaderPackageGlowCircle);
}

void DrawManager::shadeGlowRectangularPrism(ofVec3f a, ofVec3f b, float theta, ofColor c, float glowIntensity,
                                            float glowDampenRadius, int blendMode, bool toneMap) {
    // NOTE: Because this changes FBO, this resets the OF_MATRIX_MODELVIEW matrix.
    ofVec3f aGlobal = applyGlobalTransformation(a);
    ofVec3f bGlobal = applyGlobalTransformation(b);
    shaderPackageGlowRectangularPrism.shader.begin();
    shaderPackageGlowRectangularPrism.shader.setUniform3f("rectA", aGlobal);
    shaderPackageGlowRectangularPrism.shader.setUniform3f("rectB", bGlobal);
    shaderPackageGlowRectangularPrism.shader.setUniform1f("theta", theta);
    shaderPackageGlowRectangularPrism.shader.setUniform1f("r", c.r / 255.0);
    shaderPackageGlowRectangularPrism.shader.setUniform1f("g", c.g / 255.0);
    shaderPackageGlowRectangularPrism.shader.setUniform1f("b", c.b / 255.0);
    shaderPackageGlowRectangularPrism.shader.setUniform1f("a", c.a / 255.0);
    shaderPackageGlowRectangularPrism.shader.setUniform1f("glowIntensity", glowIntensity);
    shaderPackageGlowRectangularPrism.shader.setUniform1f("glowDampenRadius", glowDampenRadius);
    shaderPackageGlowRectangularPrism.shader.setUniform1i("blendMode", blendMode);
    shaderPackageGlowRectangularPrism.shader.setUniform1i("toneMap", toneMap);
    shaderPackageGlowRectangularPrism.shader.setUniform2f("screenDimensions", ofGetWidth(), ofGetHeight());
    shaderEpilogue(shaderPackageGlowRectangularPrism);
}

// void DrawManager::shadeVHS(ofVec2f aberration, float aberrationOpacity) {
//    shaderPackageVHS.shader.setUniform2f("aberration", aberration.x, aberration.y);
//    shaderPackageVHS.shader.setUniform2f("screenDimensions", ofGetWidth(), ofGetHeight());
//    shaderPackageVHS.shader.setUniform1f("opacityPct", aberrationOpacity);
//    shaderEpilogue(shaderPackageVHS);
//}
