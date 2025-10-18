//
//  ShaderPipeline.cpp
//  orgb
//
//  Implementation of shader effect pipeline
//

#include "ShaderPipeline.hpp"

#include "DrawContext.hpp"

// ============================================================================
// ShaderPipeline Implementation
// ============================================================================

ShaderPipeline::ShaderPipeline(int width, int height) : width(width), height(height) { initializeFbos(); }

void ShaderPipeline::initializeFbos() {
    processingFbo1 = getConfiguredFrameBuffer(width, height);
    processingFbo2 = getConfiguredFrameBuffer(width, height);

    // Clear both FBOs
    processingFbo1.begin();
    ofClear(0, 0, 0, 0);
    processingFbo1.end();

    processingFbo2.begin();
    ofClear(0, 0, 0, 0);
    processingFbo2.end();
}

void ShaderPipeline::addEffect(std::shared_ptr<ShaderEffect> effect) {
    if (!effect) {
        ofLogWarning("ShaderPipeline::addEffect") << "Attempted to add null effect";
        return;
    }

    // Check for duplicate names
    for (const auto & existing : effects) {
        if (existing->getName() == effect->getName()) {
            ofLogWarning("ShaderPipeline::addEffect") << "Effect '" << effect->getName() << "' already exists";
            return;
        }
    }

    effects.push_back(effect);
    ofLogNotice("ShaderPipeline::addEffect") << "Added effect: " << effect->getName();
}

void ShaderPipeline::removeEffect(const std::string & name) {
    auto it = std::remove_if(effects.begin(), effects.end(), [&name](const std::shared_ptr<ShaderEffect> & effect) {
        return effect->getName() == name;
    });

    if (it != effects.end()) {
        ofLogNotice("ShaderPipeline::removeEffect") << "Removed effect: " << name;
        effects.erase(it, effects.end());
    } else {
        ofLogWarning("ShaderPipeline::removeEffect") << "Effect not found: " << name;
    }
}

void ShaderPipeline::removeEffectAt(size_t index) {
    if (index < effects.size()) {
        std::string name = effects[index]->getName();
        effects.erase(effects.begin() + index);
        ofLogNotice("ShaderPipeline::removeEffectAt") << "Removed effect at " << index << ": " << name;
    } else {
        ofLogWarning("ShaderPipeline::removeEffectAt") << "Index out of bounds: " << index;
    }
}

void ShaderPipeline::clearEffects() {
    effects.clear();
    ofLogNotice("ShaderPipeline::clearEffects") << "Cleared all effects";
}

void ShaderPipeline::moveEffect(size_t fromIndex, size_t toIndex) {
    if (fromIndex >= effects.size() || toIndex >= effects.size()) {
        ofLogWarning("ShaderPipeline::moveEffect") << "Invalid indices: " << fromIndex << " -> " << toIndex;
        return;
    }

    if (fromIndex == toIndex) return;

    auto effect = effects[fromIndex];
    effects.erase(effects.begin() + fromIndex);
    effects.insert(effects.begin() + toIndex, effect);

    ofLogNotice("ShaderPipeline::moveEffect")
        << "Moved '" << effect->getName() << "' from " << fromIndex << " to " << toIndex;
}

void ShaderPipeline::moveEffectUp(size_t index) {
    if (index > 0 && index < effects.size()) {
        moveEffect(index, index - 1);
    }
}

void ShaderPipeline::moveEffectDown(size_t index) {
    if (index < effects.size() - 1) {
        moveEffect(index, index + 1);
    }
}

std::shared_ptr<ShaderEffect> ShaderPipeline::getEffect(const std::string & name) {
    for (auto & effect : effects) {
        if (effect->getName() == name) {
            return effect;
        }
    }
    return nullptr;
}

std::shared_ptr<ShaderEffect> ShaderPipeline::getEffectAt(size_t index) {
    if (index < effects.size()) {
        return effects[index];
    }
    return nullptr;
}

void ShaderPipeline::enableAll() {
    for (auto & effect : effects) {
        effect->setEnabled(true);
    }
    ofLogNotice("ShaderPipeline::enableAll") << "Enabled all effects";
}

void ShaderPipeline::disableAll() {
    for (auto & effect : effects) {
        effect->setEnabled(false);
    }
    ofLogNotice("ShaderPipeline::disableAll") << "Disabled all effects";
}

void ShaderPipeline::toggleAll() {
    for (auto & effect : effects) {
        effect->toggle();
    }
    ofLogNotice("ShaderPipeline::toggleAll") << "Toggled all effects";
}

ofFbo & ShaderPipeline::getCurrentFbo(int index) { return (index % 2 == 0) ? processingFbo1 : processingFbo2; }

ofFbo & ShaderPipeline::getNextFbo(int index) { return (index % 2 == 0) ? processingFbo2 : processingFbo1; }

ofFbo & ShaderPipeline::process(ofFbo & input) {
    if (effects.empty()) {
        return input;  // No effects - return input unchanged
    }

    // Count enabled effects to optimize ping-pong
    int enabledCount = getEnabledEffectCount();
    if (enabledCount == 0) {
        return input;  // All effects disabled
    }

    // Copy input to first processing FBO
    processingFbo1.begin();
    ofClear(0, 0, 0, 0);
    drawFboAtZeroZero(input);
    processingFbo1.end();

    // Apply effects in sequence, ping-ponging between FBOs
    int appliedCount = 0;
    for (size_t i = 0; i < effects.size(); i++) {
        auto & effect = effects[i];

        if (!effect->isEnabled()) {
            continue;  // Skip disabled effects
        }

        ofFbo & currentFbo = getCurrentFbo(appliedCount);
        ofFbo & nextFbo = getNextFbo(appliedCount);

        effect->apply(currentFbo, nextFbo);
        appliedCount++;
    }

    // Return the FBO with the final result
    // If odd number of enabled effects, result is in processingFbo2
    // If even number, result is in processingFbo1
    return getCurrentFbo(appliedCount);
}

void ShaderPipeline::processAndDraw(ofFbo & input, float x, float y) {
    ofFbo & result = process(input);
    ofPushStyle();
    ofSetColor(255, 255, 255, 255);
    result.draw(x, y);
    ofPopStyle();
}

void ShaderPipeline::resize(int newWidth, int newHeight) {
    if (newWidth == width && newHeight == height) {
        return;  // No change
    }

    width = newWidth;
    height = newHeight;

    initializeFbos();

    ofLogNotice("ShaderPipeline::resize") << "Resized to " << width << "x" << height;
}

void ShaderPipeline::printPipeline() const {
    ofLogNotice("ShaderPipeline") << "========================================";
    ofLogNotice("ShaderPipeline") << "Pipeline: " << effects.size() << " effects total, " << getEnabledEffectCount()
                                  << " enabled";
    ofLogNotice("ShaderPipeline") << "Resolution: " << width << "x" << height;
    ofLogNotice("ShaderPipeline") << "========================================";

    for (size_t i = 0; i < effects.size(); i++) {
        auto & effect = effects[i];
        std::string status = effect->isEnabled() ? "[ENABLED]" : "[DISABLED]";
        std::string valid = effect->isValid() ? "VALID" : "INVALID";

        ofLogNotice("ShaderPipeline") << i << ". " << status << " " << effect->getName() << " (" << valid << ")";

        if (!effect->isValid()) {
            ofLogNotice("ShaderPipeline") << "   Error: " << effect->getErrorMessage();
        }
    }

    ofLogNotice("ShaderPipeline") << "========================================";
}

std::string ShaderPipeline::getPipelineSummary() const {
    std::stringstream ss;
    ss << "Pipeline [" << getEnabledEffectCount() << "/" << effects.size() << " enabled]: ";

    for (size_t i = 0; i < effects.size(); i++) {
        if (i > 0) ss << " -> ";
        ss << effects[i]->getName();
        if (!effects[i]->isEnabled()) ss << "(off)";
    }

    return ss.str();
}

int ShaderPipeline::getEnabledEffectCount() const {
    int count = 0;
    for (const auto & effect : effects) {
        if (effect->isEnabled()) count++;
    }
    return count;
}

bool ShaderPipeline::hasEnabledEffects() const { return getEnabledEffectCount() > 0; }

// ============================================================================
// Preset Pipeline Builders
// ============================================================================

namespace ShaderPipelinePresets {

std::shared_ptr<ShaderPipeline> createRetroGlitchPipeline(int width, int height) {
    auto pipeline = std::make_shared<ShaderPipeline>(width, height);

    // TODO: Add actual effects once they're implemented
    // pipeline->addEffect(std::make_shared<FilmGrainEffect>());
    // pipeline->addEffect(std::make_shared<ScanlinesEffect>());

    ofLogNotice("ShaderPipelinePresets") << "Created RetroGlitch pipeline";
    return pipeline;
}

std::shared_ptr<ShaderPipeline> createMinimalPipeline(int width, int height) {
    auto pipeline = std::make_shared<ShaderPipeline>(width, height);

    // TODO: Add minimal effects
    // pipeline->addEffect(std::make_shared<FilmGrainEffect>());

    ofLogNotice("ShaderPipelinePresets") << "Created Minimal pipeline";
    return pipeline;
}

std::shared_ptr<ShaderPipeline> createEmptyPipeline(int width, int height) {
    auto pipeline = std::make_shared<ShaderPipeline>(width, height);
    ofLogNotice("ShaderPipelinePresets") << "Created Empty pipeline";
    return pipeline;
}

}  // namespace ShaderPipelinePresets
