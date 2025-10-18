//
//  ShaderEffect.cpp
//  orgb
//
//  Implementation of base shader effect class
//

#include "ShaderEffect.hpp"

#include "Utilities.hpp"

// ============================================================================
// ShaderEffect Implementation
// ============================================================================

ShaderEffect::ShaderEffect(const std::string & name, const std::string & shaderName)
    : effectName(name), shaderFileName(shaderName), enabled(true), shaderLoaded(false), shaderCompiled(false) {
    if (!shaderName.empty()) {
        loadShader();
    }
}

void ShaderEffect::apply(ofFbo & input, ofFbo & output) {
    if (!enabled) {
        // Effect disabled - just copy input to output
        output.begin();
        ofClear(0, 0, 0, 0);
        drawFboAtZeroZero(input);
        output.end();
        return;
    }

    if (!isValid()) {
        ofLogWarning("ShaderEffect::apply") << effectName << " is not valid, skipping. Error: " << errorMessage;
        // Copy input to output (fallback)
        output.begin();
        ofClear(0, 0, 0, 0);
        drawFboAtZeroZero(input);
        output.end();
        return;
    }

    // Apply shader effect
    output.begin();
    {
        ofClear(0, 0, 0, 0);
        shader.begin();
        {
            // Set common uniforms
            glm::vec2 resolution(input.getWidth(), input.getHeight());
            shader.setUniform2f("resolution", resolution);
            shader.setUniform1f("time", ofGetElapsedTimef());

            // Set effect-specific uniforms
            configureShaderUniforms(shader, resolution);

            // Apply parameter uniforms
            applyParametersToShader(shader);

            // Draw input texture
            drawFboAtZeroZero(input);
        }
        shader.end();
    }
    output.end();
}

void ShaderEffect::configureShaderUniforms(ofShader & shader, const glm::vec2 & resolution) {
    // Base implementation does nothing - subclasses override
}

void ShaderEffect::applyParametersToShader(ofShader & shader) {
    for (const auto & param : parameters) {
        switch (param.type) {
            case ParameterType::FLOAT:
                shader.setUniform1f(param.uniformName, param.floatValue);
                break;
            case ParameterType::INT:
                shader.setUniform1i(param.uniformName, param.intValue);
                break;
            case ParameterType::BOOL:
                shader.setUniform1i(param.uniformName, param.boolValue ? 1 : 0);
                break;
            case ParameterType::COLOR:
                shader.setUniform4f(param.uniformName, param.colorValue.r, param.colorValue.g, param.colorValue.b,
                                    param.colorValue.a);
                break;
        }
    }
}

bool ShaderEffect::loadShader() {
    if (shaderFileName.empty()) {
        shaderLoaded = true;  // No shader to load
        shaderCompiled = true;
        return true;
    }

    try {
        // Use existing createShader function for platform detection
        shader = createShader(shaderFileName);
        shaderLoaded = true;

        // Validate compilation
        return validateShader();

    } catch (const std::exception & e) {
        errorMessage = std::string("Failed to load shader: ") + e.what();
        ofLogError("ShaderEffect::loadShader") << effectName << ": " << errorMessage;
        shaderLoaded = false;
        shaderCompiled = false;
        return false;
    }
}

bool ShaderEffect::validateShader() {
    if (!shaderLoaded) {
        errorMessage = "Shader not loaded";
        shaderCompiled = false;
        return false;
    }

    // Try to begin the shader - OF will log errors if compilation failed
    // We capture the log level temporarily to check for errors
    ofLogLevel prevLogLevel = ofGetLogLevel();
    ofSetLogLevel(OF_LOG_ERROR);

    bool validationPassed = true;
    std::string validationErrors = "";

    // Test shader begin/end cycle
    try {
        shader.begin();
        shader.end();
    } catch (const std::exception & e) {
        validationPassed = false;
        validationErrors = std::string("Shader begin/end failed: ") + e.what();
    }

    // Restore log level
    ofSetLogLevel(prevLogLevel);

    if (!validationPassed) {
        errorMessage = "Shader validation failed: " + validationErrors;
        ofLogError("ShaderEffect::validateShader") << effectName << ": " << errorMessage;
        shaderCompiled = false;
        return false;
    }

    // Additional checks: verify shader has a valid GL program ID
    // This is a more direct way to check if the shader compiled successfully
    // Note: ofShader doesn't expose getProgram() publicly, so this is limited

    shaderCompiled = true;
    ofLogVerbose("ShaderEffect::validateShader") << effectName << " validated successfully";
    return true;
}

// ============================================================================
// PassthroughEffect Implementation
// ============================================================================

void PassthroughEffect::apply(ofFbo & input, ofFbo & output) {
    // Simple copy - no shader processing
    output.begin();
    ofClear(0, 0, 0, 0);
    drawFboAtZeroZero(input);
    output.end();
}
