//
//  Sprite.hpp
//  orgb
//
//  Created by Kevin Katz on 9/29/21.
//

#ifndef Sprite_hpp
#define Sprite_hpp

#include <stdio.h>

#include "ofMain.h"

class BaseParticle {
   public:
    BaseParticle(ofVec3f pos, ofVec3f vel);
    virtual ~BaseParticle(){};

    ofVec3f position;
    ofVec3f velocity;

    void update(float dt, ofVec3f acceleration = ofVec3f(0, 0, 0));
    void updateVelocity(ofVec3f newVelocity);
    void updatePosition(ofVec3f newPosition);
};

#endif /* Sprite_hpp */
