//
//  Sprite.hpp
//  orgb
//
//  Created by Kevin Katz on 9/29/21.
//

#ifndef Sprite_hpp
#define Sprite_hpp

#include <stdio.h>

#include <glm/glm.hpp>
#include "ofMain.h"

class BaseParticle {
   public:
    BaseParticle(glm::vec3 pos, glm::vec3 vel);
    virtual ~BaseParticle(){};

    glm::vec3 position;
    glm::vec3 velocity;

    void update(float dt, glm::vec3 acceleration = glm::vec3(0, 0, 0));
    void updateVelocity(glm::vec3 newVelocity);
    void updatePosition(glm::vec3 newPosition);
};

#endif /* Sprite_hpp */
