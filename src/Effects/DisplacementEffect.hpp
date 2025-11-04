//
//  DisplacementEffect.hpp
//  orgb
//
//  Glitch-style displacement post-processing effect
//

#ifndef DisplacementEffect_hpp
#define DisplacementEffect_hpp

#include "../ShaderEffect.hpp"

class DisplacementEffect : public ShaderEffect {
   public:
    DisplacementEffect();

    // Parameter accessors
    void setStrength(float value);
    void setFrequency(float value);
    void setBlockSize(float value);

    float getStrength() const { return strength; }
    float getFrequency() const { return frequency; }
    float getBlockSize() const { return blockSize; }

    // GUI integration
    ofParameterGroup & getParameterGroup() { return parameterGroup; }

   protected:
    void configureShaderUniforms(ofShader & shader, const glm::vec2 & resolution) override;

   private:
    float strength;   // 0.0 - 1.0
    float frequency;  // 0.1 - 10.0
    float blockSize;  // 1.0 - 50.0

    // ofParameter integration for GUI
    ofParameterGroup parameterGroup;
    ofParameter<bool> enableParam;
    ofParameter<float> strengthParam;
    ofParameter<float> frequencyParam;
    ofParameter<float> blockSizeParam;

    void onEnableChanged(bool & value);
    void onStrengthChanged(float & value);
    void onFrequencyChanged(float & value);
    void onBlockSizeChanged(float & value);
};

#endif /* DisplacementEffect_hpp */
