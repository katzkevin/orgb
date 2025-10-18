//
//  PointWaves.hpp
//  orgb
//
//  Created by Kevin Katz on 5/4/22.
//

#ifndef EdgeLasers_hpp
#define EdgeLasers_hpp

#include <stdio.h>
#include "BaseWaves.hpp"

class EdgeLasers : public BaseWaves {
public:
    EdgeLasers(std::string name);
    virtual ~EdgeLasers(){};
    
    ofParameter<bool> lineSegmentDeterministic;

protected:
    void drawUnit(ColorProvider &clr, KeyState & ks, DrawManager &dm, const Press &p, float alpha) override;
};

#endif /* EdgeLasers_hpp */
