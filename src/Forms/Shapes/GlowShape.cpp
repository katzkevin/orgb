//
//  FatGlowShape.cpp
//  orgb
//
//  Created by Kevin Katz on 6/14/22.
//

#include <stdio.h>
#include "GlowShape.hpp"

GlowShape::GlowShape(std::string name) : Shape(name) {
    glowIntensity.set(0.729);
    intensityAtEighthWidth.set(0.06);
    toneMap.set(false);
    drawMode.set(1);
}
