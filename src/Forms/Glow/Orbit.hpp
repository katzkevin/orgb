//
//  Orbit.hpp
//  orgb
//
//  Created by Kevin Katz on 5/4/22.
//

#ifndef Orbit_hpp
#define Orbit_hpp

#include <stdio.h>

#include "KeyState.hpp"
#include "BaseParticle.hpp"
#include "Press.hpp"
#include "Utilities.hpp"
#include "ColorProvider.hpp"
#include "VisualForm.hpp"

class Orbit : public VisualForm {
   public:
    Orbit(std::string name);
    virtual ~Orbit(){};

    void update(KeyState & ks, ColorProvider & clr);
    void draw(KeyState& ks, ColorProvider& clr, DrawManager& dm);

    std::unordered_map<int, BaseParticle> particles;

    float maxWaveWidth;

    ofParameter<float> glowIntensity;
    ofParameter<float> intensityAtEighthWidth;
    ofParameter<float> orbRadius;
    ofParameter<float> rate;
    ofParameter<int> blendMode;
    ofParameter<bool> toneMap;
};

#endif /* Orbit_hpp */
