//
//  ShaderEffect.hpp
//  orgb
//
//  Base class for post-processing shader effects with validation,
//  parameter management, and runtime toggling.
//

#ifndef ShaderEffect_hpp
#define ShaderEffect_hpp

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "DrawContext.hpp"
#include "ofMain.h"

// ============================================================================
// Parameter System for GUI/Runtime Control
// ============================================================================

enum class ParameterType { FLOAT, INT, BOOL, COLOR };

struct ShaderParameter {
    std::string name;
    std::string uniformName;  // GLSL uniform name
    ParameterType type;

    // Storage (use appropriate field based on type)
    float floatValue;
    int intValue;
    bool boolValue;
    ofFloatColor colorValue;

    // Range constraints (for float/int)
    float minValue;
    float maxValue;

    // Factory methods
    static ShaderParameter Float(const std::string& name, const std::string& uniformName, float defaultValue,
                                  float min = 0.0f, float max = 1.0f) {
        ShaderParameter p;
        p.name = name;
        p.uniformName = uniformName;
        p.type = ParameterType::FLOAT;
        p.floatValue = defaultValue;
        p.minValue = min;
        p.maxValue = max;
        return p;
    }

    static ShaderParameter Int(const std::string& name, const std::string& uniformName, int defaultValue, int min = 0,
                               int max = 10) {
        ShaderParameter p;
        p.name = name;
        p.uniformName = uniformName;
        p.type = ParameterType::INT;
        p.intValue = defaultValue;
        p.minValue = static_cast<float>(min);
        p.maxValue = static_cast<float>(max);
        return p;
    }

    static ShaderParameter Bool(const std::string& name, const std::string& uniformName, bool defaultValue) {
        ShaderParameter p;
        p.name = name;
        p.uniformName = uniformName;
        p.type = ParameterType::BOOL;
        p.boolValue = defaultValue;
        return p;
    }

    static ShaderParameter Color(const std::string& name, const std::string& uniformName, ofFloatColor defaultValue) {
        ShaderParameter p;
        p.name = name;
        p.uniformName = uniformName;
        p.type = ParameterType::COLOR;
        p.colorValue = defaultValue;
        return p;
    }
};

// ============================================================================
// ShaderEffect Base Class
// ============================================================================

class ShaderEffect {
   public:
    ShaderEffect(const std::string& name, const std::string& shaderName);
    virtual ~ShaderEffect() = default;

    // Main interface - apply effect to input FBO, write to output FBO
    virtual void apply(ofFbo& input, ofFbo& output);

    // Effect state
    bool isEnabled() const { return enabled; }
    void setEnabled(bool state) { enabled = state; }
    void toggle() { enabled = !enabled; }

    // Metadata
    std::string getName() const { return effectName; }
    std::string getShaderName() const { return shaderFileName; }

    // Parameter access
    std::vector<ShaderParameter>& getParameters() { return parameters; }
    const std::vector<ShaderParameter>& getParameters() const { return parameters; }

    // Validation
    bool isValid() const { return shaderLoaded && shaderCompiled; }
    std::string getErrorMessage() const { return errorMessage; }

   protected:
    // Subclasses override this to configure shader uniforms
    virtual void configureShaderUniforms(ofShader& shader, const glm::vec2& resolution);

    // Subclasses can add parameters in constructor
    void addParameter(const ShaderParameter& param) { parameters.push_back(param); }

    // Helper to set parameter uniforms automatically
    void applyParametersToShader(ofShader& shader);

    // Shader management
    bool loadShader();
    bool validateShader();

    // State
    std::string effectName;
    std::string shaderFileName;
    bool enabled;
    bool shaderLoaded;
    bool shaderCompiled;
    std::string errorMessage;

    ofShader shader;
    std::vector<ShaderParameter> parameters;
};

// ============================================================================
// Concrete Effect Implementations
// ============================================================================

// Passthrough effect (useful for testing)
class PassthroughEffect : public ShaderEffect {
   public:
    PassthroughEffect() : ShaderEffect("Passthrough", "") { enabled = false; }  // No shader needed
    void apply(ofFbo& input, ofFbo& output) override;
};

#endif /* ShaderEffect_hpp */
