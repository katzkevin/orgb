#ifndef PARTICLE_H
#define PARTICLE_H

#include "BaseParticle.hpp"
#include "ofMain.h"

class Particle : public BaseParticle {
   public:
    Particle(ofVec3f pos, ofVec3f vel, ofColor c);
    virtual ~Particle(){};

    void setColor(ofColor c);
    void draw(float drawRadius);

    ofColor color;
};

#endif  // PARTICLE_H
