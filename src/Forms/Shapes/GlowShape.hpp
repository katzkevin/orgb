//
//  FatGlowShape.hpp
//  orgb
//
//  Created by Kevin Katz on 6/14/22.
//

#ifndef GlowShape_hpp
#define GlowShape_hpp

#include <stdio.h>
#include "Shape.hpp"

class GlowShape : public Shape {
public:
    GlowShape(std::string name);
    virtual ~GlowShape(){};
};

#endif /* FatGlowShape_hpp */
