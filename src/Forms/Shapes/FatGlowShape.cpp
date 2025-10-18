//
//  FatGlowShape.cpp
//  orgb
//
//  Created by Kevin Katz on 6/14/22.
//

#include <stdio.h>
#include "FatGlowShape.hpp"

FatGlowShape::FatGlowShape(std::string name) : Shape(name) {
    glowIntensity.set(2.59);
    intensityAtEighthWidth.set(0.16);
    toneMap.set(false);
    drawMode.set(1);
}
