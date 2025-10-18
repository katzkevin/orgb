//
//  RadialParticles.hpp
//  orgb
//
//  Created by Kevin Katz on 2/23/22.
//

#ifndef RadialParticles_hpp
#define RadialParticles_hpp

#include <stdio.h>
#include "BaseParticles.hpp"
#include "ColorProvider.hpp"

class RadialParticles : public BaseParticles {
public:
    RadialParticles(std::string name);
    virtual ~RadialParticles(){};
    
protected:
    void createParticlesForPress(Press &press, int numberOfParticlesToCreate, ofColor c, float arousalPct) override;
    ofVec3f startPositionForPress(const Press &p) override;
};

#endif /* RadialParticles_hpp */
