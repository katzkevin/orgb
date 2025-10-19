//
//  Field.hpp
//  orgb
//
//  Created by Kevin Katz on 9/29/21.
//

#ifndef Field_hpp
#define Field_hpp

#include <stdio.h>

#include "BaseWaves.hpp"
#include "Field.hpp"
#include "Flock.hpp"
#include "KeyState.hpp"
#include "SizedSprite.hpp"
#include "Utilities.hpp"
#include "VisualForm.hpp"
#include "ofMain.h"
#include "ofxGui.h"

class Field : public VisualForm {
   public:
    float width;
    float height;
    float depth{};

    Flock flock;

    Field(std::string name);
    virtual ~Field() = default;
    void setup() override;

    void update(KeyState & ks, ColorProvider & clr) override;
    void draw(KeyState & ks, ColorProvider & clr, DrawManager & dm) override;
    void newPressHandler(ColorProvider & clr, Press & p) override;

    void adjustFlockPopulation();
    static void rotateFlockHueOverTime();
    static ofVec3f steerAccordingToKeyPresses(KeyState & ks);

    static float generateNoisySpriteSize(float salt);
    static ofColor generateNoisySpriteColor(float salt);

    void backgroundChanged(ofColor & c);
    void spriteColorChanged(ofColor & c);
    void spriteColorChangedFloatParam(float & f);
    void spriteSizeChanged(float & s);
    void spriteVariationChanged(float & v);

    void seedSingleSprite(float salt);

    ofParameter<int> drawStyle;
    ofParameter<int> population;
    ofParameter<float> fastForward;
    ofParameter<float> minVelocity;
    ofParameter<float> maxVelocity;
    ofParameter<float> visualRange;
    ofParameter<float> minDistance;
    ofParameter<float> centeringFactor;
    ofParameter<float> alignmentFactor;
    ofParameter<float> avoidFactor;
    ofParameter<float> turnFactor;
    ofParameter<float> margin;
    ofParameter<float> tendTowardsCenterFactor;
    ofParameter<float> spriteBaseSize;
    ofParameter<float> spriteVariation;

    ofParameter<ofColor> spriteBaseColor;
    //    ofParameter<ofColor> background;

    ofParameter<float> spriteBaseAlphaPct;
    ofParameter<float> spriteHueNoiseMaximumPct;
    ofParameter<float> spriteSaturationNoiseMaximumPct;
    ofParameter<float> spriteValueNoiseMaximumPct;

    ofParameter<float> glowIntensity;
    ofParameter<float> intensityAtEighthWidth;
    ofParameter<bool> toneMap;
    ofParameter<int> blendMode;

    ofParameter<float> noiseTemporalRate;
    ofParameter<float> noiseSpatialFrequency;
    ofParameter<float> noiseScale;
    ofParameter<bool> noiseVisualize;

   protected:
    void translateField() const;
};

#endif /* Field_hpp */
