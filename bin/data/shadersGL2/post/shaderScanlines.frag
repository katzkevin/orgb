#version 120

uniform sampler2DRect tex0;
uniform vec2 resolution;
uniform float time;

// Scanline parameters
uniform float scanlineIntensity;  // 0.0 - 1.0, default 0.3
uniform float scanlineCount;      // Lines per screen, default 200.0
uniform float scanlineSpeed;      // Animation speed, default 0.1

// Optional CRT effects
uniform float vignette;         // 0.0 - 1.0, default 0.3
uniform float rgbShift;         // 0.0 - 5.0, default 0.5 (subtle chromatic aberration)

// Vignette effect (darkens edges)
float getVignette(vec2 uv) {
    uv = uv * 2.0 - 1.0;  // Center coordinates (-1 to 1)
    float dist = length(uv);
    return 1.0 - smoothstep(0.5, 1.5, dist) * vignette;
}

void main() {
    vec2 uv = gl_TexCoord[0].xy;
    vec2 normalizedUV = uv / resolution;

    // Animated scanline position
    float scanlineOffset = time * scanlineSpeed;
    float scanlinePos = mod(normalizedUV.y * scanlineCount + scanlineOffset, 1.0);

    // Create scanline pattern (sine wave for smooth lines)
    float scanline = sin(scanlinePos * 3.14159 * 2.0) * 0.5 + 0.5;
    scanline = 1.0 - (scanline * scanlineIntensity);

    // Sample color with optional RGB shift
    vec4 color;
    if (rgbShift > 0.0) {
        // Chromatic aberration - separate RGB channels
        float r = texture2DRect(tex0, uv + vec2(rgbShift, 0.0)).r;
        float g = texture2DRect(tex0, uv).g;
        float b = texture2DRect(tex0, uv - vec2(rgbShift, 0.0)).b;
        color = vec4(r, g, b, texture2DRect(tex0, uv).a);
    } else {
        color = texture2DRect(tex0, uv);
    }

    // Apply scanlines
    color.rgb *= scanline;

    // Apply vignette
    if (vignette > 0.0) {
        float vignetteAmount = getVignette(normalizedUV);
        color.rgb *= vignetteAmount;
    }

    gl_FragColor = color;
}
