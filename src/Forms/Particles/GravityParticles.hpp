//
//  particles.hpp
//  firstSketch
//
//  Created by Kevin Katz on 9/24/21.
//

#ifndef GravityParticles_hpp
#define GravityParticles_hpp

#include <stdio.h>
#include "BaseParticles.hpp"
#include "ColorProvider.hpp"

class GravityParticles : public BaseParticles {
   public:
    GravityParticles(std::string name);
    virtual ~GravityParticles(){};

    void update(KeyState & ks, ColorProvider & clr) override;
    
    ofParameter<float> baseGravity;
    ofParameter<float> topToBottomGravityRatio;
    ofParameter<float> angularVariance;

   protected:
    void createParticlesForPress(Press &press, int numberOfParticlesToCreate, ofColor c, float arousalPct) override;
    virtual ofVec3f startPositionForPress(const Press &p) override;
    
    void pruneParticles() override;
        
};

#endif /* GravityParticles_hpp */
