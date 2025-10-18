//
//  Thunder.hpp
//  orgb
//
//  Created by Kevin Katz on 10/4/21.
//

#ifndef Thunder_hpp
#define Thunder_hpp

#include <stdio.h>
#include <random>

#include "KeyState.hpp"
#include "Press.hpp"
#include "VisualForm.hpp"
#include "LightningBolt.hpp"
#include "Utilities.hpp"
#include "ColorProvider.hpp"
#include "ofMain.h"

class Thunder : public VisualForm {
   protected:
    LightningBolt createBolt(const Press &p, float arousalGain, unsigned int randomSeed);
    virtual LightningBolt getOrCreateBolt(const Press &p, float arousalGain, unsigned int randomSeed);

    std::map<unsigned int, LightningBolt> bolts;
    
    ofParameter<int> recursionDepth;
    ofParameter<float> branchingFactor;
    ofParameter<float> jitterUnit;
    ofParameter<bool> lineSegmentDeterministic;

   public:
    Thunder(std::string _name);
    virtual ~Thunder();
    
    void draw(KeyState& ks, ColorProvider& clr, DrawManager& dm);
    void update(KeyState &ks, ColorProvider &clr);
};

#endif /* Thunder_hpp */
