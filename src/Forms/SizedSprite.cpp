//
//  FieldSprite.cpp
//  orgb
//
//  Created by Kevin Katz on 10/30/21.
//

#include "SizedSprite.hpp"

SizedSprite::SizedSprite(ofVec3f pos, ofVec3f vel, ofColor c, float s) : Particle(pos, vel, c) { setSize(s); }

void SizedSprite::setSize(float s) { size = s; }
