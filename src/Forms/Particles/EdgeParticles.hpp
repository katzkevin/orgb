//
//  EdgeParticles.hpp
//  orgb
//
//  Created by Kevin Katz on 5/27/22.
//

#ifndef EdgeParticles_hpp
#define EdgeParticles_hpp

#include <stdio.h>
#include "GravityParticles.hpp"
#include "ColorProvider.hpp"

class EdgeParticles : public GravityParticles {
   public:
    EdgeParticles(std::string name);
    virtual ~EdgeParticles(){};
    

   protected:
    ofVec3f startPositionForPress(const Press &p) override;
};

#endif /* EdgeParticles_hpp */
