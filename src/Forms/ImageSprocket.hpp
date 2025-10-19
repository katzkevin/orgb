//
//  ImageSprocket.hpp
//  orgb
//
//  Created by Kevin Katz on 10/22/21.
//

#ifndef ImageSprocket_hpp
#define ImageSprocket_hpp

#include <stdio.h>

#include <boost/filesystem.hpp>
#include <string>

#include "ImageWrapper.hpp"
#include "KeyState.hpp"
#include "Press.hpp"
#include "VisualForm.hpp"
#include "ofMain.h"
#include "ofxGui.h"

class ImageSprocket : public VisualForm {
   public:
    ImageSprocket();
    virtual ~ImageSprocket() = default;

    void setup() override;
    void update(KeyState & ks, ColorProvider & clr) override;
    void draw(KeyState & ks, ColorProvider & clr, DrawManager & dm) override;

    void newPressHandler(ColorProvider & clr, Press & p) override;
    void pressHandler(Press & p) override;

    void updateUi();
    void loadPhotos(int count);

    void drawCenteredAndScaled(ImageWrapper & imageWrapper, bool fillScreen,
                               ofVec4f mixFactors = ofVec4f(1.0, 1.0, 1.0, 1.0), float alphaPct = 1.0);

    void maxPhotosChanged(int & count);
    void offsetXForegroundChanged(float & offsetX);
    void offsetYForegroundChanged(float & offsetY);
    void scaleForegroundChanged(float & scale);
    void offsetXBackgroundChanged(float & offsetX);
    void offsetYBackgroundChanged(float & offsetY);
    void scaleBackgroundChanged(float & scale);

    std::vector<ImageWrapper> images;

    ofShader shaderRGBMixer;

    ofFbo rgbMixFbo;
    ofFbo drawFbo;

    ofParameter<int> maxPhotos;
    ofParameter<int> index0;
    ofParameter<int> index1;

    ofParameter<bool> fillScreen;

    ofParameter<float> backgroundAlphaBlendPct;
    ofParameter<float> foregroundAlphaBlendPct;

    ofParameter<float> rgbMixForegroundRed;
    ofParameter<float> rgbMixForegroundGreen;
    ofParameter<float> rgbMixForegroundBlue;
    ofParameter<float> rgbMixBackgroundRed;
    ofParameter<float> rgbMixBackgroundGreen;
    ofParameter<float> rgbMixBackgroundBlue;

    ofParameter<float> offsetXForeground;
    ofParameter<float> offsetYForeground;
    ofParameter<float> scaleForeground;
    ofParameter<float> offsetXBackground;
    ofParameter<float> offsetYBackground;
    ofParameter<float> scaleBackground;
};

namespace ImageUtilities {
std::vector<ImageWrapper> getImages(boost::filesystem::path directory, unsigned int maxCount = 999999);

}

#endif /* ImageSprocket_hpp */
