//
//  NoiseGrid.hpp
//  orgb
//
//  Created by Kevin Katz on 8/17/22.
//

#ifndef NoiseGrid_hpp
#define NoiseGrid_hpp

#include <stdio.h>

#include "Shape.hpp"

class NoiseGrid : public Shape {
   public:
    NoiseGrid(std::string name);
    virtual ~NoiseGrid() {};

   protected:
    void drawUnit(const ofColor & color, KeyState & ks, DrawManager & dm, Press & press) override;

    ofParameter<float> baseZVelocityPerSecond;
    ofParameter<float> topToBottomZVelocityRatio;
    ofParameter<float> a4Frequency;
    ofParameter<bool> scaleFrequency;
    ofParameter<float> resolution;
    ofParameter<float> noiseAmplitude;
    // ofParameter<float> topToBottomWaveLengthRatio;

    ofImage canvas;
};

#endif /* NoiseGrid_hpp */
