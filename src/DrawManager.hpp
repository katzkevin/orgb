//
//  DrawManager.hpp
//  orgb
//
//  Created by Kevin Katz on 4/4/22.
//

#ifndef DrawManager_hpp
#define DrawManager_hpp

#include <stdio.h>

#include <boost/optional.hpp>

#include "DrawContext.hpp"
#include "ofMain.h"

#define GAUSSIAN_CENTER_PIXEL 0.382928
#define INVERSE_OF_GAUSSIAN_CENTER_PIXEL 1.0 / GAUSSIAN_CENTER_PIXEL

class DrawManager {
    // Global state machine to permit safer shader use across app.

   public:
    class ShaderPackage {
       public:
        ShaderPackage(std::string shaderName) { shader = createShader(shaderName); }
        ofShader shader;
    };
    boost::optional<ofFbo> activeCanvas;
    ofFbo fboFront;
    ofFbo fboBack;

    DrawManager();
    virtual ~DrawManager(){};

    void beginDraw();
    void endDraw();  // End drawing without drawing to screen
    void endAndDrawFbo();

    // FBO accessors for post-processing
    ofFbo& getFboFront() { return fboFront; }
    ofFbo& getFboBack() { return fboBack; }

    ShaderPackage shaderPackageBlurX;
    void shadeBlurX(float delta, float gain = 1.0);
    ShaderPackage shaderPackageBlurY;
    void shadeBlurY(float delta, float gain = 1.0);
    ShaderPackage shaderPackageGlowLine;
    void shadeGlowLine(ofVec3f from, ofVec3f to, ofColor c, float intensity, float glowDampenRadius, int blendMode,
                       bool toneMap);
    ShaderPackage shaderPackageGlowCircle;
    void shadeGlowCircle(ofVec3f center, float radius, ofColor c, float intensity, float glowDampenRadius,
                         int blendMode, bool toneMap);
    ShaderPackage shaderPackageGlowRectangularPrism;
    void shadeGlowRectangularPrism(ofVec3f a, ofVec3f b, float theta, ofColor c, float intensity,
                                   float glowDampenRadius, int blendMode, bool toneMap);

    //    ShaderPackage shaderPackageVHS;
    //    void shadeVHS(ofVec2f aberration, float aberrationOpacity);

    //    std::vector<ofParameter<bool>> shaderToggles;
    //
    //    ofParameter<float> blurOffsetStepPixels;
    //    ofParameter<float> aberrationOpacity;
    //    ofParameter<float> aberrationTremorDurationS;
    //    ofParameter<float> aberrationMinUnits;
    //    ofParameter<float> aberrationMaxUnits;

   private:
    void shaderEpilogue(ShaderPackage &sp);

    std::vector<ofShader> shaders;
    std::vector<std::function<void(ofShader &)>> shaderInitializations;
    std::vector<ofFbo> fbos;
};

#endif /* DrawManager_hpp */
