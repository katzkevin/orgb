//
//  FatGlowShape.hpp
//  orgb
//
//  Created by Kevin Katz on 6/14/22.
//

#ifndef FatGlowShape_hpp
#define FatGlowShape_hpp

#include "Shape.hpp"

class FatGlowShape : public Shape {
   public:
    explicit FatGlowShape(const std::string & name);
    ~FatGlowShape() override = default;
};

#endif /* FatGlowShape_hpp */
