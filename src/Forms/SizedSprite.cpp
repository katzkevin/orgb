//
//  FieldSprite.cpp
//  orgb
//
//  Created by Kevin Katz on 10/30/21.
//

#include "SizedSprite.hpp"

SizedSprite::SizedSprite(glm::vec3 pos, glm::vec3 vel, ofColor c, float s) : Particle(pos, vel, c) { setSize(s); }

void SizedSprite::setSize(float s) { size = s; }
