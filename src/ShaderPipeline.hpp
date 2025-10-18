//
//  ShaderPipeline.hpp
//  orgb
//
//  Manages a stack of post-processing shader effects with runtime toggling,
//  reordering, and efficient FBO ping-ponging.
//

#ifndef ShaderPipeline_hpp
#define ShaderPipeline_hpp

#include <memory>
#include <string>
#include <vector>

#include "ShaderEffect.hpp"
#include "ofMain.h"

// ============================================================================
// ShaderPipeline - Manages post-processing effect stack
// ============================================================================

class ShaderPipeline {
   public:
    ShaderPipeline(int width, int height);
    ~ShaderPipeline() = default;

    // Effect management
    void addEffect(std::shared_ptr<ShaderEffect> effect);
    void removeEffect(const std::string & name);
    void removeEffectAt(size_t index);
    void clearEffects();

    // Effect ordering
    void moveEffect(size_t fromIndex, size_t toIndex);
    void moveEffectUp(size_t index);    // Move toward beginning (applied earlier)
    void moveEffectDown(size_t index);  // Move toward end (applied later)

    // Effect access
    std::shared_ptr<ShaderEffect> getEffect(const std::string & name);
    std::shared_ptr<ShaderEffect> getEffectAt(size_t index);
    size_t getEffectCount() const { return effects.size(); }
    std::vector<std::shared_ptr<ShaderEffect>> & getEffects() { return effects; }

    // Bulk enable/disable
    void enableAll();
    void disableAll();
    void toggleAll();

    // Main render interface
    // Applies all enabled effects in order to the input FBO
    // Returns the final processed FBO
    ofFbo & process(ofFbo & input);

    // Direct draw to screen (convenience)
    void processAndDraw(ofFbo & input, float x = 0, float y = 0);

    // FBO management
    void resize(int width, int height);
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    // Debug/Info
    void printPipeline() const;
    std::string getPipelineSummary() const;

    // Statistics
    int getEnabledEffectCount() const;
    bool hasEnabledEffects() const;

   private:
    // Effect stack (applied in order)
    std::vector<std::shared_ptr<ShaderEffect>> effects;

    // Double-buffered FBOs for ping-pong processing
    ofFbo processingFbo1;
    ofFbo processingFbo2;

    int width;
    int height;

    // Helper to get appropriate processing FBO
    ofFbo & getCurrentFbo(int index);
    ofFbo & getNextFbo(int index);

    void initializeFbos();
};

// ============================================================================
// Preset Pipeline Builders
// ============================================================================

namespace ShaderPipelinePresets {

// Create a basic retro/glitch aesthetic pipeline
std::shared_ptr<ShaderPipeline> createRetroGlitchPipeline(int width, int height);

// Create a clean, minimal pipeline
std::shared_ptr<ShaderPipeline> createMinimalPipeline(int width, int height);

// Create an empty pipeline (for custom building)
std::shared_ptr<ShaderPipeline> createEmptyPipeline(int width, int height);

}  // namespace ShaderPipelinePresets

#endif /* ShaderPipeline_hpp */
