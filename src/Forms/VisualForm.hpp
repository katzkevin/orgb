//
//  VisualForm.hpp
//  orgb
//
//  Created by Kevin Katz on 10/28/21.
//

#ifndef VisualForm_hpp
#define VisualForm_hpp

#include "ColorProvider.hpp"
#include "DrawManager.hpp"
#include "KeyState.hpp"
#include "ofCamera.h"
#include "ofxGui.h"

class VisualForm {
   public:
    VisualForm(std::string name);
    virtual ~VisualForm() = default;
    virtual void setup() {};
    virtual void update(KeyState & ks, ColorProvider & clr) {};
    virtual void draw(KeyState & ks, ColorProvider & clr, DrawManager & dm) = 0;  // ABSTRACT!
    virtual void newPressHandler(ColorProvider & clr, Press & p) {};
    virtual void pressHandler(Press & p) {};

    ofParameterGroup parameters;
    std::string name;
};

#endif /* VisualForm_hpp */
