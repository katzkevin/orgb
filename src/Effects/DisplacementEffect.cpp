//
//  DisplacementEffect.cpp
//  orgb
//

#include "DisplacementEffect.hpp"

DisplacementEffect::DisplacementEffect()
    : ShaderEffect("Displacement", "post/shaderDisplacement"),
      strength(0.5f),
      frequency(1.0f),
      blockSize(10.0f) {
    // Add parameters for GUI control
    addParameter(ShaderParameter::Float("Strength", "displacementStrength", strength, 0.0f, 1.0f));
    addParameter(ShaderParameter::Float("Frequency", "frequency", frequency, 0.1f, 10.0f));
    addParameter(
        ShaderParameter::Float("Block Size", "blockSize", blockSize, 1.0f, 50.0f));

    // Setup ofParameter integration
    parameterGroup.setName("Displacement");
    parameterGroup.add(enableParam.set("Enable", true));
    parameterGroup.add(strengthParam.set("Strength", 0.5f, 0.0f, 1.0f));
    parameterGroup.add(frequencyParam.set("Frequency", 1.0f, 0.1f, 10.0f));
    parameterGroup.add(blockSizeParam.set("Block Size", 10.0f, 1.0f, 50.0f));

    // Add listeners
    enableParam.addListener(this, &DisplacementEffect::onEnableChanged);
    strengthParam.addListener(this, &DisplacementEffect::onStrengthChanged);
    frequencyParam.addListener(this, &DisplacementEffect::onFrequencyChanged);
    blockSizeParam.addListener(this, &DisplacementEffect::onBlockSizeChanged);
}

void DisplacementEffect::setStrength(float value) {
    strength = ofClamp(value, 0.0f, 1.0f);
    // Update parameter
    for (auto & param : parameters) {
        if (param.name == "Strength") {
            param.floatValue = strength;
            break;
        }
    }
}

void DisplacementEffect::setFrequency(float value) {
    frequency = ofClamp(value, 0.1f, 10.0f);
    // Update parameter
    for (auto & param : parameters) {
        if (param.name == "Frequency") {
            param.floatValue = frequency;
            break;
        }
    }
}

void DisplacementEffect::setBlockSize(float value) {
    blockSize = ofClamp(value, 1.0f, 50.0f);
    // Update parameter
    for (auto & param : parameters) {
        if (param.name == "Block Size") {
            param.floatValue = blockSize;
            break;
        }
    }
}

void DisplacementEffect::configureShaderUniforms(ofShader & shader,
                                                  const glm::vec2 & resolution) {
    shader.setUniform1f("displacementStrength", strength);
    shader.setUniform1f("frequency", frequency);
    shader.setUniform1f("blockSize", blockSize);
}

void DisplacementEffect::onEnableChanged(bool & value) { setEnabled(value); }

void DisplacementEffect::onStrengthChanged(float & value) { setStrength(value); }

void DisplacementEffect::onFrequencyChanged(float & value) { setFrequency(value); }

void DisplacementEffect::onBlockSizeChanged(float & value) { setBlockSize(value); }
