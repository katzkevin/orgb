//
//  LaserWaves.cpp
//  orgb
//
//  Created by Kevin Katz on 2/22/22.
//

#include "LaserWaves.hpp"

LaserWaves::LaserWaves(std::string name) : BaseWaves(name) {
    glowIntensity.set(2.98);
    intensityAtEighthWidth.set(0.13);
    blendMode.set(0);
    toneMap.set(false);
}
