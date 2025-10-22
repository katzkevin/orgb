//
//  ImageWrapper.cpp
//  orgb
//
//  Created by Kevin Katz on 10/26/21.
//

#include "ImageWrapper.hpp"

using namespace std::filesystem;

ImageWrapper::ImageWrapper(const path & p, ofImage & i) {
    imagePath = p;
    image = i;
    scale = 1.0;
    offset = ofVec2f(0.0, 0.0);
}
