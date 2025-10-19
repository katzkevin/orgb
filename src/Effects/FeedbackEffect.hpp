//
//  FeedbackEffect.hpp
//  orgb
//
//  Feedback/echo effect - blends previous frames with displacement
//

#ifndef FeedbackEffect_hpp
#define FeedbackEffect_hpp

#include "../ShaderEffect.hpp"

class FeedbackEffect : public ShaderEffect {
   public:
    FeedbackEffect();

    // Parameter accessors
    void setIntensity(float value);          // blend amount with previous frame
    void setDisplacementX(float value);      // horizontal shift per iteration
    void setDisplacementY(float value);      // vertical shift per iteration
    void setScale(float value);              // scale change per iteration
    void setRotation(float value);           // rotation per iteration (degrees)
    void setFeedbackColor(float value);      // color shift in feedback

    float getIntensity() const { return intensity; }
    float getDisplacementX() const { return displacementX; }
    float getDisplacementY() const { return displacementY; }
    float getScale() const { return scale; }
    float getRotation() const { return rotation; }
    float getFeedbackColor() const { return feedbackColor; }

    // GUI integration
    ofParameterGroup & getParameterGroup() { return parameterGroup; }

   protected:
    void configureShaderUniforms(ofShader & shader, const glm::vec2 & resolution) override;

   private:
    float intensity;         // 0.0 - 1.0
    float displacementX;     // -10.0 - 10.0 pixels
    float displacementY;     // -10.0 - 10.0 pixels
    float scale;             // 0.95 - 1.05
    float rotation;          // -5.0 - 5.0 degrees
    float feedbackColor;     // 0.0 - 1.0

    // ofParameter integration for GUI
    ofParameterGroup parameterGroup;
    ofParameter<bool> enableParam;
    ofParameter<float> intensityParam;
    ofParameter<float> displacementXParam;
    ofParameter<float> displacementYParam;
    ofParameter<float> scaleParam;
    ofParameter<float> rotationParam;
    ofParameter<float> feedbackColorParam;

    void onEnableChanged(bool & value);
    void onIntensityChanged(float & value);
    void onDisplacementXChanged(float & value);
    void onDisplacementYChanged(float & value);
    void onScaleChanged(float & value);
    void onRotationChanged(float & value);
    void onFeedbackColorChanged(float & value);
};

#endif /* FeedbackEffect_hpp */
