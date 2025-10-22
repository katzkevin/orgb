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
    explicit GlowShape(const std::string & name);
    ~GlowShape() override = default;
};

#endif /* FatGlowShape_hpp */
