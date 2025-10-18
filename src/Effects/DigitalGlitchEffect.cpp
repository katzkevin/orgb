//
//  DigitalGlitchEffect.cpp
//  orgb
//

#include "DigitalGlitchEffect.hpp"

DigitalGlitchEffect::DigitalGlitchEffect()
    : ShaderEffect("Digital Glitch", "post/shaderDigitalGlitch"),
      intensity(0.2f),
      blockSize(16.0f),
      colorShift(5.0f),
      noiseAmount(0.1f) {
    // Add parameters for GUI control
    addParameter(ShaderParameter::Float("Intensity", "glitchIntensity", intensity, 0.0f, 1.0f));
    addParameter(ShaderParameter::Float("Block Size", "blockSize", blockSize, 8.0f, 64.0f));
    addParameter(ShaderParameter::Float("Color Shift", "colorShiftAmount", colorShift, 0.0f, 20.0f));
    addParameter(ShaderParameter::Float("Noise", "noiseAmount", noiseAmount, 0.0f, 1.0f));

    // Setup ofParameter integration
    parameterGroup.setName("Digital Glitch");
    parameterGroup.add(enableParam.set("Enable", false));  // Start disabled
    parameterGroup.add(intensityParam.set("Intensity", 0.2f, 0.0f, 1.0f));
    parameterGroup.add(blockSizeParam.set("Block Size", 16.0f, 8.0f, 64.0f));
    parameterGroup.add(colorShiftParam.set("Color Shift", 5.0f, 0.0f, 20.0f));
    parameterGroup.add(noiseAmountParam.set("Noise", 0.1f, 0.0f, 1.0f));

    // Add listeners
    enableParam.addListener(this, &DigitalGlitchEffect::onEnableChanged);
    intensityParam.addListener(this, &DigitalGlitchEffect::onIntensityChanged);
    blockSizeParam.addListener(this, &DigitalGlitchEffect::onBlockSizeChanged);
    colorShiftParam.addListener(this, &DigitalGlitchEffect::onColorShiftChanged);
    noiseAmountParam.addListener(this, &DigitalGlitchEffect::onNoiseAmountChanged);

    // Glitch effects often work better when not constantly enabled
    // Start disabled by default
    setEnabled(false);
}

void DigitalGlitchEffect::setIntensity(float value) {
    intensity = ofClamp(value, 0.0f, 1.0f);
    for (auto& param : parameters) {
        if (param.name == "Intensity") {
            param.floatValue = intensity;
            break;
        }
    }
}

void DigitalGlitchEffect::setBlockSize(float value) {
    blockSize = ofClamp(value, 8.0f, 64.0f);
    for (auto& param : parameters) {
        if (param.name == "Block Size") {
            param.floatValue = blockSize;
            break;
        }
    }
}

void DigitalGlitchEffect::setColorShift(float value) {
    colorShift = ofClamp(value, 0.0f, 20.0f);
    for (auto& param : parameters) {
        if (param.name == "Color Shift") {
            param.floatValue = colorShift;
            break;
        }
    }
}

void DigitalGlitchEffect::setNoiseAmount(float value) {
    noiseAmount = ofClamp(value, 0.0f, 1.0f);
    for (auto& param : parameters) {
        if (param.name == "Noise") {
            param.floatValue = noiseAmount;
            break;
        }
    }
}

void DigitalGlitchEffect::configureShaderUniforms(ofShader& shader, const glm::vec2& resolution) {
    shader.setUniform1f("glitchIntensity", intensity);
    shader.setUniform1f("blockSize", blockSize);
    shader.setUniform1f("colorShiftAmount", colorShift);
    shader.setUniform1f("noiseAmount", noiseAmount);
}

void DigitalGlitchEffect::onEnableChanged(bool& value) {
    setEnabled(value);
}

void DigitalGlitchEffect::onIntensityChanged(float& value) {
    setIntensity(value);
}

void DigitalGlitchEffect::onBlockSizeChanged(float& value) {
    setBlockSize(value);
}

void DigitalGlitchEffect::onColorShiftChanged(float& value) {
    setColorShift(value);
}

void DigitalGlitchEffect::onNoiseAmountChanged(float& value) {
    setNoiseAmount(value);
}
