//
//  FeedbackEffect.cpp
//  orgb
//

#include "FeedbackEffect.hpp"

FeedbackEffect::FeedbackEffect()
    : ShaderEffect("Feedback", "post/shaderFeedback"),
      intensity(0.85f),
      displacementX(0.0f),
      displacementY(0.0f),
      scale(0.998f),
      rotation(0.0f),
      feedbackColor(0.0f) {
    // Add parameters for GUI control
    addParameter(ShaderParameter::Float("Intensity", "intensity", intensity, 0.0f, 1.0f));
    addParameter(ShaderParameter::Float("Displacement X", "displacementX", displacementX, -10.0f, 10.0f));
    addParameter(ShaderParameter::Float("Displacement Y", "displacementY", displacementY, -10.0f, 10.0f));
    addParameter(ShaderParameter::Float("Scale", "scale", scale, 0.95f, 1.05f));
    addParameter(ShaderParameter::Float("Rotation", "rotation", rotation, -5.0f, 5.0f));
    addParameter(ShaderParameter::Float("Color Shift", "feedbackColor", feedbackColor, 0.0f, 1.0f));

    // Setup ofParameter integration
    parameterGroup.setName("Feedback");
    parameterGroup.add(enableParam.set("Enable", true));
    parameterGroup.add(intensityParam.set("Intensity", 0.85f, 0.0f, 1.0f));
    parameterGroup.add(displacementXParam.set("Shift X", 0.0f, -10.0f, 10.0f));
    parameterGroup.add(displacementYParam.set("Shift Y", 0.0f, -10.0f, 10.0f));
    parameterGroup.add(scaleParam.set("Scale", 0.998f, 0.95f, 1.05f));
    parameterGroup.add(rotationParam.set("Rotation", 0.0f, -5.0f, 5.0f));
    parameterGroup.add(feedbackColorParam.set("Color Shift", 0.0f, 0.0f, 1.0f));

    // Add listeners
    enableParam.addListener(this, &FeedbackEffect::onEnableChanged);
    intensityParam.addListener(this, &FeedbackEffect::onIntensityChanged);
    displacementXParam.addListener(this, &FeedbackEffect::onDisplacementXChanged);
    displacementYParam.addListener(this, &FeedbackEffect::onDisplacementYChanged);
    scaleParam.addListener(this, &FeedbackEffect::onScaleChanged);
    rotationParam.addListener(this, &FeedbackEffect::onRotationChanged);
    feedbackColorParam.addListener(this, &FeedbackEffect::onFeedbackColorChanged);
}

void FeedbackEffect::setIntensity(float value) {
    intensity = ofClamp(value, 0.0f, 1.0f);
    for (auto & param : parameters) {
        if (param.name == "Intensity") {
            param.floatValue = intensity;
            break;
        }
    }
}

void FeedbackEffect::setDisplacementX(float value) {
    displacementX = ofClamp(value, -10.0f, 10.0f);
    for (auto & param : parameters) {
        if (param.name == "Displacement X") {
            param.floatValue = displacementX;
            break;
        }
    }
}

void FeedbackEffect::setDisplacementY(float value) {
    displacementY = ofClamp(value, -10.0f, 10.0f);
    for (auto & param : parameters) {
        if (param.name == "Displacement Y") {
            param.floatValue = displacementY;
            break;
        }
    }
}

void FeedbackEffect::setScale(float value) {
    scale = ofClamp(value, 0.95f, 1.05f);
    for (auto & param : parameters) {
        if (param.name == "Scale") {
            param.floatValue = scale;
            break;
        }
    }
}

void FeedbackEffect::setRotation(float value) {
    rotation = ofClamp(value, -5.0f, 5.0f);
    for (auto & param : parameters) {
        if (param.name == "Rotation") {
            param.floatValue = rotation;
            break;
        }
    }
}

void FeedbackEffect::setFeedbackColor(float value) {
    feedbackColor = ofClamp(value, 0.0f, 1.0f);
    for (auto & param : parameters) {
        if (param.name == "Color Shift") {
            param.floatValue = feedbackColor;
            break;
        }
    }
}

void FeedbackEffect::configureShaderUniforms(ofShader & shader, const glm::vec2 & resolution) {
    shader.setUniform1f("intensity", intensity);
    shader.setUniform1f("displacementX", displacementX);
    shader.setUniform1f("displacementY", displacementY);
    shader.setUniform1f("scale", scale);
    shader.setUniform1f("rotation", rotation);
    shader.setUniform1f("feedbackColor", feedbackColor);
}

void FeedbackEffect::onEnableChanged(bool & value) { setEnabled(value); }

void FeedbackEffect::onIntensityChanged(float & value) { setIntensity(value); }

void FeedbackEffect::onDisplacementXChanged(float & value) { setDisplacementX(value); }

void FeedbackEffect::onDisplacementYChanged(float & value) { setDisplacementY(value); }

void FeedbackEffect::onScaleChanged(float & value) { setScale(value); }

void FeedbackEffect::onRotationChanged(float & value) { setRotation(value); }

void FeedbackEffect::onFeedbackColorChanged(float & value) { setFeedbackColor(value); }
