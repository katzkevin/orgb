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

    // Layer effects for chaos and expressiveness
    // Order matters - displacement makes everything else more interesting
    postProcessing->addEffect(std::make_shared<DisplacementEffect>());
    postProcessing->addEffect(std::make_shared<ChromaticAberrationEffect>());
    postProcessing->addEffect(std::make_shared<FeedbackEffect>());
    postProcessing->addEffect(std::make_shared<VHSGlitchEffect>());
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
├── ChromaticAberrationEffect - Violent RGB channel separation
├── DisplacementEffect - Noise-based warping/distortion
├── FeedbackEffect - Echo/trails with displacement
├── FilmGrainEffect - Organic texture
├── ScanlinesEffect - CRT scanlines
└── VHSGlitchEffect - Analog glitch artifacts
```

## Using Effects

### ChromaticAberrationEffect

Aggressive RGB channel separation that responds to direction, radial distortion, and noise.

```cpp
auto chroma = std::make_shared<ChromaticAberrationEffect>();
chroma->setIntensity(5.0f);       // 0.0 - 50.0 pixels
chroma->setDirection(45.0f);      // 0.0 - 360.0 degrees
chroma->setRadialAmount(0.3f);    // 0.0 - 1.0 (stronger at edges)
chroma->setNoiseAmount(0.2f);     // 0.0 - 1.0 chaos
```

### DisplacementEffect

Organic warping using multi-octave noise fields.

```cpp
auto displace = std::make_shared<DisplacementEffect>();
displace->setIntensity(20.0f);     // 0.0 - 100.0 pixels
displace->setNoiseScale(0.01f);    // 0.001 - 0.1
displace->setNoiseSpeed(1.0f);     // 0.0 - 5.0
displace->setWarpAmount(0.5f);     // 0.0 - 1.0 domain warping
displace->setTurbulence(0.5f);     // 0.0 - 1.0 multi-octave
```

### FeedbackEffect

Blends transformed versions of the image with itself to create trails and echo effects.

```cpp
auto feedback = std::make_shared<FeedbackEffect>();
feedback->setIntensity(0.85f);         // 0.0 - 1.0 blend amount
feedback->setDisplacementX(0.0f);      // -10.0 - 10.0 pixels
feedback->setDisplacementY(0.0f);      // -10.0 - 10.0 pixels
feedback->setScale(0.998f);            // 0.95 - 1.05 (zoom per iteration)
feedback->setRotation(0.0f);           // -5.0 - 5.0 degrees
feedback->setFeedbackColor(0.0f);      // 0.0 - 1.0 color shift
```

### VHSGlitchEffect

Analog VHS artifacts: line displacement, color bleeding, tracking errors, and signal corruption.

```cpp
auto vhs = std::make_shared<VHSGlitchEffect>();
vhs->setIntensity(0.5f);           // 0.0 - 1.0 overall strength
vhs->setLineDisplacement(20.0f);   // 0.0 - 100.0 pixels
vhs->setColorBleed(0.4f);          // 0.0 - 1.0 horizontal smearing
vhs->setSignalNoise(0.3f);         // 0.0 - 1.0 corruption
vhs->setTrackingError(0.2f);       // 0.0 - 1.0 wavy distortion
```

### FilmGrainEffect

```cpp
auto grain = std::make_shared<FilmGrainEffect>();
grain->setIntensity(0.05f);    // 0.0 - 1.0
grain->setGrainSize(1.5f);     // 1.0 - 4.0
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

## Composition Strategies

### Layering for Maximum Chaos

Effects compound - later effects process the output of earlier effects. Here's a recommended order:

1. **Displacement** (foundation) - Makes everything else more organic
2. **Chromatic Aberration** - Amplifies displacement with color separation
3. **Feedback** - Creates temporal evolution and trails
4. **VHS Glitch** - Adds punctuation/moments of corruption
5. **Film Grain** - Adds final texture layer
6. **Scanlines** - Optional retro touch

```cpp
// Chaotic audio-reactive setup
auto displacement = std::make_shared<DisplacementEffect>();
displacement->setIntensity(30.0f);
displacement->setTurbulence(0.8f);

auto chroma = std::make_shared<ChromaticAberrationEffect>();
chroma->setIntensity(15.0f);
chroma->setRadialAmount(0.5f);
chroma->setNoiseAmount(0.4f);

auto feedback = std::make_shared<FeedbackEffect>();
feedback->setIntensity(0.9f);
feedback->setScale(0.995f);

auto vhs = std::make_shared<VHSGlitchEffect>();
vhs->setLineDisplacement(40.0f);
vhs->setColorBleed(0.6f);

postProcessing->addEffect(displacement);
postProcessing->addEffect(chroma);
postProcessing->addEffect(feedback);
postProcessing->addEffect(vhs);
```

### Audio-Reactive Parameters

Map audio features to effect parameters for dynamic response:

```cpp
// In update() - map audio to displacement
float bass = audioAnalyzer.getBassEnergy();        // 0.0 - 1.0
displacement->setIntensity(ofMap(bass, 0, 1, 5, 50));

// Transients trigger glitch bursts
if (audioAnalyzer.onsetDetected()) {
    vhs->setIntensity(1.0f);
    chroma->setIntensity(30.0f);
} else {
    vhs->setIntensity(vhs->getIntensity() * 0.95f);  // decay
    chroma->setIntensity(chroma->getIntensity() * 0.98f);
}

// High frequencies modulate chromatic direction
float highFreq = audioAnalyzer.getHighEnergy();
chroma->setDirection(ofGetElapsedTimef() * 30.0f + highFreq * 180.0f);

// Overall energy drives feedback
float energy = audioAnalyzer.getOverallEnergy();
feedback->setIntensity(ofMap(energy, 0, 1, 0.7, 0.95));
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
