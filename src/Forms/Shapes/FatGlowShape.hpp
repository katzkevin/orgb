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
    FatGlowShape(std::string name);
    ~FatGlowShape() override {};
};

#endif /* FatGlowShape_hpp */
