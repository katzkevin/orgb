//
//  PointWaves.hpp
//  orgb
//
//  Created by Kevin Katz on 5/4/22.
//

#ifndef PointWaves_hpp
#define PointWaves_hpp

#include <stdio.h>
#include "BaseWaves.hpp"

class PointWaves : public BaseWaves {
public:
    PointWaves(std::string name);
    virtual ~PointWaves(){};

protected:
    void drawUnit(ColorProvider &clr, KeyState & ks, DrawManager &dm, const Press &p, float alpha) override;
};

#endif /* PointWaves_hpp */
