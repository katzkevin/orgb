//
//  ColorProvider.hpp
//  orgb
//
//  Created by Kevin Katz on 3/5/22.
//

#ifndef ColorProvider_hpp
#define ColorProvider_hpp

#include "Press.hpp"
#include "Utilities.hpp"
#include "ofParameter.h"

class ColorProvider {
   public:
    ColorProvider();
    ~ColorProvider() override {};
    ofColor color(const Press & p) const;
    ofColor color(const Press & p, double opacityPct) const;

    void setPalette(std::vector<ofColor> & colors);
    void setPalette(float baseHue, float baseSaturation, float baseValue, float maxHue, float maxSaturation,
                    float maxValue, bool clockwise);
    void floatParamChanged(float & v);
    void boolParamChanged(bool & v);

    ofParameter<float> baseHue;         // 0-255
    ofParameter<float> baseSaturation;  // 0-255
    ofParameter<float> baseValue;       // 0-255
    ofParameter<float> maxHue;          // 0-255
    ofParameter<float> maxSaturation;   // 0-255
    ofParameter<float> maxValue;        // 0-255
    ofParameter<bool> clockwise;
    ofParameter<bool> cyclical;  // if True, cycle per octave

   private:
    std::vector<ofColor> palette;
};
#endif /* ColorProvider_hpp */
