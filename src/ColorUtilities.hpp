//
//  ColorUtilities.hpp
//  orgb
//
//  Created by Kevin Katz on 6/29/22.
//

#ifndef ColorUtilities_hpp
#define ColorUtilities_hpp

#include "ofMain.h"

namespace ColorUtilities {
std::vector<ofColor> complementaryPalette(float baseHuePct);
std::vector<ofColor> triadPalette(float baseHuePct);
std::vector<ofColor> tertiaryPalette(float baseHuePct);
}  // namespace ColorUtilities

#endif /* ColorUtilities_hpp */
