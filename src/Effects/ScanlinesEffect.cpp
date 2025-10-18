//
//  ScanlinesEffect.cpp
//  orgb
//

#include "ScanlinesEffect.hpp"

ScanlinesEffect::ScanlinesEffect()
    : ShaderEffect("Scanlines", "post/shaderScanlines"),
      intensity(0.3f),
      scanlineCount(200.0f),
      speed(0.1f),
      vignette(0.3f),
      rgbShift(0.5f) {
    // Add parameters for GUI control
    addParameter(ShaderParameter::Float("Intensity", "scanlineIntensity", intensity, 0.0f, 1.0f));
    addParameter(ShaderParameter::Float("Line Count", "scanlineCount", scanlineCount, 50.0f, 500.0f));
    addParameter(ShaderParameter::Float("Speed", "scanlineSpeed", speed, 0.0f, 1.0f));
    addParameter(ShaderParameter::Float("Vignette", "vignette", vignette, 0.0f, 1.0f));
    addParameter(ShaderParameter::Float("RGB Shift", "rgbShift", rgbShift, 0.0f, 5.0f));

    // Setup ofParameter integration
    parameterGroup.setName("Scanlines");
    parameterGroup.add(enableParam.set("Enable", true));
    parameterGroup.add(intensityParam.set("Intensity", 0.3f, 0.0f, 1.0f));
    parameterGroup.add(scanlineCountParam.set("Line Count", 200.0f, 50.0f, 500.0f));
    parameterGroup.add(speedParam.set("Speed", 0.1f, 0.0f, 1.0f));
    parameterGroup.add(vignetteParam.set("Vignette", 0.3f, 0.0f, 1.0f));
    parameterGroup.add(rgbShiftParam.set("RGB Shift", 0.5f, 0.0f, 5.0f));

    // Add listeners
    enableParam.addListener(this, &ScanlinesEffect::onEnableChanged);
    intensityParam.addListener(this, &ScanlinesEffect::onIntensityChanged);
    scanlineCountParam.addListener(this, &ScanlinesEffect::onScanlineCountChanged);
    speedParam.addListener(this, &ScanlinesEffect::onSpeedChanged);
    vignetteParam.addListener(this, &ScanlinesEffect::onVignetteChanged);
    rgbShiftParam.addListener(this, &ScanlinesEffect::onRGBShiftChanged);
}

void ScanlinesEffect::setIntensity(float value) {
    intensity = ofClamp(value, 0.0f, 1.0f);
    for (auto & param : parameters) {
        if (param.name == "Intensity") {
            param.floatValue = intensity;
            break;
        }
    }
}

void ScanlinesEffect::setScanlineCount(float value) {
    scanlineCount = ofClamp(value, 50.0f, 500.0f);
    for (auto & param : parameters) {
        if (param.name == "Line Count") {
            param.floatValue = scanlineCount;
            break;
        }
    }
}

void ScanlinesEffect::setSpeed(float value) {
    speed = ofClamp(value, 0.0f, 1.0f);
    for (auto & param : parameters) {
        if (param.name == "Speed") {
            param.floatValue = speed;
            break;
        }
    }
}

void ScanlinesEffect::setVignette(float value) {
    vignette = ofClamp(value, 0.0f, 1.0f);
    for (auto & param : parameters) {
        if (param.name == "Vignette") {
            param.floatValue = vignette;
            break;
        }
    }
}

void ScanlinesEffect::setRGBShift(float value) {
    rgbShift = ofClamp(value, 0.0f, 5.0f);
    for (auto & param : parameters) {
        if (param.name == "RGB Shift") {
            param.floatValue = rgbShift;
            break;
        }
    }
}

void ScanlinesEffect::configureShaderUniforms(ofShader & shader, const glm::vec2 & resolution) {
    shader.setUniform1f("scanlineIntensity", intensity);
    shader.setUniform1f("scanlineCount", scanlineCount);
    shader.setUniform1f("scanlineSpeed", speed);
    shader.setUniform1f("vignette", vignette);
    shader.setUniform1f("rgbShift", rgbShift);
}

void ScanlinesEffect::onEnableChanged(bool & value) { setEnabled(value); }

void ScanlinesEffect::onIntensityChanged(float & value) { setIntensity(value); }

void ScanlinesEffect::onScanlineCountChanged(float & value) { setScanlineCount(value); }

void ScanlinesEffect::onSpeedChanged(float & value) { setSpeed(value); }

void ScanlinesEffect::onVignetteChanged(float & value) { setVignette(value); }

void ScanlinesEffect::onRGBShiftChanged(float & value) { setRGBShift(value); }
