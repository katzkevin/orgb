#include "Particle.h"

#include "ofMain.h"

Particle::Particle(ofVec3f pos, ofVec3f vel, ofColor c) : BaseParticle(pos, vel) { color = c; }

//------------------------------------------------------------

void Particle::draw(float drawRadius) {
    ofSetColor(this->color);
    ofDrawCircle(position, drawRadius);
}

void Particle::setColor(ofColor c) { color = c; }
