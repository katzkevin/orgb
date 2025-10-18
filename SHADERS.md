# ORGB Shader System

Post-processing shader pipeline for ORGB visualization engine.

## Quick Start

```cpp
#include "ShaderPipeline.hpp"
#include "Effects/AllEffects.hpp"

// In ofApp.h
std::shared_ptr<ShaderPipeline> postProcessing;
ofFbo renderTarget;

// In setup()
void ofApp::setup() {
    renderTarget.allocate(1920, 1080, GL_RGBA);
    postProcessing = std::make_shared<ShaderPipeline>(1920, 1080);

    // Add effects
    postProcessing->addEffect(std::make_shared<FilmGrainEffect>());
    postProcessing->addEffect(std::make_shared<ScanlinesEffect>());
}

// In draw()
void ofApp::draw() {
    // Render scene to FBO
    renderTarget.begin();
    // ... your visualization ...
    renderTarget.end();

    // Apply post-processing
    postProcessing->processAndDraw(renderTarget, 0, 0);
}
```

## Architecture

```
ShaderPipeline
├── manages effect chain
├── handles FBO ping-pong
└── provides hot-reload support

ShaderEffect (base class)
├── loads/compiles shaders
├── manages parameters
└── applies effect to texture

Effects
├── FilmGrainEffect
└── ScanlinesEffect
```

## Using Effects

### FilmGrainEffect

```cpp
auto grain = std::make_shared<FilmGrainEffect>();
grain->setIntensity(0.05f);    // 0.0 - 1.0
grain->setGrainSize(1.5f);     // 1.0 - 4.0
grain->setEnabled(true);
```

### ScanlinesEffect

```cpp
auto scanlines = std::make_shared<ScanlinesEffect>();
scanlines->setIntensity(0.3f);        // 0.0 - 1.0
scanlines->setScanlineCount(240.0f);  // number of lines
scanlines->setRGBShift(1.0f);         // chromatic aberration
scanlines->setVignette(0.4f);         // 0.0 - 1.0
```

### Pipeline Control

```cpp
// Toggle effects
if (auto effect = postProcessing->getEffect("Film Grain")) {
    effect->toggle();
}

// Enable/disable all
postProcessing->enableAll();
postProcessing->disableAll();

// Remove effect
postProcessing->removeEffect("Scanlines");
```

## Creating Custom Effects

1. Create effect class inheriting from `ShaderEffect`:

```cpp
class MyEffect : public ShaderEffect {
public:
    MyEffect() : ShaderEffect("myeffect", "post") {
        intensity = 1.0f;
    }

    void applyUniforms() override {
        shader.setUniform1f("intensity", intensity);
    }

    void setIntensity(float val) { intensity = val; }

private:
    float intensity;
};
```

2. Create shader files:
- `bin/data/shadersGL2/post/shaderMyeffect.frag`
- `bin/data/shadersGL2/post/shaderMyeffect.vert`

3. Add to pipeline:

```cpp
postProcessing->addEffect(std::make_shared<MyEffect>());
```

## Validation & Testing

### Validate Shaders

```bash
just shader-validate          # quick check
just shader-validate-verbose  # detailed output
```

### Run Tests

```bash
just shader-test  # C++ unit tests
just shader-check # validation + tests
```

### Files

**Core:**
- `src/ShaderEffect.hpp/cpp` - Base effect class
- `src/ShaderPipeline.hpp/cpp` - Pipeline manager
- `src/ShaderHotReloader.hpp` - Auto-reload on file changes
- `src/Effects/` - Effect implementations

**Shaders:**
- `bin/data/shadersGL2/post/` - GL2 shaders
- `bin/data/shadersGL3/post/` - GL3 shaders (future)
- `bin/data/shadersES2/post/` - ES2 shaders (future)

**Tools:**
- `scripts/validateShaders.sh` - Offline shader validation
- `tests/shader_tests.cpp` - Unit tests
