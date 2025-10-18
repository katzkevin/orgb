//
//  RandomParticles.hpp
//  orgb
//
//  Created by Kevin Katz on 5/27/22.
//

#ifndef RandomParticles_hpp
#define RandomParticles_hpp

#include <stdio.h>
#include "BaseParticles.hpp"
#include "ColorProvider.hpp"

class RandomParticles : public BaseParticles {
   public:
    RandomParticles(std::string name);
    virtual ~RandomParticles(){};
    
    ofParameter<float> baseRandom;
    
   protected:
    void createParticlesForPress(Press &press, int numberOfParticlesToCreate, ofColor c, float arousalPct) override;
    ofVec3f startPositionForPress(const Press &p) override;
    float opacityForEphemeralPress(KeyState & ks, const Press & p) const override;
    float opacityForPress(KeyState & ks, const Press & p) const override;
        
};

#endif /* RandomParticles_hpp */
