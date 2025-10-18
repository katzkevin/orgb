//
//  ImageWrapper.hpp
//  orgb
//
//  Created by Kevin Katz on 10/26/21.
//

#ifndef ImageWrapper_hpp
#define ImageWrapper_hpp

#include <stdio.h>

#include <boost/filesystem.hpp>
#include <string>

#include "ofMain.h"

using namespace boost::filesystem;

class ImageWrapper {
   public:
    ImageWrapper(path p, ofImage & i);
    virtual ~ImageWrapper() {};
    path imagePath;
    ofImage image;
    float scale;
    ofVec2f offset;
};

#endif /* ImageWrapper_hpp */
