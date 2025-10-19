//
//  Flock.hpp
//  orgb
//
//  Created by Kevin Katz on 2/18/22.
//

#ifndef Flock_hpp
#define Flock_hpp

#include <glm/glm.hpp>

#include "SizedSprite.hpp"
#include "Utilities.hpp"
#include "ofMain.h"

class Flock {
   public:
    Flock();
    ~Flock() = default;

    void update(float dt, float elapsedTimeF);

    std::vector<SizedSprite> l;

    float fastForward;
    float visualRange;
    float minDistance;
    float avoidFactor;
    float alignmentFactor;
    float centeringFactor;
    float turnFactor;
    float minVelocity;
    float maxVelocity;
    float margin;
    float xMin;
    float xMax;
    float yMin;
    float yMax;
    float zMin;
    float zMax;
};

#endif /* Flock_hpp */
