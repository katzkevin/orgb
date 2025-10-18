//
//  LightningBolt.cpp
//  orgb
//
//  Created by Kevin Katz on 2/27/22.
//

#include "LightningBolt.hpp"

LightningBolt::LightningBolt(ofVec3f from, ofVec3f to, int depth, float jitterUnit, float branchingFactor, float seed) {
    tCreatedSeconds = getSystemTimeSecondsPrecise();
    std::list<vector<ofPoint>> branchQueue;
    trunk = singleLightningBolt(from, to, depth, jitterUnit, branchingFactor, seed);
}

void LightningBolt::singleLightningBoltR(vector<ofPoint>& ls, int lo, int hi, float jitterUnit, float branchingFactor, float seed) {
    if (hi - lo <= 1) {
        return;
    }
    int loOffset = floor((hi - lo) / 2.0);
    int target = lo + loOffset;

    ofPoint loVector = ls.at(lo);
    ofPoint hiVector = ls.at(hi);
    ofPoint delta = hiVector - loVector;
    ofPoint halfDelta = delta * 0.5;
    
    float jitterRotation = ofRandom(jitterUnit * PI / 8, jitterUnit * PI / 4);
    // ofPoint perturbed = rotated * perturbation / ofMap(ofGetMouseX(), 0, ofGetScreenWidth(), 0, 5.0);
    // ls[target] = loVector + getRandomlyRotatedVectorRad(halfDelta, jitterRotation);;
    ofVec3f perturbation = getRandomlyRotatedVectorRad(halfDelta, jitterRotation);
    ls[target] = loVector + perturbation;
    
    singleLightningBoltR(ls, lo, target, jitterUnit, branchingFactor, seed * jitterRotation);
    singleLightningBoltR(ls, target, hi, jitterUnit, branchingFactor, seed / jitterRotation);
    
    // We express that higher in the chain is more likely to branch (because the ancestor bolt would be more likely to ionize air)
    // float branchLikelihood = exponentialMap(hi - lo, 0, ls.size(), 0, branchingFactor, true, 2);
    
    if (ofRandomuf() < branchingFactor) {
        float ttl = std::floor(std::log2(hi - lo));
        // Multiplying delta by 1.25 makes the branches a bit more aggressive
        branches.push_back(LightningBolt(hiVector, hiVector + delta * 1.25, ttl, jitterUnit, branchingFactor, ofRandomuf()));
    }
    
}

vector<ofPoint> LightningBolt::singleLightningBolt(ofPoint from, ofPoint to, int depth, float jitterUnit, float branchingFactor, float seed) {
    // BranchingFactor := likelihood of branch per joint
    vector<ofPoint> ls(pow(2, depth) + 1, ofPoint(0, 0, 0));
    ls[0] = from;
    ls[ls.size() - 1] = to;

    singleLightningBoltR(ls, 0, static_cast<int>(ls.size()) - 1, jitterUnit, branchingFactor, seed);

    return ls;
}

void LightningBolt::draw(ofColor color) {
    ofMesh mesh;
    for (unsigned int i = 0; i < trunk.size(); ++i) {
        mesh.addVertex(trunk[i]);
        mesh.addColor(color);
        mesh.addIndex(i);
    }
    mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
    mesh.draw();
    for (auto & branch: branches) {
        branch.draw(color);
    }
}
