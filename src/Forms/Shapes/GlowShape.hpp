//
//  FatGlowShape.hpp
//  orgb
//
//  Created by Kevin Katz on 6/14/22.
//

#ifndef GlowShape_hpp
#define GlowShape_hpp

#include "Shape.hpp"

class GlowShape : public Shape {
   public:
    GlowShape(std::string name);
    ~GlowShape() override {};
};

#endif /* FatGlowShape_hpp */
