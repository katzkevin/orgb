//
//  Sprite.cpp
//  orgb
//
//  Created by Kevin Katz on 9/29/21.
//

#include "BaseParticle.hpp"

#include "Utilities.hpp"

BaseParticle::BaseParticle(ofVec3f pos, ofVec3f vel) {
    position = pos;
    velocity = vel;
}

void BaseParticle::update(float dt, ofVec3f acceleration) {
    velocity = velocity + acceleration * dt;
    position = position + velocity * dt;
}

void BaseParticle::updateVelocity(ofVec3f newVelocity) { velocity = newVelocity; }

void BaseParticle::updatePosition(ofVec3f newPosition) { position = newPosition; }
