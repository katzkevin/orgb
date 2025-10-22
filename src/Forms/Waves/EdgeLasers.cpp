//
//  PointWaves.cpp
//  orgb
//
//  Created by Kevin Katz on 5/4/22.
//

#include "EdgeLasers.hpp"

#include <math.h>

#define SHAPE_PRIME 7757
#define MAX_DEPTH_FRACTION 64

EdgeLasers::EdgeLasers(std::string name) : BaseWaves(name) {
    glowIntensity.set(1.2);
    toneMap.set(false);
    // intensityAtEighthWidth.set(0.);
    parameters.add(lineSegmentDeterministic.set("Line Segment Deterministic", false));
}

void EdgeLasers::drawUnit(ColorProvider & clr, KeyState & ks, DrawManager & dm, const Press & p, float alpha) {
    int index = p.note % NUM_NOTES;
    float halfOfTheWidthOfNotesZone = (ofGetWidth() / NUM_NOTES) / 2.0;

    // Deterministic per display. Random seed differs per orgb process.
    std::pair<ofVec3f, ofVec3f> lineSegment =
        edgeToEdgeLineSegment(p, lineSegmentDeterministic ? p.id : (p.id + ks.randomSeed % SHAPE_PRIME));
    ofVec3f from = lineSegment.first;
    ofVec3f to = lineSegment.second;

    float computedDampenRadius =
        getGlowDampenRatio(glowIntensity, intensityAtEighthWidth, std::min(ofGetWidth(), ofGetHeight()) / 8.0);
    dm.shadeGlowLine(to + (from - to) * 2, from + (to - from) * 2, clr.color(p, alpha), glowIntensity,
                     computedDampenRadius, blendMode, toneMap);
}
