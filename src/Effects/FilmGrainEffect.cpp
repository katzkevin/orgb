//
//  FilmGrainEffect.cpp
//  orgb
//

#include "FilmGrainEffect.hpp"

FilmGrainEffect::FilmGrainEffect()
    : ShaderEffect("Film Grain", "post/shaderFilmGrain"), intensity(0.05f), grainSize(1.5f) {
    // Add parameters for GUI control
    addParameter(ShaderParameter::Float("Intensity", "grainIntensity", intensity, 0.0f, 1.0f));
    addParameter(ShaderParameter::Float("Grain Size", "grainSize", grainSize, 1.0f, 4.0f));

    // Setup ofParameter integration
    parameterGroup.setName("Film Grain");
    parameterGroup.add(enableParam.set("Enable", true));
    parameterGroup.add(intensityParam.set("Intensity", 0.05f, 0.0f, 1.0f));
    parameterGroup.add(grainSizeParam.set("Grain Size", 1.5f, 1.0f, 4.0f));

    // Add listeners
    enableParam.addListener(this, &FilmGrainEffect::onEnableChanged);
    intensityParam.addListener(this, &FilmGrainEffect::onIntensityChanged);
    grainSizeParam.addListener(this, &FilmGrainEffect::onGrainSizeChanged);
}

void FilmGrainEffect::setIntensity(float value) {
    intensity = ofClamp(value, 0.0f, 1.0f);
    // Update parameter
    for (auto& param : parameters) {
        if (param.name == "Intensity") {
            param.floatValue = intensity;
            break;
        }
    }
}

void FilmGrainEffect::setGrainSize(float value) {
    grainSize = ofClamp(value, 1.0f, 4.0f);
    // Update parameter
    for (auto& param : parameters) {
        if (param.name == "Grain Size") {
            param.floatValue = grainSize;
            break;
        }
    }
}

void FilmGrainEffect::configureShaderUniforms(ofShader& shader, const glm::vec2& resolution) {
    shader.setUniform1f("grainIntensity", intensity);
    shader.setUniform1f("grainSize", grainSize);
}

void FilmGrainEffect::onEnableChanged(bool& value) {
    setEnabled(value);
}

void FilmGrainEffect::onIntensityChanged(float& value) {
    setIntensity(value);
}

void FilmGrainEffect::onGrainSizeChanged(float& value) {
    setGrainSize(value);
}
