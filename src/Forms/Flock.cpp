//
//  Flock.cpp
//  orgb
//
//  Created by Kevin Katz on 2/18/22.
//

#include "Flock.hpp"

Flock::Flock() {}

void Flock::update(float dt) {
    float granularity = 60.0;
    float stepScale = granularity * dt * fastForward;  // This normalizes calculations at 60Hz, balancing them for FFW.

    const float squareVisualRange = pow(visualRange, 2);
    const float squareMinDistance = pow(minDistance, 2);
    float timeS = ofGetElapsedTimef();
    int index = 0;
    for (auto it = l.begin(); it != l.end(); ++it) {
        ofVec3f withinRangeCentroid = ofVec3f(0.0, 0.0, 0.0);
        ofVec3f withinRangeAverageVelocity = ofVec3f(0.0, 0.0, 0.0);
        int numWithinRange = 0;

        for (auto other = l.begin(); other != l.end(); ++other) {
            if (it == other) {
                continue;
            }
            float squareDistance = it->position.squareDistance(other->position);
            if (squareDistance <= squareVisualRange) {
                withinRangeCentroid += other->position;
                withinRangeAverageVelocity += other->velocity;
                ++numWithinRange;
            }
            if (squareDistance <= squareMinDistance) {
                ofVec3f vectorToOther = other->position - it->position;
                it->velocity += avoidFactor * stepScale * -vectorToOther;
            }
        }

        if (numWithinRange) {
            // Alignment
            withinRangeAverageVelocity /= numWithinRange;
            it->velocity += withinRangeAverageVelocity * alignmentFactor * stepScale;

            // Centering
            withinRangeCentroid /= numWithinRange;
            ofVec3f vectorToCentroid = withinRangeCentroid - it->position;
            it->velocity += vectorToCentroid * centeringFactor * stepScale;
        }

        //
        if (it->position.x < xMin + margin) {
            it->velocity += ofVec3f(turnFactor * stepScale, 0.0, 0.0);
        } else if (it->position.x > xMax - margin) {
            it->velocity -= ofVec3f(turnFactor * stepScale, 0.0, 0.0);
        }

        if (it->position.y < yMin + margin) {
            it->velocity += ofVec3f(0.0, turnFactor * stepScale, 0.0);
        } else if (it->position.y > yMax - margin) {
            it->velocity -= ofVec3f(0.0, turnFactor * stepScale, 0.0);
        }

        if (it->position.z < zMin + margin) {
            it->velocity += ofVec3f(0.0, 0.0, turnFactor * stepScale);
        } else if (it->position.z > zMax - margin) {
            it->velocity -= ofVec3f(0.0, 0.0, turnFactor * stepScale);
        }

        // it->velocity += steer;

        float v = it->velocity.length();

        if (v > maxVelocity) {
            // Strive for target velocity
            it->velocity *= maxVelocity / v;
        } else if (v < minVelocity) {
            it->velocity *= minVelocity / v;
        }

        it->update(dt * fastForward);

        ofVec3f noiseGradientAtCoordinate = noiseGradientForCoordinates(
            it->position.x, it->position.y, noiseSpatialFrequency, noiseTemporalRate, noiseScale, timeS);
        ofVec3f acceleration = ofVec3f(noiseGradientAtCoordinate.x, noiseGradientAtCoordinate.y, 0);
        it->update(dt * fastForward, acceleration);

        // it->update(1 / 60.0);
    }
}
