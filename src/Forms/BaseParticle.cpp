//
//  Sprite.cpp
//  orgb
//
//  Created by Kevin Katz on 9/29/21.
//

#include "BaseParticle.hpp"

#include "Utilities.hpp"

BaseParticle::BaseParticle(glm::vec3 pos, glm::vec3 vel) {
    position = pos;
    velocity = vel;
}

void BaseParticle::update(float dt, glm::vec3 acceleration) {
    velocity = velocity + acceleration * dt;
    position = position + velocity * dt;
}

void BaseParticle::updateVelocity(glm::vec3 newVelocity) { velocity = newVelocity; }

void BaseParticle::updatePosition(glm::vec3 newPosition) { position = newPosition; }
