//
//  DigitalGlitchEffect.hpp
//  orgb
//
//  Digital glitch effect with block displacement and color separation
//

#ifndef DigitalGlitchEffect_hpp
#define DigitalGlitchEffect_hpp

#include "../ShaderEffect.hpp"

class DigitalGlitchEffect : public ShaderEffect {
   public:
    DigitalGlitchEffect();

    // Parameter accessors
    void setIntensity(float value);
    void setBlockSize(float value);
    void setColorShift(float value);
    void setNoiseAmount(float value);

    float getIntensity() const { return intensity; }
    float getBlockSize() const { return blockSize; }
    float getColorShift() const { return colorShift; }
    float getNoiseAmount() const { return noiseAmount; }

    // GUI integration
    ofParameterGroup & getParameterGroup() { return parameterGroup; }

   protected:
    void configureShaderUniforms(ofShader & shader, const glm::vec2 & resolution) override;

   private:
    float intensity;    // 0.0 - 1.0
    float blockSize;    // 8.0 - 64.0
    float colorShift;   // 0.0 - 20.0
    float noiseAmount;  // 0.0 - 1.0

    // ofParameter integration for GUI
    ofParameterGroup parameterGroup;
    ofParameter<bool> enableParam;
    ofParameter<float> intensityParam;
    ofParameter<float> blockSizeParam;
    ofParameter<float> colorShiftParam;
    ofParameter<float> noiseAmountParam;

    void onEnableChanged(bool & value);
    void onIntensityChanged(float & value);
    void onBlockSizeChanged(float & value);
    void onColorShiftChanged(float & value);
    void onNoiseAmountChanged(float & value);
};

#endif /* DigitalGlitchEffect_hpp */
