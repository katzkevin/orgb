//
//  DisplacementEffect.cpp
//  orgb
//

#include "DisplacementEffect.hpp"

DisplacementEffect::DisplacementEffect()
    : ShaderEffect("Displacement", "post/shaderDisplacement"),
      intensity(20.0f),
      noiseScale(0.01f),
      noiseSpeed(1.0f),
      warpAmount(0.5f),
      turbulence(0.5f) {
    // Add parameters for GUI control
    addParameter(ShaderParameter::Float("Intensity", "intensity", intensity, 0.0f, 100.0f));
    addParameter(ShaderParameter::Float("Noise Scale", "noiseScale", noiseScale, 0.001f, 0.1f));
    addParameter(ShaderParameter::Float("Speed", "noiseSpeed", noiseSpeed, 0.0f, 5.0f));
    addParameter(ShaderParameter::Float("Warp", "warpAmount", warpAmount, 0.0f, 1.0f));
    addParameter(ShaderParameter::Float("Turbulence", "turbulence", turbulence, 0.0f, 1.0f));

    // Setup ofParameter integration
    parameterGroup.setName("Displacement");
    parameterGroup.add(enableParam.set("Enable", true));
    parameterGroup.add(intensityParam.set("Intensity", 20.0f, 0.0f, 100.0f));
    parameterGroup.add(noiseScaleParam.set("Noise Scale", 0.01f, 0.001f, 0.1f));
    parameterGroup.add(noiseSpeedParam.set("Speed", 1.0f, 0.0f, 5.0f));
    parameterGroup.add(warpAmountParam.set("Warp", 0.5f, 0.0f, 1.0f));
    parameterGroup.add(turbulenceParam.set("Turbulence", 0.5f, 0.0f, 1.0f));

    // Add listeners
    enableParam.addListener(this, &DisplacementEffect::onEnableChanged);
    intensityParam.addListener(this, &DisplacementEffect::onIntensityChanged);
    noiseScaleParam.addListener(this, &DisplacementEffect::onNoiseScaleChanged);
    noiseSpeedParam.addListener(this, &DisplacementEffect::onNoiseSpeedChanged);
    warpAmountParam.addListener(this, &DisplacementEffect::onWarpAmountChanged);
    turbulenceParam.addListener(this, &DisplacementEffect::onTurbulenceChanged);
}

void DisplacementEffect::setIntensity(float value) {
    intensity = ofClamp(value, 0.0f, 100.0f);
    for (auto & param : parameters) {
        if (param.name == "Intensity") {
            param.floatValue = intensity;
            break;
        }
    }
}

void DisplacementEffect::setNoiseScale(float value) {
    noiseScale = ofClamp(value, 0.001f, 0.1f);
    for (auto & param : parameters) {
        if (param.name == "Noise Scale") {
            param.floatValue = noiseScale;
            break;
        }
    }
}

void DisplacementEffect::setNoiseSpeed(float value) {
    noiseSpeed = ofClamp(value, 0.0f, 5.0f);
    for (auto & param : parameters) {
        if (param.name == "Speed") {
            param.floatValue = noiseSpeed;
            break;
        }
    }
}

void DisplacementEffect::setWarpAmount(float value) {
    warpAmount = ofClamp(value, 0.0f, 1.0f);
    for (auto & param : parameters) {
        if (param.name == "Warp") {
            param.floatValue = warpAmount;
            break;
        }
    }
}

void DisplacementEffect::setTurbulence(float value) {
    turbulence = ofClamp(value, 0.0f, 1.0f);
    for (auto & param : parameters) {
        if (param.name == "Turbulence") {
            param.floatValue = turbulence;
            break;
        }
    }
}

void DisplacementEffect::configureShaderUniforms(ofShader & shader, const glm::vec2 & resolution) {
    shader.setUniform1f("intensity", intensity);
    shader.setUniform1f("noiseScale", noiseScale);
    shader.setUniform1f("noiseSpeed", noiseSpeed);
    shader.setUniform1f("warpAmount", warpAmount);
    shader.setUniform1f("turbulence", turbulence);
}

void DisplacementEffect::onEnableChanged(bool & value) { setEnabled(value); }

void DisplacementEffect::onIntensityChanged(float & value) { setIntensity(value); }

void DisplacementEffect::onNoiseScaleChanged(float & value) { setNoiseScale(value); }

void DisplacementEffect::onNoiseSpeedChanged(float & value) { setNoiseSpeed(value); }

void DisplacementEffect::onWarpAmountChanged(float & value) { setWarpAmount(value); }

void DisplacementEffect::onTurbulenceChanged(float & value) { setTurbulence(value); }
