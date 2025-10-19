# ORGB - WebAssembly Build

This directory contains the compiled WebAssembly build of ORGB for running in web browsers.

## Files

- `index.html` - Entry point (open this in a browser)
- `index.wasm` - WebAssembly binary (11 MB)
- `index.js` - JavaScript glue code (6.8 MB)
- `index.data` - Asset bundle (5.5 MB)
- `app.js`, `style.css` - UI assets

## Running

### Option 1: Using emrun (Recommended)

```bash
# From the emsdk environment
emrun .
```

### Option 2: Using Python HTTP server

```bash
# From this directory
python3 -m http.server 8000

# Then open http://localhost:8000 in your browser
```

### Option 3: Using any web server

Serve this directory with any HTTP server. Due to CORS and WebAssembly requirements, you **must** serve via HTTP/HTTPS (not file://).

## Browser Compatibility

- ✅ Chrome/Chromium - Full support
- ✅ Firefox - Full support
- ✅ Edge - Full support
- ⚠️  Safari - Limited WebGL support (may not work)

## Known Limitations

The browser build has the following limitations compared to the native build:

- **No network support**: MQTT, OSC, and NDI are disabled
- **No native file system access**: Limited to browser file APIs
- **WebGL only**: Uses OpenGL ES 2.0+ (no legacy fixed-function pipeline)

## Performance

The initial load may take several seconds due to the large WASM binary and asset bundle. Once loaded, performance should be comparable to native builds on modern hardware.
