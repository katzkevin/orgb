//
//  BaseParticles.hpp
//  orgb
//
//  Created by Kevin Katz on 2/23/22.
//

#ifndef BaseParticles_hpp
#define BaseParticles_hpp

#include <stdio.h>

#include "ColorProvider.hpp"
#include "KeyState.hpp"
#include "Particle.h"
#include "Press.hpp"
#include "Utilities.hpp"
#include "VisualForm.hpp"

class BaseParticles : public VisualForm {
   public:
    BaseParticles(std::string name);
    virtual ~BaseParticles() {};

    void update(KeyState & ks, ColorProvider & clr);
    void draw(KeyState & ks, ColorProvider & clr, DrawManager & dm);

   protected:
    ofParameter<float> particleRate;
    ofParameter<float> maxParticles;
    ofParameter<float> initialVelocityLowerBound;
    ofParameter<float> topToBottomInitialVelocityRatio;

    ofParameter<float> blurLowerLimit;
    ofParameter<float> blurUpperLimit;
    ofParameter<float> blurGain;

    ofParameter<float> noiseTemporalRate;
    ofParameter<float> noiseSpatialFrequency;
    ofParameter<float> noiseScale;
    ofParameter<bool> noiseVisualize;

    // Old:
    // WARNING: Do not treat particles' keys as up to date. They're saved at the moment they're inserted, so they never
    // see, for example, the invocation of a t_released. This is why shapes' release ADSR worked but particles' did not.
    // New: Use press id.
    std::unordered_multimap<unsigned int, Particle> particles;

    void renderPixelsForPress(ColorProvider & clr, KeyState & ks, const Press & p);

    virtual void createParticlesForPress(Press & press, int numberOfParticlesToCreate, ofColor c, float arousalPct) = 0;
    virtual ofVec3f startPositionForPress(const Press & p);
    virtual float opacityForEphemeralPress(KeyState & ks, const Press & p) const;
    virtual float opacityForPress(KeyState & ks, const Press & p) const;

    virtual void pruneParticles();

    ofImage canvas;
};

#endif /* BaseParticles_hpp */
