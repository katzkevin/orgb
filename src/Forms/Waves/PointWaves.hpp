//
//  PointWaves.hpp
//  orgb
//
//  Created by Kevin Katz on 5/4/22.
//

#ifndef PointWaves_hpp
#define PointWaves_hpp

#include "BaseWaves.hpp"

class PointWaves : public BaseWaves {
   public:
    explicit PointWaves(std::string name);
    ~PointWaves() override {};

   protected:
    void drawUnit(ColorProvider & clr, KeyState & ks, DrawManager & dm, const Press & p, float alpha) override;
};

#endif /* PointWaves_hpp */
