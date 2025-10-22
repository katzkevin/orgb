//
//  Flock.cpp
//  orgb
//
//  Created by Kevin Katz on 2/18/22.
//

#include "Flock.hpp"

#include <math.h>

Flock::Flock() {}

void Flock::update(float dt, float elapsedTimeF) {
    float granularity = 60.0;
    float stepScale = granularity * dt * fastForward;  // This normalizes calculations at 60Hz, balancing them for FFW.

    const float squareVisualRange = pow(visualRange, 2);
    const float squareMinDistance = pow(minDistance, 2);

    for (auto it = l.begin(); it != l.end(); ++it) {
        glm::vec3 withinRangeCentroid = glm::vec3(0.0, 0.0, 0.0);
        glm::vec3 withinRangeAverageVelocity = glm::vec3(0.0, 0.0, 0.0);
        int numWithinRange = 0;

        for (auto other = l.begin(); other != l.end(); ++other) {
            if (it == other) {
                continue;
            }
            glm::vec3 diff = other->position - it->position;
            float squareDistance = glm::dot(diff, diff);
            if (squareDistance <= squareVisualRange) {
                withinRangeCentroid += other->position;
                withinRangeAverageVelocity += other->velocity;
                ++numWithinRange;
            }
            if (squareDistance <= squareMinDistance) {
                glm::vec3 vectorToOther = other->position - it->position;
                it->velocity += avoidFactor * stepScale * (-vectorToOther);
            }
        }

        if (numWithinRange) {
            // Alignment
            withinRangeAverageVelocity /= numWithinRange;
            it->velocity += withinRangeAverageVelocity * alignmentFactor * stepScale;

            // Centering
            withinRangeCentroid /= static_cast<float>(numWithinRange);
            glm::vec3 vectorToCentroid = withinRangeCentroid - it->position;
            it->velocity += vectorToCentroid * centeringFactor * stepScale;
        }

        //
        if (it->position.x < xMin + margin) {
            it->velocity += glm::vec3(turnFactor * stepScale, 0.0, 0.0);
        } else if (it->position.x > xMax - margin) {
            it->velocity -= glm::vec3(turnFactor * stepScale, 0.0, 0.0);
        }

        if (it->position.y < yMin + margin) {
            it->velocity += glm::vec3(0.0, turnFactor * stepScale, 0.0);
        } else if (it->position.y > yMax - margin) {
            it->velocity -= glm::vec3(0.0, turnFactor * stepScale, 0.0);
        }

        if (it->position.z < zMin + margin) {
            it->velocity += glm::vec3(0.0, 0.0, turnFactor * stepScale);
        } else if (it->position.z > zMax - margin) {
            it->velocity -= glm::vec3(0.0, 0.0, turnFactor * stepScale);
        }

        // it->velocity += steer;

        float v = glm::length(it->velocity);

        if (v > maxVelocity) {
            // Strive for target velocity
            it->velocity *= maxVelocity / v;
        } else if (v < minVelocity) {
            it->velocity *= minVelocity / v;
        }

        it->update(dt * fastForward);
    }
}
