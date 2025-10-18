//
//  FilmGrainEffect.hpp
//  orgb
//
//  Film grain post-processing effect for organic texture
//

#ifndef FilmGrainEffect_hpp
#define FilmGrainEffect_hpp

#include "../ShaderEffect.hpp"

class FilmGrainEffect : public ShaderEffect {
   public:
    FilmGrainEffect();

    // Parameter accessors
    void setIntensity(float value);
    void setGrainSize(float value);

    float getIntensity() const { return intensity; }
    float getGrainSize() const { return grainSize; }

    // GUI integration
    ofParameterGroup& getParameterGroup() { return parameterGroup; }

   protected:
    void configureShaderUniforms(ofShader& shader, const glm::vec2& resolution) override;

   private:
    float intensity;   // 0.0 - 1.0
    float grainSize;   // 1.0 - 4.0

    // ofParameter integration for GUI
    ofParameterGroup parameterGroup;
    ofParameter<bool> enableParam;
    ofParameter<float> intensityParam;
    ofParameter<float> grainSizeParam;

    void onEnableChanged(bool& value);
    void onIntensityChanged(float& value);
    void onGrainSizeChanged(float& value);
};

#endif /* FilmGrainEffect_hpp */
