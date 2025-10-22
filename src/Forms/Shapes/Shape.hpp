//
//  Shape.hpp
//  orgb
//
//  Created by Kevin Katz on 10/4/21.
//

#ifndef Shape_hpp
#define Shape_hpp

#include <random>

#include "ColorProvider.hpp"
#include "KeyState.hpp"
#include "Press.hpp"
#include "Utilities.hpp"
#include "VisualForm.hpp"
#include "ofMain.h"

class Shape : public VisualForm {
   private:
    std::map<Press, ofPath> shapes;
    ofPath getOrCreatePath(Press & p);

   public:
    explicit Shape(const std::string & name);
    ~Shape() override {};

    void draw(KeyState & ks, ColorProvider & clr, DrawManager & dm) override;
    ofPath shape(int sideCount, float radius);
    float calculateRadius(Press & p);

    virtual void drawUnit(const ofColor & color, KeyState & ks, DrawManager & dm, Press & press);

    ofParameter<int> drawMode;
    ofParameter<float> blurOffset;
    ofParameter<float> blurGain;
    ofParameter<float> glowIntensity;
    ofParameter<float> intensityAtEighthWidth;
    ofParameter<int> blendMode;
    ofParameter<bool> toneMap;
};

#endif /* Shape_hpp */
