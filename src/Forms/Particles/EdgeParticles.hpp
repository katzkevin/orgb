//
//  EdgeParticles.hpp
//  orgb
//
//  Created by Kevin Katz on 5/27/22.
//

#ifndef EdgeParticles_hpp
#define EdgeParticles_hpp

#include "ColorProvider.hpp"
#include "GravityParticles.hpp"

class EdgeParticles : public GravityParticles {
   public:
    EdgeParticles(std::string name);
    ~EdgeParticles() override {};

   protected:
    ofVec3f startPositionForPress(const Press & p) override;
};

#endif /* EdgeParticles_hpp */
