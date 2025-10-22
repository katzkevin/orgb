//
//  FatGlowShape.cpp
//  orgb
//
//  Created by Kevin Katz on 6/14/22.
//

#include "GlowShape.hpp"

#include <stdio.h>

GlowShape::GlowShape(const std::string & name) : Shape(name) {
    glowIntensity.set(0.729);
    intensityAtEighthWidth.set(0.06);
    toneMap.set(false);
    drawMode.set(1);
}
