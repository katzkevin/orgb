//
//  FieldSprite.hpp
//  orgb
//
//  Created by Kevin Katz on 10/30/21.
//

#ifndef FieldSprite_hpp
#define FieldSprite_hpp

#include <stdio.h>

#include "Particle.h"
#include "ofMain.h"

class SizedSprite : public Particle {
   public:
    SizedSprite(ofVec3f pos, ofVec3f vel, ofColor c, float s);
    virtual ~SizedSprite(){};

    unsigned int id;
    float size;

    void setSize(float s);
};

#endif /* FieldSprite_hpp */
