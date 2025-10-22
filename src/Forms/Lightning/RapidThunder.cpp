//
//  StableThunder.cpp
//  orgb
//
//  Created by Kevin Katz on 3/2/22.
//

#include "RapidThunder.hpp"

#include <math.h>

RapidThunder::RapidThunder(const std::string & name) : Thunder(name) {
    parameters.add(boltHz.set("boltHz", 10, 4, 60));
}

LightningBolt RapidThunder::getOrCreateBolt(const Press & p, float arousalGain, unsigned int randomSeed) {
    // Slower with low arousal, faster with high arousal
    float effectiveInterBoltTimeS = (1 / boltHz) * ofMap(arousalGain, 0, 1, 2.0, 0.5);
    try {
        LightningBolt storedBolt = bolts.at(p.id);
        if (getSystemTimeSecondsPrecise() - storedBolt.tCreatedSeconds < effectiveInterBoltTimeS) {
            return storedBolt;
        } else {
            bolts.erase(p.id);  // If found but stale, delete then recreate after catch.
        }
    } catch (const std::out_of_range & oor) {
        // If not found, proceed to create
    }
    LightningBolt b = createBolt(p, arousalGain, randomSeed);
    bolts.insert({p.id, b});
    return b;
}
