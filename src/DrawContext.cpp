//
//  DrawContext.cpp
//  orgb
//
//  Created by Kevin Katz on 4/4/22.
//

#include "DrawContext.hpp"

// TODO This is defined
#define MULTISAMPLE_COUNT 0

ofFbo getConfiguredFrameBuffer(int width, int height) {
    ofFbo::Settings settings;
    settings.numSamples = MULTISAMPLE_COUNT;  // also try 8, if your GPU supports it
    settings.useDepth = true;
    settings.width = width;
    settings.height = height;
    ofFbo fbo;
    fbo.allocate(settings);
    return fbo;
}

ofShader createShader(std::string shaderName) {
    ofShader shader;
    bool result;
#ifdef TARGET_OPENGLES
    ofLogVerbose("createShader") << "Using OpenGL ES Shader.";
    shader.load("shadersES2/" + shaderName);
#else
    if (ofIsGLProgrammableRenderer()) {
        ofLogVerbose("createShader") << "Using OpenGL3 Shader.";
        shader.load("shadersGL3/" + shaderName);
    } else {
        ofLogVerbose("createShader") << "Using OpenGL2 Shader.";
        shader.load("shadersGL2/" + shaderName);
    }
#endif
    // TODO This is useless. Doesn't indicate an error.
    //    if (!result) {
    //        ofLogError("createShader") << "Failed to load " << shaderName;
    //        throw std::invalid_argument("Failed to load " + shaderName);
    //    }
    return shader;
}

void applyShaderToCurrentFbo(ofFbo& sourceFrameBuffer, ofShader& shader, std::function<void(ofShader&)>& configureShader) {
    shader.begin();
    {
        configureShader(shader);
        drawFboAtZeroZero(sourceFrameBuffer);
    }
    shader.end();
}

void drawFboAtZeroZero(ofFbo& sourceFrameBuffer) {
    ofPushStyle();
    {
        ofSetColor(255, 255, 255, 255);
        // Opposite solution of
        //  https://forum.openframeworks.cc/t/fbo-problems-with-alpha/1643/26409
        // ? Works.
        kkDisableAlphaBlending();
        sourceFrameBuffer.draw(0, 0);
    }
    ofPopStyle();
}
