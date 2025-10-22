//
//  MeshGrid.hpp
//  orgb
//
//  Created by Kevin Katz on 8/17/22.
//

#ifndef MeshGrid_hpp
#define MeshGrid_hpp

#include "Shape.hpp"

class MeshGrid : public Shape {
   public:
    explicit MeshGrid(const std::string & name);
    ~MeshGrid() override = default;

   protected:
    void drawUnit(const ofColor & color, KeyState & ks, DrawManager & dm, Press & press) override;

    ofParameter<bool> scaleWavelength;
    ofParameter<float> minPeriodSeconds;
    ofParameter<float> topToBottomPeriodRatio;
    ofParameter<float> a4WaveLength;
    // ofParameter<float> topToBottomWaveLengthRatio;
};

#endif /* MeshGrid_hpp */
