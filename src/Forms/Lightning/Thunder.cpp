//
//  Thunder.cpp
//  orgb
//
//  Created by Kevin Katz on 10/4/21.
//

#include "Thunder.hpp"

#define THUNDER_SEED_PRIME 7759
#define THUNDER_BRANCH_PRIME 7789
// On a 32 length bolt, you'd expect max 1.
#define MAX_BRANCHES_PER_UNIT_LENGTH 1.0 / 32.0
#define THUNDER_DECAY_TIME_S 0.25

Thunder::Thunder(std::string _name) : VisualForm(_name) {
    int maxSide = std::max(ofGetWidth(), ofGetHeight());
    int defaultRecursionDepth = int(floor(std::log2(maxSide)));
    parameters.add(recursionDepth.set("recursionDepth", defaultRecursionDepth, 0, defaultRecursionDepth * 1.5));
    parameters.add(branchingFactor.set("branchingFactor", 0.105, 0, 1));
    parameters.add(jitterUnit.set("jitterUnit", 0.59, 0, 1.0));
    parameters.add(lineSegmentDeterministic.set("Line Segment Deterministic", false));
}

Thunder::~Thunder() { bolts.clear(); }

void Thunder::update(KeyState & ks, ColorProvider & clr) {
    // TODO What happens if we update with a press and never get around to deleting it
    for (const auto & p : ks.allPresses()) {
        double amplitude = p.audibleAmplitudePct(ks.attackTimeS, ks.decayTimeS, ks.sustainLevelPct, ks.releaseTimeS);
        if (ofIsFloatEqual(amplitude, 0.0) && bolts.count(p.id)) {
            bolts.erase(p.id);
        }
    }
    for (const auto & p : ks.allEphemeralPresses()) {
        if (ofIsFloatEqual(p.velocityPct, 0.0f) && bolts.count(p.id)) {
            bolts.erase(p.id);
        }
    }
}

void Thunder::draw(KeyState & ks, ColorProvider & clr, DrawManager & dm) {
    ofPushStyle();
    float arousalGain = ks.arousalGain();
    for (const auto & p : ks.allPresses()) {
        double amplitude = p.audibleAmplitudePct(ks.attackTimeS, ks.decayTimeS, ks.sustainLevelPct, ks.releaseTimeS);
        if (ofIsFloatEqual(amplitude, 0.0)) {
            // This press is already decayed, don't bother fetching or drawing.
            continue;
        }
        LightningBolt lightningBolt =
            getOrCreateBolt(p, arousalGain, lineSegmentDeterministic ? p.id : p.id + ks.randomSeed);
        ofColor color = clr.color(p, amplitude);
        lightningBolt.draw(color);
    }

    for (const auto & p : ks.allEphemeralPresses()) {
        LightningBolt lightningBolt =
            getOrCreateBolt(p, arousalGain, lineSegmentDeterministic ? p.id : p.id + ks.randomSeed);
        ofColor color = clr.color(p, p.velocityPct);
        lightningBolt.draw(color);
    };
    ofPopStyle();
    dm.shadeBlurX(3.5 * ks.valenceGain(),
                  INVERSE_OF_GAUSSIAN_CENTER_PIXEL);  // Blurrier at low arousal, sharper at high
    dm.shadeBlurY(3.5 * ks.valenceGain(),
                  INVERSE_OF_GAUSSIAN_CENTER_PIXEL);  // Blurrier at low arousal, sharper at high
}

LightningBolt Thunder::createBolt(const Press & p, float arousalGain, unsigned int seed) {
    std::pair<ofVec3f, ofVec3f> lineSegment = edgeToEdgeLineSegment(p, seed);
    ofVec3f from = lineSegment.first;
    ofVec3f to = lineSegment.second;

    LightningBolt computedBolt =
        LightningBolt(from, to, recursionDepth, jitterUnit * arousalGain, branchingFactor, seed % THUNDER_SEED_PRIME);
    return computedBolt;
}

LightningBolt Thunder::getOrCreateBolt(const Press & p, float arousalGain, unsigned int randomSeed) {
    try {
        return bolts.at(p.id);
    } catch (const std::out_of_range & oor) {
        LightningBolt b = createBolt(p, arousalGain, randomSeed);
        bolts.insert({p.id, b});
        return b;
    }
}
