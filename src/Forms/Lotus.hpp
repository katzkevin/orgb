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
    ~Lotus() override {};

    void update(KeyState void update(KeyState & ks, ColorProvider & clr);
                ks, ColorProvider void update(KeyState & ks, ColorProvider & clr); clr) override;
    void draw(KeyState void draw(KeyState & ks, ColorProvider & clr, DrawManager & dm);
              ks, ColorProvider void draw(KeyState & ks, ColorProvider & clr, DrawManager & dm);
              clr, DrawManager void draw(KeyState & ks, ColorProvider & clr, DrawManager & dm); dm) override;

    void drawLeaf(float scale, ofColor color, float salt);
    void rotatingPastelLotus();
    void circlesAllTheWayDown(float x, float y, float radius, int generation);
};

#endif /* Lotus_hpp */
