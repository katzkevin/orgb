//
//  ChromaticAberrationEffect.hpp
//  orgb
//
//  Aggressive chromatic aberration for chaotic RGB channel separation
//

#ifndef ChromaticAberrationEffect_hpp
#define ChromaticAberrationEffect_hpp

#include "../ShaderEffect.hpp"

class ChromaticAberrationEffect : public ShaderEffect {
   public:
    ChromaticAberrationEffect();

    // Parameter accessors
    void setIntensity(float value);
    void setDirection(float value);      // 0-360 degrees
    void setRadialAmount(float value);   // radial distortion
    void setNoiseAmount(float value);    // chaos/instability

    float getIntensity() const { return intensity; }
    float getDirection() const { return direction; }
    float getRadialAmount() const { return radialAmount; }
    float getNoiseAmount() const { return noiseAmount; }

    // GUI integration
    ofParameterGroup & getParameterGroup() { return parameterGroup; }

   protected:
    void configureShaderUniforms(ofShader & shader, const glm::vec2 & resolution) override;

   private:
    float intensity;      // 0.0 - 100.0 pixels
    float direction;      // 0.0 - 360.0 degrees
    float radialAmount;   // 0.0 - 1.0
    float noiseAmount;    // 0.0 - 1.0

    // ofParameter integration for GUI
    ofParameterGroup parameterGroup;
    ofParameter<bool> enableParam;
    ofParameter<float> intensityParam;
    ofParameter<float> directionParam;
    ofParameter<float> radialAmountParam;
    ofParameter<float> noiseAmountParam;

    void onEnableChanged(bool & value);
    void onIntensityChanged(float & value);
    void onDirectionChanged(float & value);
    void onRadialAmountChanged(float & value);
    void onNoiseAmountChanged(float & value);
};

#endif /* ChromaticAberrationEffect_hpp */
