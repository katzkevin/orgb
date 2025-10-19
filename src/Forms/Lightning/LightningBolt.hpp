//
//  LightningBolt.hpp
//  orgb
//
//  Created by Kevin Katz on 2/27/22.
//

#ifndef LightningBolt_hpp
#define LightningBolt_hpp

#include <stdio.h>

#include "Utilities.hpp"

class LightningBolt {
   public:
    LightningBolt(ofVec3f from, ofVec3f to, int depth, float jitterUnit, float branchingFactor, float seed);
    virtual ~LightningBolt() = default;

    static void draw(ofColor color);

    double tCreatedSeconds;

   private:
    void singleLightningBoltR(vector<ofPoint> & ls, int lo, int hi, float jitterUnit, float branchingFactor,
                              float seed);
    vector<ofPoint> singleLightningBolt(ofPoint from, ofPoint to, int ttl, float jitterUnit, float branchingFactor,
                                        float seed);

    vector<ofPoint> trunk{};
    std::list<LightningBolt> branches {}
    ;
};

#endif /* LightningBolt_hpp */
