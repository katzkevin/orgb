# ORGB - Emscripten Browser Port

This is a git worktree dedicated to porting ORGB to run in web browsers using Emscripten.

## Overview

ORGB is being adapted to run in the browser using WebAssembly via Emscripten. This allows the audio-reactive visualization engine to run natively in modern web browsers without plugins.

## Prerequisites

1. **Emscripten SDK** - Install and activate the Emscripten toolchain:
   ```bash
   # Clone emsdk
   git clone https://github.com/emscripten-core/emsdk.git
   cd emsdk

   # Install and activate the latest version
   ./emsdk install latest
   ./emsdk activate latest

   # Set up environment variables (run this in each new terminal)
   source ./emsdk_env.sh
   ```

2. **openFrameworks 0.12.1+** - Must be the same version as configured in `config.make`
   - Path: `/Users/katz/workspace/of_v0.12.1_osx_release`
   - Ensure the `ofxEmscripten` addon is available

3. **Python** - For running the local web server with `emrun`

## Configuration Changes

### Key Differences from Native Build

1. **Addons**:
   - Added `ofxEmscripten` (required)
   - Disabled `ofxNDI` (native networking not supported)
   - Disabled `ofxMQTT` and `ofxOsc` (may need websocket adapters)

2. **Dependencies**:
   - Boost filesystem disabled (not available in browser)
   - Platform-specific macOS linker flags removed

3. **Build Output**:
   - Generates `bin/em/orgb/index.html` instead of native binary
   - Includes WebAssembly (.wasm), JavaScript (.js), and data files

## Building

1. **Ensure Emscripten is activated**:
   ```bash
   source /path/to/emsdk/emsdk_env.sh
   ```

2. **Build the project**:
   ```bash
   # From the org-emscripten directory
   make

   # Or for debug build
   make Debug
   ```

3. **Build artifacts** will be in:
   - Release: `bin/em/orgb/index.html`
   - Debug: `bin/em/orgb/index.html` (with debug symbols)

## Running

### Using emrun (Recommended)

```bash
# Run with the default browser
emrun bin/em/orgb

# List available browsers
emrun --list_browsers

# Run with a specific browser
emrun --browser firefox bin/em/orgb
```

### Using a Local Web Server

You can also use any web server. Due to CORS restrictions and WebAssembly requirements, you must serve via HTTP/HTTPS:

```bash
# Python 3
cd bin/em/orgb
python3 -m http.server 8000

# Then open http://localhost:8000 in your browser
```

### Browser Compatibility

- **Chrome/Chromium**: Full support
- **Firefox**: Full support
- **Safari**: Limited WebGL support (may not work)
- **Edge**: Full support

Note: WebGL 2.0 support is required. Older browsers may not work.

## Memory Configuration

Default memory: 128MB

If you encounter memory issues, increase the allocation in `config.make`:

```make
PROJECT_EMSCRIPTEN_TOTAL_MEMORY = 268435456  # 256MB
```

## Known Limitations

### Not Yet Implemented

- [ ] Audio input (microphone access) - needs browser permissions API
- [ ] NDI streaming (requires native networking)
- [ ] MQTT/OSC networking (needs websocket bridge)
- [ ] File system access (limited to virtual filesystem)

### Browser-Specific Limitations

1. **Audio Context**: Requires user interaction to start (browsers auto-block)
2. **File Loading**: Must use preloaded data or fetch API
3. **Threading**: Limited by browser SharedArrayBuffer policies
4. **Performance**: Slightly slower than native builds

## Development Workflow

### Making Changes

1. Edit code in this worktree (`org-emscripten`)
2. Build and test in browser
3. Commit browser-specific changes to `emscripten-port` branch
4. Changes that should go to main must be cherry-picked

### Code Adaptations

Use preprocessor directives for platform-specific code:

```cpp
#ifdef __EMSCRIPTEN__
    // Browser-specific code
    #include <emscripten.h>
    #include <emscripten/html5.h>
#else
    // Native code
#endif
```

### Adding Browser-Specific Features

Consider implementing:
- Web Audio API integration
- WebRTC for networking (instead of OSC/MQTT)
- IndexedDB for persistent storage
- WebGL-specific optimizations

## Troubleshooting

### Build Errors

1. **"Cannot find emcc"**: Ensure Emscripten SDK is activated
2. **Missing headers**: Check that ofxEmscripten addon is present
3. **Linker errors**: May need to adjust memory settings or remove unsupported libraries

### Runtime Errors

1. **Black screen**: Check browser console for WebGL errors
2. **Memory errors**: Increase `PROJECT_EMSCRIPTEN_TOTAL_MEMORY`
3. **File not found**: Ensure data files are in `bin/data/`
4. **Audio issues**: Check that AudioContext is started (requires user interaction)

### Performance Issues

1. Use Release build (optimized with -O3)
2. Profile with browser developer tools
3. Consider reducing visual complexity
4. Check for main thread blocking operations

## Resources

- [openFrameworks Emscripten Guide](https://openframeworks.cc/setup/emscripten/)
- [Emscripten Documentation](https://emscripten.org/docs/)
- [WebGL 2.0 Specification](https://www.khronos.org/webgl/)
- [Web Audio API](https://developer.mozilla.org/en-US/docs/Web/API/Web_Audio_API)

## Next Steps

1. [ ] Compile and verify basic functionality
2. [ ] Implement browser-based audio input
3. [ ] Replace NDI with WebRTC for video streaming
4. [ ] Add WebSocket bridge for OSC/MQTT
5. [ ] Optimize performance for 60fps in browser
6. [ ] Deploy to web hosting

---

**Note**: This is an experimental port. Not all features from the native version may be available or work identically in the browser.
