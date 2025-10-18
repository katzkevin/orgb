//
//  particles.cpp
//  firstSketch
//
//  Created by Kevin Katz on 9/24/21.
//

#include "GravityParticles.hpp"



GravityParticles::GravityParticles(std::string name) : BaseParticles(name) {
    particleRate.set(2000);
    initialVelocityLowerBound.set(200);
    blurLowerLimit.set(1);
    blurUpperLimit.set(2);
    noiseTemporalRate.set(1.4);
    noiseScale.set(8.667);
    parameters.add(baseGravity.set("baseGravity", 4000, 300, 4000));
    parameters.add(topToBottomGravityRatio.set("topToBottomGravityRatio", 4, 1, 10));
    parameters.add(angularVariance.set("angularVariance", 0.84, 0, PI/2));
}

ofVec3f GravityParticles::startPositionForPress(const Press &p) {
    int xPos = ofRandomWidth();
    // Subtly pertub to avoid initial streak
    int yPos = ofRandom(0.0, 2.0);
    return ofVec3f(xPos, yPos, 0);
}

static void wallBounce(Particle &particle) {
    ofVec2f pos = particle.position;
    ofVec2f vel = particle.velocity;

    if (pos.x < 0) {
        particle.velocity = ofVec2f(-vel.x, vel.y);
        int xOver = -pos.x;
        // This will be problematic at extremely high velocities (if xOver is greater than width?
        particle.position = ofVec2f(xOver, pos.y);
    }
    // TODO Off by one? exactly getWidth
    else if (pos.x >= ofGetWidth()) {
        particle.velocity = ofVec2f(-vel.x, vel.y);
        int xOver = pos.x - ofGetWidth();
        // This will be problematic at extremely high velocities (if xOver is greater than width?
        particle.position = ofVec2f(ofGetWidth() - xOver, pos.y);
    }
}


void GravityParticles::update(KeyState & ks, ColorProvider & clr) {
    for (auto it = particles.begin(); it != particles.end(); ++it) {
        it->second.updateVelocity(it->second.velocity + ofVec3f(0, baseGravity, 0) * ofGetLastFrameTime());
        it->second.update(ofGetLastFrameTime());
        // Correct for beyond borders
        wallBounce(it->second);
    }
    BaseParticles::update(ks, clr);
}

void GravityParticles::createParticlesForPress(Press & press, int numberOfParticlesToCreate, ofColor c, float arousalPct) {
    // int xPos = ofMap(kv.second.note % NUM_NOTES, 0, NUM_NOTES, 0, ofGetWidth());
    float arousalModifier = ofMap(arousalPct, 0, 1, 0.5, 2.0);
    for (int i = 0; i < numberOfParticlesToCreate; i++) {
        // Perturb +/- 0.5 to accommodate for the more keys than particles in a frame, situation
        float angle = ofMap(ofRandomf(), -1, 1, -angularVariance, angularVariance) + PI / 2; // + PI/2 because it's centered about PI/2 (down)
        float velocity = initialVelocityLowerBound * arousalModifier;
        float vx;
        if ((PI / 2.0) - 0.001 < angle && angle < (PI / 2.0) + 0.001) {
            vx = 0;
        } else {
            vx = cos(angle) * velocity;
        }
        // Subtly perturb to avoid streaks
        
        float vy = sin(angle) * velocity * ofRandom(0.05,20);
        ofVec3f startPosition = startPositionForPress(press);
        Particle myParticle(startPosition, ofVec3f(vx, vy, 0), c);
        particles.emplace(press.id, myParticle);
    }
}


void GravityParticles::pruneParticles() {
    // https://stackoverflow.com/a/800984
    
    for (auto iter = particles.begin(); iter != particles.end();) {
        if (iter->second.position.y > ofGetHeight() || iter->second.color.a < 0.0001) {
            iter = particles.erase(iter);
        } else {
            ++iter;
        }
    }
}
