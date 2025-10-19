//
//  ImageWrapper.hpp
//  orgb
//
//  Created by Kevin Katz on 10/26/21.
//

#ifndef ImageWrapper_hpp
#define ImageWrapper_hpp

#include <filesystem>
#include <string>

#include "ofMain.h"

using namespace std::filesystem;

class ImageWrapper {
   public:
    ImageWrapper(path p, ofImage & i);
    ~ImageWrapper() override {};
    path imagePath;
    ofImage image;
    float scale;
    ofVec2f offset;
};

#endif /* ImageWrapper_hpp */
