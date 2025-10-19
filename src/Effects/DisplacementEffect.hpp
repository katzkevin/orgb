//
//  DisplacementEffect.hpp
//  orgb
//
//  Noise-based displacement/warp for organic chaos
//

#ifndef DisplacementEffect_hpp
#define DisplacementEffect_hpp

#include "../ShaderEffect.hpp"

class DisplacementEffect : public ShaderEffect {
   public:
    DisplacementEffect();

    // Parameter accessors
    void setIntensity(float value);       // overall displacement strength
    void setNoiseScale(float value);      // scale of noise pattern
    void setNoiseSpeed(float value);      // animation speed
    void setWarpAmount(float value);      // secondary warp effect
    void setTurbulence(float value);      // multi-octave noise intensity

    float getIntensity() const { return intensity; }
    float getNoiseScale() const { return noiseScale; }
    float getNoiseSpeed() const { return noiseSpeed; }
    float getWarpAmount() const { return warpAmount; }
    float getTurbulence() const { return turbulence; }

    // GUI integration
    ofParameterGroup & getParameterGroup() { return parameterGroup; }

   protected:
    void configureShaderUniforms(ofShader & shader, const glm::vec2 & resolution) override;

   private:
    float intensity;      // 0.0 - 100.0 pixels
    float noiseScale;     // 0.001 - 0.1
    float noiseSpeed;     // 0.0 - 5.0
    float warpAmount;     // 0.0 - 1.0
    float turbulence;     // 0.0 - 1.0

    // ofParameter integration for GUI
    ofParameterGroup parameterGroup;
    ofParameter<bool> enableParam;
    ofParameter<float> intensityParam;
    ofParameter<float> noiseScaleParam;
    ofParameter<float> noiseSpeedParam;
    ofParameter<float> warpAmountParam;
    ofParameter<float> turbulenceParam;

    void onEnableChanged(bool & value);
    void onIntensityChanged(float & value);
    void onNoiseScaleChanged(float & value);
    void onNoiseSpeedChanged(float & value);
    void onWarpAmountChanged(float & value);
    void onTurbulenceChanged(float & value);
};

#endif /* DisplacementEffect_hpp */
