//
//  Lotus.hpp
//  orgb
//
//  Created by Kevin Katz on 11/8/21.
//

#ifndef Lotus_hpp
#define Lotus_hpp

#include "KeyState.hpp"
#include "Press.hpp"
#include "Utilities.hpp"
#include "VisualForm.hpp"
#include "ofMain.h"

class Lotus : public VisualForm {
   public:
    Lotus();
    ~Lotus() override = default;

    void update(KeyState & ks, ColorProvider & clr) override;
    void draw(KeyState & ks, ColorProvider & clr, DrawManager & dm) override;

    void drawLeaf(float scale, ofColor color, float salt);
    void rotatingPastelLotus();
    void circlesAllTheWayDown(float x, float y, float radius, int generation);
};

#endif /* Lotus_hpp */
