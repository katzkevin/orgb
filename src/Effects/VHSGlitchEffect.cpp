//
//  VHSGlitchEffect.cpp
//  orgb
//

#include "VHSGlitchEffect.hpp"

VHSGlitchEffect::VHSGlitchEffect()
    : ShaderEffect("VHS Glitch", "post/shaderVHSGlitch"),
      intensity(0.5f),
      lineDisplacement(20.0f),
      colorBleed(0.4f),
      signalNoise(0.3f),
      trackingError(0.2f) {
    // Add parameters for GUI control
    addParameter(ShaderParameter::Float("Intensity", "intensity", intensity, 0.0f, 1.0f));
    addParameter(ShaderParameter::Float("Line Displacement", "lineDisplacement", lineDisplacement, 0.0f, 100.0f));
    addParameter(ShaderParameter::Float("Color Bleed", "colorBleed", colorBleed, 0.0f, 1.0f));
    addParameter(ShaderParameter::Float("Signal Noise", "signalNoise", signalNoise, 0.0f, 1.0f));
    addParameter(ShaderParameter::Float("Tracking Error", "trackingError", trackingError, 0.0f, 1.0f));

    // Setup ofParameter integration
    parameterGroup.setName("VHS Glitch");
    parameterGroup.add(enableParam.set("Enable", true));
    parameterGroup.add(intensityParam.set("Intensity", 0.5f, 0.0f, 1.0f));
    parameterGroup.add(lineDisplacementParam.set("Line Shift", 20.0f, 0.0f, 100.0f));
    parameterGroup.add(colorBleedParam.set("Color Bleed", 0.4f, 0.0f, 1.0f));
    parameterGroup.add(signalNoiseParam.set("Signal Noise", 0.3f, 0.0f, 1.0f));
    parameterGroup.add(trackingErrorParam.set("Tracking", 0.2f, 0.0f, 1.0f));

    // Add listeners
    enableParam.addListener(this, &VHSGlitchEffect::onEnableChanged);
    intensityParam.addListener(this, &VHSGlitchEffect::onIntensityChanged);
    lineDisplacementParam.addListener(this, &VHSGlitchEffect::onLineDisplacementChanged);
    colorBleedParam.addListener(this, &VHSGlitchEffect::onColorBleedChanged);
    signalNoiseParam.addListener(this, &VHSGlitchEffect::onSignalNoiseChanged);
    trackingErrorParam.addListener(this, &VHSGlitchEffect::onTrackingErrorChanged);
}

void VHSGlitchEffect::setIntensity(float value) {
    intensity = ofClamp(value, 0.0f, 1.0f);
    for (auto & param : parameters) {
        if (param.name == "Intensity") {
            param.floatValue = intensity;
            break;
        }
    }
}

void VHSGlitchEffect::setLineDisplacement(float value) {
    lineDisplacement = ofClamp(value, 0.0f, 100.0f);
    for (auto & param : parameters) {
        if (param.name == "Line Displacement") {
            param.floatValue = lineDisplacement;
            break;
        }
    }
}

void VHSGlitchEffect::setColorBleed(float value) {
    colorBleed = ofClamp(value, 0.0f, 1.0f);
    for (auto & param : parameters) {
        if (param.name == "Color Bleed") {
            param.floatValue = colorBleed;
            break;
        }
    }
}

void VHSGlitchEffect::setSignalNoise(float value) {
    signalNoise = ofClamp(value, 0.0f, 1.0f);
    for (auto & param : parameters) {
        if (param.name == "Signal Noise") {
            param.floatValue = signalNoise;
            break;
        }
    }
}

void VHSGlitchEffect::setTrackingError(float value) {
    trackingError = ofClamp(value, 0.0f, 1.0f);
    for (auto & param : parameters) {
        if (param.name == "Tracking Error") {
            param.floatValue = trackingError;
            break;
        }
    }
}

void VHSGlitchEffect::configureShaderUniforms(ofShader & shader, const glm::vec2 & resolution) {
    shader.setUniform1f("intensity", intensity);
    shader.setUniform1f("lineDisplacement", lineDisplacement);
    shader.setUniform1f("colorBleed", colorBleed);
    shader.setUniform1f("signalNoise", signalNoise);
    shader.setUniform1f("trackingError", trackingError);
}

void VHSGlitchEffect::onEnableChanged(bool & value) { setEnabled(value); }

void VHSGlitchEffect::onIntensityChanged(float & value) { setIntensity(value); }

void VHSGlitchEffect::onLineDisplacementChanged(float & value) { setLineDisplacement(value); }

void VHSGlitchEffect::onColorBleedChanged(float & value) { setColorBleed(value); }

void VHSGlitchEffect::onSignalNoiseChanged(float & value) { setSignalNoise(value); }

void VHSGlitchEffect::onTrackingErrorChanged(float & value) { setTrackingError(value); }
