//
//  ColorUtilities.cpp
//  orgb
//
//  Created by Kevin Katz on 6/29/22.
//

#include "ColorUtilities.hpp"

#define PALETTE_GRANULARITY 12
namespace ColorUtilities {
std::vector<ofColor> complementaryPalette(float baseHuePct) {
    std::vector<ofColor> cs;
    for (int i = 0; i < PALETTE_GRANULARITY; i++) {
        float offset = (1 / 2.0) * (i / 12.0);
        cs.push_back(ofColor::fromHsb(255 * (baseHuePct + offset), 255, 255));
    }
    return cs;
}

std::vector<ofColor> triadPalette(float baseHuePct) {
    std::vector<ofColor> cs;
    for (int i = 0; i < PALETTE_GRANULARITY; i++) {
        float offset = (2 / 3.0) * (i / 12.0);
        cs.push_back(ofColor::fromHsb(255 * (baseHuePct + offset), 255, 255));
    }
    return cs;
}

std::vector<ofColor> tertiaryPalette(float baseHuePct) {
    std::vector<ofColor> cs;
    for (int i = 0; i < PALETTE_GRANULARITY; i++) {
        float offset = (1 / 6.0) * (i / 12.0);
        cs.push_back(ofColor::fromHsb(255 * (baseHuePct + offset), 255, 255));
    }
    return cs;
}

}  // namespace ColorUtilities
