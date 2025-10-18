//
//  ImageSprocket.cpp
//  orgb
//
//  Created by Kevin Katz on 10/22/21.
//

#include "ImageSprocket.hpp"

#include <boost/filesystem.hpp>
#include <string>

#include "DrawContext.hpp"
#include "Utilities.hpp"
#include "ofMain.h"

// This also defined in DrawContext
#define MULTISAMPLE_COUNT 0

using namespace boost::filesystem;

ImageSprocket::ImageSprocket() : VisualForm("ImageSprocket") {
    index0 = 0;
    index1 = 0;
}

void ImageSprocket::setup() {
    shaderRGBMixer = createShader("shaderRGBMixer");

    ofFbo rgbMixFbo = getConfiguredFrameBuffer(ofGetWidth(), ofGetHeight());
    ofFbo drawFbo = getConfiguredFrameBuffer(ofGetWidth(), ofGetHeight());

    parameters.add(maxPhotos.set("maxPhotos", 2, 0, 100));
    parameters.add(index0.set("index0", 0, 0, static_cast<int>(images.size())));
    parameters.add(index1.set("index1", 0, 0, static_cast<int>(images.size())));

    parameters.add(fillScreen.set("fillScreen", false));

    parameters.add(backgroundAlphaBlendPct.set("backgroundAlphaBlendPct", 1.0, 0, 1.0));
    parameters.add(foregroundAlphaBlendPct.set("foregroundAlphaBlendPct", 1.0, 0, 1.0));

    parameters.add(rgbMixBackgroundRed.set("rgbMixBackroundRed", 1.0, 0, 1.0));
    parameters.add(rgbMixBackgroundGreen.set("rgbMixBackgroundGreen", 1.0, 0, 1.0));
    parameters.add(rgbMixBackgroundBlue.set("rgbMixBackgroundBlue", 1.0, 0, 1.0));
    parameters.add(rgbMixForegroundRed.set("rgbMixForegroundRed", 1.0, 0, 1.0));
    parameters.add(rgbMixForegroundGreen.set("rgbMixForegroundGreen", 1.0, 0, 1.0));
    parameters.add(rgbMixForegroundBlue.set("rgbMixForegroundBlue", 1.0, 0, 1.0));

    parameters.add(offsetXForeground.set("offsetXForeground", 0.0, -ofGetWidth(), ofGetWidth()));
    parameters.add(offsetYForeground.set("offsetYForeground", 0.0, -ofGetHeight(), ofGetHeight()));
    parameters.add(scaleForeground.set("scaleForeground", 1.0, 0.2, 8.0));
    parameters.add(offsetXBackground.set("offsetXBackground", 1.0, -ofGetWidth(), ofGetHeight()));
    parameters.add(offsetYBackground.set("offsetYBackground", 1.0, -ofGetWidth(), ofGetHeight()));
    parameters.add(scaleBackground.set("scaleBackground", 1.0, 0.2, 8.0));

    maxPhotos.addListener(this, &ImageSprocket::maxPhotosChanged);
    offsetXForeground.addListener(this, &ImageSprocket::offsetXForegroundChanged);
    offsetYForeground.addListener(this, &ImageSprocket::offsetYForegroundChanged);
    scaleForeground.addListener(this, &ImageSprocket::scaleForegroundChanged);
    offsetXBackground.addListener(this, &ImageSprocket::offsetXBackgroundChanged);
    offsetYBackground.addListener(this, &ImageSprocket::offsetYBackgroundChanged);
    scaleBackground.addListener(this, &ImageSprocket::scaleBackgroundChanged);

    loadPhotos(maxPhotos.get());

    rgbMixFbo = getConfiguredFrameBuffer(ofGetWidth(), ofGetHeight());
    drawFbo = getConfiguredFrameBuffer(ofGetWidth(), ofGetHeight());
}

void ImageSprocket::offsetXForegroundChanged(float &offsetX) { images[index0].offset[0] = offsetX; }
void ImageSprocket::offsetYForegroundChanged(float &offsetY) { images[index0].offset[1] = offsetY; }
void ImageSprocket::scaleForegroundChanged(float &scale) { images[index0].scale = scale; }
void ImageSprocket::offsetXBackgroundChanged(float &offsetX) { images[index1].offset[0] = offsetX; }
void ImageSprocket::offsetYBackgroundChanged(float &offsetY) { images[index1].offset[1] = offsetY; }
void ImageSprocket::scaleBackgroundChanged(float &scale) { images[index1].scale = scale; }

void ImageSprocket::maxPhotosChanged(int &count) {
    if (count > images.size()) {
        loadPhotos(count);
    }
}

void ImageSprocket::updateUi() {
    offsetXForeground.set(images[index0].offset.x);
    offsetYForeground.set(images[index0].offset.y);
    scaleForeground.set(images[index0].scale);
    offsetXBackground.set(images[index1].offset.x);
    offsetYBackground.set(images[index1].offset.y);
    scaleBackground.set(images[index1].scale);
}

void ImageSprocket::loadPhotos(int count) {
    ofDirectory ofDirectoryImages("imagesprocket/foreground");
    ofDirectory ofDirectoryDescartes("imagesprocket/descartes");
    path imagesDirectory(ofDirectoryImages.path());
    path descartesDirectory(ofDirectoryDescartes.path());

    images.clear();

    images = ImageUtilities::getImages(imagesDirectory, count);
    std::vector<ImageWrapper> descartes = ImageUtilities::getImages(descartesDirectory, count);
    for (auto img : descartes) {
        img.image.setImageType(OF_IMAGE_COLOR_ALPHA);
    }
    images.insert(images.end(), descartes.begin(), descartes.end());
}

void ImageSprocket::update(KeyState &ks, ColorProvider &clr) {}

void ImageSprocket::draw(KeyState &ks, ColorProvider &clr, DrawManager &dm) {
    // Because we use screen blending to double expose, using alpha is not effective for crossfade, control.
    // Instead, splash with white by a certain amount to wash out the the photo before blending.
    //    ofBackground(255,255,255);
    ofSetColor(ofColor::white);
    drawCenteredAndScaled(images[index0], fillScreen,
                          ofVec4f(rgbMixBackgroundRed, rgbMixBackgroundGreen, rgbMixBackgroundBlue, 1.0),
                          backgroundAlphaBlendPct);

    ofPushStyle();
    {
        ofEnableBlendMode(OF_BLENDMODE_SCREEN);
        drawCenteredAndScaled(images[index1], fillScreen,
                              ofVec4f(rgbMixForegroundRed, rgbMixForegroundGreen, rgbMixForegroundBlue, 1.0),
                              foregroundAlphaBlendPct);
    }
    ofPopStyle();
}

void ImageSprocket::pressHandler(Press &p) {
    switch (p.note) {
        case '-':
            backgroundAlphaBlendPct = ofClamp(backgroundAlphaBlendPct - 0.05, 0.0, 1.0);
            cout << "Alpha Updated to: " << backgroundAlphaBlendPct << endl;
            break;
        case '=':
            backgroundAlphaBlendPct = ofClamp(backgroundAlphaBlendPct + 0.05, 0.0, 1.0);
            cout << "Alpha Updated to: " << backgroundAlphaBlendPct << endl;
            break;
    }
}

void ImageSprocket::newPressHandler(ColorProvider &clr, Press &p) {
    switch (p.note) {
        case 'i':
            index0 = positive_modulo(index0 - 1, static_cast<int>(images.size()));
            break;
        case 'o':
            index0 = (index0 + 1) % static_cast<int>(images.size());
            break;
        case 'k':
            index1 = positive_modulo(index1 - 1, static_cast<int>(images.size()));
            break;
        case 'l':
            index1 = (index1 + 1) % static_cast<int>(images.size());
            break;
        case 'f':
            fillScreen = !fillScreen;
            break;
        case 'r':
            for (auto it = images.begin(); it != images.end(); ++it) {
                it->offset = ofVec2f(0.0, 0.0);
                it->scale = 1.0;
            }
        case 'x':
            int tmp = index0;
            index0 = index1;
            index1 = tmp;
            break;
    }
    // Update UI to reflect new image state
    updateUi();
}

void ImageSprocket::drawCenteredAndScaled(ImageWrapper &imageWrapper, bool fillScreen, ofVec4f mixFactors,
                                          float alphaPct) {
    ofImage image = imageWrapper.image;

    float horizontalScaleToFit = static_cast<float>(ofGetWidth()) / image.getWidth();
    float verticalScaleToFit = static_cast<float>(ofGetHeight()) / image.getHeight();

    int drawWidth, drawHeight;

    bool horizontalChangeIsLessDrastic = horizontalScaleToFit < verticalScaleToFit;

    if ((fillScreen && horizontalChangeIsLessDrastic) || (!fillScreen && !horizontalChangeIsLessDrastic)) {
        // Horizontal change is less drastic
        float horizontalScale = static_cast<float>(ofGetWidth()) / image.getWidth();
        drawWidth = horizontalScale * image.getWidth();
        drawHeight = horizontalScale * image.getHeight();
    } else {
        // Vertical change is less drastic
        float verticalScale = static_cast<float>(ofGetHeight()) / image.getHeight();
        drawWidth = verticalScale * image.getWidth();
        drawHeight = verticalScale * image.getHeight();
    }
    int topLeftX = ofGetWidth() / 2.0 - drawWidth / 2.0;
    int topLeftY = ofGetHeight() / 2.0 - drawHeight / 2.0;

    drawFbo = getConfiguredFrameBuffer(ofGetWidth(), ofGetHeight());
    rgbMixFbo = getConfiguredFrameBuffer(ofGetWidth(), ofGetHeight());

    rgbMixFbo.begin();
    ofClear(0);
    image.draw(topLeftX + imageWrapper.offset.x, topLeftY + imageWrapper.offset.y, drawWidth * imageWrapper.scale,
               drawHeight * imageWrapper.scale);

    // Because we use screen blend for double expose, blacken photos by a factor of 1-alphaPct
    ofPushStyle();
    {
        ofSetColor(0, 0, 0, ofMap(1.0 - alphaPct, 0, 1, 0, 255));
        ofDrawRectangle(0, 0, rgbMixFbo.getWidth(), rgbMixFbo.getHeight());
    }
    ofPopStyle();

    rgbMixFbo.end();

    std::function<void(ofShader &)> configureShader = [mixFactors](ofShader &shader) {
        shader.setUniform4f("mixFactors", mixFactors);
    };

    drawFbo.begin();
    applyShaderToCurrentFbo(rgbMixFbo, shaderRGBMixer, configureShader);
    drawFbo.end();
    drawFboAtZeroZero(drawFbo);
}
namespace ImageUtilities {

std::vector<ImageWrapper> getImages(boost::filesystem::path directory, unsigned int maxCount) {
    ofImageLoadSettings settings;
    settings.exifRotate = true;

    std::vector<ImageWrapper> acc;

    for (auto i = directory_iterator(directory); i != directory_iterator(); i++) {
        if (acc.size() > maxCount) {
            break;
        }
        if (!is_directory(i->path())) {
            cout << i->path().filename().string() << endl;
            ofImage img(i->path().string(), settings);
            ImageWrapper iw(i->path(), img);
            acc.push_back(iw);
        } else {
            continue;
        }
    }

    sort(acc.begin(), acc.end(), [](ImageWrapper a, ImageWrapper b) { return a.imagePath < b.imagePath; });
    return acc;
}

}  // namespace ImageUtilities
