//
//  LineSegmentGlow.hpp
//  orgb
//
//  Created by Kevin Katz on 4/29/22.
//

#ifndef LineSegmentGlow_hpp
#define LineSegmentGlow_hpp

#include "ColorProvider.hpp"
#include "KeyState.hpp"
#include "Press.hpp"
#include "Utilities.hpp"
#include "VisualForm.hpp"

class GlowLinePlayground : public VisualForm {
   public:
    explicit GlowLinePlayground(std::string name);
    ~GlowLinePlayground() override {};
    void draw(KeyState & ks, ColorProvider & clr, DrawManager & dm);

   protected:
    ofParameter<float> glowIntensity;
    ofParameter<float> intensityAtEighthWidth;
    ofParameter<int> blendMode;
    ofParameter<bool> toneMap;

    ofParameter<float> aAlpha;
    ofParameter<float> slope;
    ofParameter<float> length;
    ofParameter<float> velocity;
};

#endif /* LineSegmentGlow_hpp */
