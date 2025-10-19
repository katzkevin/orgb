//
//  VHSGlitchEffect.hpp
//  orgb
//
//  Analog VHS glitch with line displacement, color bleeding, and tracking errors
//

#ifndef VHSGlitchEffect_hpp
#define VHSGlitchEffect_hpp

#include "../ShaderEffect.hpp"

class VHSGlitchEffect : public ShaderEffect {
   public:
    VHSGlitchEffect();

    // Parameter accessors
    void setIntensity(float value);
    void setLineDisplacement(float value);   // horizontal line shifts
    void setColorBleed(float value);         // color smearing/bleeding
    void setSignalNoise(float value);        // bad signal corruption
    void setTrackingError(float value);      // VHS tracking issues

    float getIntensity() const { return intensity; }
    float getLineDisplacement() const { return lineDisplacement; }
    float getColorBleed() const { return colorBleed; }
    float getSignalNoise() const { return signalNoise; }
    float getTrackingError() const { return trackingError; }

    // GUI integration
    ofParameterGroup & getParameterGroup() { return parameterGroup; }

   protected:
    void configureShaderUniforms(ofShader & shader, const glm::vec2 & resolution) override;

   private:
    float intensity;          // 0.0 - 1.0 overall effect strength
    float lineDisplacement;   // 0.0 - 100.0 pixels
    float colorBleed;         // 0.0 - 1.0
    float signalNoise;        // 0.0 - 1.0
    float trackingError;      // 0.0 - 1.0

    // ofParameter integration for GUI
    ofParameterGroup parameterGroup;
    ofParameter<bool> enableParam;
    ofParameter<float> intensityParam;
    ofParameter<float> lineDisplacementParam;
    ofParameter<float> colorBleedParam;
    ofParameter<float> signalNoiseParam;
    ofParameter<float> trackingErrorParam;

    void onEnableChanged(bool & value);
    void onIntensityChanged(float & value);
    void onLineDisplacementChanged(float & value);
    void onColorBleedChanged(float & value);
    void onSignalNoiseChanged(float & value);
    void onTrackingErrorChanged(float & value);
};

#endif /* VHSGlitchEffect_hpp */
