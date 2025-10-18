//
//  ScanlinesEffect.hpp
//  orgb
//
//  CRT-style scanlines with optional vignette and RGB shift
//

#ifndef ScanlinesEffect_hpp
#define ScanlinesEffect_hpp

#include "../ShaderEffect.hpp"

class ScanlinesEffect : public ShaderEffect {
   public:
    ScanlinesEffect();

    // Parameter accessors
    void setIntensity(float value);
    void setScanlineCount(float value);
    void setSpeed(float value);
    void setVignette(float value);
    void setRGBShift(float value);

    float getIntensity() const { return intensity; }
    float getScanlineCount() const { return scanlineCount; }
    float getSpeed() const { return speed; }
    float getVignette() const { return vignette; }
    float getRGBShift() const { return rgbShift; }

    // GUI integration
    ofParameterGroup & getParameterGroup() { return parameterGroup; }

   protected:
    void configureShaderUniforms(ofShader & shader, const glm::vec2 & resolution) override;

   private:
    float intensity;      // 0.0 - 1.0
    float scanlineCount;  // Lines per screen
    float speed;          // Animation speed
    float vignette;       // Edge darkening
    float rgbShift;       // Chromatic aberration

    // ofParameter integration for GUI
    ofParameterGroup parameterGroup;
    ofParameter<bool> enableParam;
    ofParameter<float> intensityParam;
    ofParameter<float> scanlineCountParam;
    ofParameter<float> speedParam;
    ofParameter<float> vignetteParam;
    ofParameter<float> rgbShiftParam;

    void onEnableChanged(bool & value);
    void onIntensityChanged(float & value);
    void onScanlineCountChanged(float & value);
    void onSpeedChanged(float & value);
    void onVignetteChanged(float & value);
    void onRGBShiftChanged(float & value);
};

#endif /* ScanlinesEffect_hpp */
