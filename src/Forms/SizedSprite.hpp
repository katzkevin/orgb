//
//  FieldSprite.hpp
//  orgb
//
//  Created by Kevin Katz on 10/30/21.
//

#ifndef FieldSprite_hpp
#define FieldSprite_hpp

#include <glm/glm.hpp>

#include "Particle.h"
#include "ofMain.h"

class SizedSprite : public Particle {
   public:
    SizedSprite(glm::vec3 pos, glm::vec3 vel, ofColor c, float s);
    ~SizedSprite() override = default;

    unsigned int id;
    float size;

    void setSize(float s);
};

#endif /* FieldSprite_hpp */
