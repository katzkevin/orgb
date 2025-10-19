//
//  waves.hpp
//  firstSketch
//
//  Created by Kevin Katz on 9/22/21.
//

#ifndef BaseWaves_hpp
#define BaseWaves_hpp

#include <stdio.h>

#include "ColorProvider.hpp"
#include "KeyState.hpp"
#include "Press.hpp"
#include "Utilities.hpp"
#include "VisualForm.hpp"

class BaseWaves : public VisualForm {
   public:
    BaseWaves(std::string name);
    virtual ~BaseWaves() {};

    void update(KeyState & ks, ColorProvider & clr) override;

    void draw(KeyState & ks, ColorProvider & clr, DrawManager & dm) override;

    float maxWaveWidth;

    ofParameter<float> glowIntensity;
    ofParameter<float> intensityAtEighthWidth;
    ofParameter<int> blendMode;
    ofParameter<bool> toneMap;

   protected:
    virtual void drawUnit(ColorProvider & clr, KeyState & ks, DrawManager & dm, const Press & p, float alpha);
};

#endif /* BaseWaves_hpp */
