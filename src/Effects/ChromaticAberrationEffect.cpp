//
//  ChromaticAberrationEffect.cpp
//  orgb
//

#include "ChromaticAberrationEffect.hpp"

ChromaticAberrationEffect::ChromaticAberrationEffect()
    : ShaderEffect("Chromatic Aberration", "post/shaderChromaticAberration"),
      intensity(5.0f),
      direction(0.0f),
      radialAmount(0.3f),
      noiseAmount(0.2f) {
    // Add parameters for GUI control
    addParameter(ShaderParameter::Float("Intensity", "intensity", intensity, 0.0f, 50.0f));
    addParameter(ShaderParameter::Float("Direction", "direction", direction, 0.0f, 360.0f));
    addParameter(ShaderParameter::Float("Radial Amount", "radialAmount", radialAmount, 0.0f, 1.0f));
    addParameter(ShaderParameter::Float("Noise Amount", "noiseAmount", noiseAmount, 0.0f, 1.0f));

    // Setup ofParameter integration
    parameterGroup.setName("Chromatic Aberration");
    parameterGroup.add(enableParam.set("Enable", true));
    parameterGroup.add(intensityParam.set("Intensity", 5.0f, 0.0f, 50.0f));
    parameterGroup.add(directionParam.set("Direction", 0.0f, 0.0f, 360.0f));
    parameterGroup.add(radialAmountParam.set("Radial", 0.3f, 0.0f, 1.0f));
    parameterGroup.add(noiseAmountParam.set("Chaos", 0.2f, 0.0f, 1.0f));

    // Add listeners
    enableParam.addListener(this, &ChromaticAberrationEffect::onEnableChanged);
    intensityParam.addListener(this, &ChromaticAberrationEffect::onIntensityChanged);
    directionParam.addListener(this, &ChromaticAberrationEffect::onDirectionChanged);
    radialAmountParam.addListener(this, &ChromaticAberrationEffect::onRadialAmountChanged);
    noiseAmountParam.addListener(this, &ChromaticAberrationEffect::onNoiseAmountChanged);
}

void ChromaticAberrationEffect::setIntensity(float value) {
    intensity = ofClamp(value, 0.0f, 50.0f);
    for (auto & param : parameters) {
        if (param.name == "Intensity") {
            param.floatValue = intensity;
            break;
        }
    }
}

void ChromaticAberrationEffect::setDirection(float value) {
    direction = fmod(value, 360.0f);
    if (direction < 0.0f) direction += 360.0f;
    for (auto & param : parameters) {
        if (param.name == "Direction") {
            param.floatValue = direction;
            break;
        }
    }
}

void ChromaticAberrationEffect::setRadialAmount(float value) {
    radialAmount = ofClamp(value, 0.0f, 1.0f);
    for (auto & param : parameters) {
        if (param.name == "Radial Amount") {
            param.floatValue = radialAmount;
            break;
        }
    }
}

void ChromaticAberrationEffect::setNoiseAmount(float value) {
    noiseAmount = ofClamp(value, 0.0f, 1.0f);
    for (auto & param : parameters) {
        if (param.name == "Noise Amount") {
            param.floatValue = noiseAmount;
            break;
        }
    }
}

void ChromaticAberrationEffect::configureShaderUniforms(ofShader & shader, const glm::vec2 & resolution) {
    shader.setUniform1f("intensity", intensity);
    shader.setUniform1f("direction", direction);
    shader.setUniform1f("radialAmount", radialAmount);
    shader.setUniform1f("noiseAmount", noiseAmount);
}

void ChromaticAberrationEffect::onEnableChanged(bool & value) { setEnabled(value); }

void ChromaticAberrationEffect::onIntensityChanged(float & value) { setIntensity(value); }

void ChromaticAberrationEffect::onDirectionChanged(float & value) { setDirection(value); }

void ChromaticAberrationEffect::onRadialAmountChanged(float & value) { setRadialAmount(value); }

void ChromaticAberrationEffect::onNoiseAmountChanged(float & value) { setNoiseAmount(value); }
