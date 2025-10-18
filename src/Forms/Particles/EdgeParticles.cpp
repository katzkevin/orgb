//
//  EdgeParticles.cpp
//  orgb
//
//  Created by Kevin Katz on 5/27/22.
//

#include "EdgeParticles.hpp"

EdgeParticles::EdgeParticles(std::string name) : GravityParticles(name) {
    blurLowerLimit.set(0);
    blurUpperLimit.set(0);
    blurGain.set(1);
    initialVelocityLowerBound.set(6);
    angularVariance.set(0.56);
    baseGravity.set(820);
}

ofVec3f EdgeParticles::startPositionForPress(const Press &p) {
    int xPos = ofRandomWidth();
    // Subtly pertub to avoid initial streak
    int yPos = ofRandom(-2.0, 0.0);
    return ofVec3f(xPos, yPos, 0);
}

static void wallBounce(Particle &particle) {
    glm::vec3 pos = particle.position;
    glm::vec3 vel = particle.velocity;

    if (pos.x < 0) {
        particle.velocity = glm::vec3(-vel.x, vel.y, vel.z);
        int xOver = -pos.x;
        // This will be problematic at extremely high velocities (if xOver is greater than width?
        particle.position = glm::vec3(xOver, pos.y, pos.z);
    }
    // TODO Off by one? exactly getWidth
    else if (pos.x >= ofGetWidth()) {
        particle.velocity = glm::vec3(-vel.x, vel.y, vel.z);
        int xOver = pos.x - ofGetWidth();
        // This will be problematic at extremely high velocities (if xOver is greater than width?
        particle.position = glm::vec3(ofGetWidth() - xOver, pos.y, pos.z);
    }
}

