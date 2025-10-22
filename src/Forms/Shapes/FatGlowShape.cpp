//
//  FatGlowShape.cpp
//  orgb
//
//  Created by Kevin Katz on 6/14/22.
//

#include "FatGlowShape.hpp"

#include <stdio.h>

FatGlowShape::FatGlowShape(const std::string & name) : Shape(name) {
    glowIntensity.set(2.59);
    intensityAtEighthWidth.set(0.16);
    toneMap.set(false);
    drawMode.set(1);
}
