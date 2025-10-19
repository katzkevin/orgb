#ifndef PARTICLE_H
#define PARTICLE_H

#include <glm/glm.hpp>

#include "BaseParticle.hpp"
#include "ofMain.h"

class Particle : public BaseParticle {
   public:
    Particle(glm::vec3 pos, glm::vec3 vel, ofColor c);
    ~Particle() override {};

    void setColor(ofColor c);
    void draw(float drawRadius);

    ofColor color;
};

#endif  // PARTICLE_H
