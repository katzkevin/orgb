//
//  StableThunder.hpp
//  orgb
//
//  Created by Kevin Katz on 3/2/22.
//

#ifndef StableThunder_hpp
#define StableThunder_hpp

#include "Press.hpp"
#include "Thunder.hpp"
#include "ofMain.h"

class RapidThunder : public Thunder {
   public:
    explicit RapidThunder(std::string name);
    ~RapidThunder() override {};

    ofParameter<float> boltHz;

   protected:
    LightningBolt getOrCreateBolt(const Press & p, float arousalGain, unsigned int randomSeed) override;
};

#endif /* StableThunder_hpp */
