#include "Particle.h"

#include "ofMain.h"

Particle::Particle(glm::vec3 pos, glm::vec3 vel, ofColor c) : BaseParticle(pos, vel) { color = c; }

//------------------------------------------------------------

void Particle::draw(float drawRadius) {
    ofSetColor(this->color);
    ofDrawCircle(position, drawRadius);
}

void Particle::setColor(ofColor c) { color = c; }
