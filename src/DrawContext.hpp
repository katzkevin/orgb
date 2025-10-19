//
//  DrawContext.hpp
//  orgb
//
//  Created by Kevin Katz on 4/4/22.
//

#ifndef DrawContext_hpp
#define DrawContext_hpp

#include "Utilities.hpp"
#include "ofMain.h"

ofFbo getConfiguredFrameBuffer(int width, int height);
ofShader createShader(std::string shaderName);
void applyShaderToCurrentFbo(ofFbo & sourceFrameBuffer, ofShader & shader,
                             std::function<void(ofShader &)> & configureShader);  // Write to active frame buffer
void drawFboAtZeroZero(ofFbo & sourceFrameBuffer);

#endif /* DrawContext_hpp */
