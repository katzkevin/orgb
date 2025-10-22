//
//  Thunder.hpp
//  orgb
//
//  Created by Kevin Katz on 10/4/21.
//

#ifndef Thunder_hpp
#define Thunder_hpp

#include <random>

#include "ColorProvider.hpp"
#include "KeyState.hpp"
#include "LightningBolt.hpp"
#include "Press.hpp"
#include "Utilities.hpp"
#include "VisualForm.hpp"
#include "ofMain.h"

class Thunder : public VisualForm {
   protected:
    LightningBolt createBolt(const Press & p, float arousalGain, unsigned int randomSeed);
    virtual LightningBolt getOrCreateBolt(const Press & p, float arousalGain, unsigned int randomSeed);

    std::map<unsigned int, LightningBolt> bolts;

    ofParameter<int> recursionDepth;
    ofParameter<float> branchingFactor;
    ofParameter<float> jitterUnit;
    ofParameter<bool> lineSegmentDeterministic;

   public:
    explicit Thunder(std::string name);
    ~Thunder() override;

    void draw(KeyState & ks, ColorProvider & clr, DrawManager & dm) override;
    void update(KeyState & ks, ColorProvider & clr) override;
};

#endif /* Thunder_hpp */
