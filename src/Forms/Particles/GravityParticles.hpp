//
//  particles.hpp
//  firstSketch
//
//  Created by Kevin Katz on 9/24/21.
//

#ifndef GravityParticles_hpp
#define GravityParticles_hpp

#include "BaseParticles.hpp"
#include "ColorProvider.hpp"

class GravityParticles : public BaseParticles {
   public:
    explicit GravityParticles(std::string name);
    ~GravityParticles() override = default;

    void update(KeyState & ks, ColorProvider & clr) override;

    ofParameter<float> baseGravity;
    ofParameter<float> topToBottomGravityRatio;
    ofParameter<float> angularVariance;

   protected:
    void createParticlesForPress(Press & press, int numberOfParticlesToCreate, ofColor c, float arousalPct) override;
    ofVec3f startPositionForPress(const Press & p) override;

    void pruneParticles() override;
};

#endif /* GravityParticles_hpp */
