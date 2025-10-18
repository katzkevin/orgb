#version 120

uniform sampler2DRect tex0;
uniform vec2 resolution;
uniform float time;

// Film grain parameters
uniform float grainIntensity;  // 0.0 - 1.0, default 0.05
uniform float grainSize;       // 1.0 - 4.0, default 1.5

// Pseudo-random noise function
float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

// 2D noise function
float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);

    // Smooth interpolation
    f = f * f * (3.0 - 2.0 * f);

    // Four corners of the pixel
    float a = rand(i);
    float b = rand(i + vec2(1.0, 0.0));
    float c = rand(i + vec2(0.0, 1.0));
    float d = rand(i + vec2(1.0, 1.0));

    // Bilinear interpolation
    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

// Animated grain with temporal variation
float filmGrain(vec2 uv, float time) {
    // Scale UV by grain size
    vec2 grainUV = uv / grainSize;

    // Add time-based variation for animation
    float t = time * 2.0;
    vec2 offset = vec2(rand(vec2(t)), rand(vec2(t + 1.0)));

    // Generate multi-octave noise for more organic grain
    float n = noise(grainUV + offset);
    n += 0.5 * noise(grainUV * 2.0 + offset * 2.0);
    n += 0.25 * noise(grainUV * 4.0 + offset * 4.0);

    // Normalize
    n /= 1.75;

    // Center around 0 and scale by intensity
    return (n - 0.5) * grainIntensity;
}

void main() {
    vec2 uv = gl_TexCoord[0].xy;

    // Sample original color
    vec4 color = texture2DRect(tex0, uv);

    // Generate grain
    float grain = filmGrain(uv, time);

    // Add grain to all RGB channels (monochromatic grain)
    color.rgb += vec3(grain);

    // Ensure we don't clip or go below 0
    color.rgb = clamp(color.rgb, 0.0, 1.0);

    gl_FragColor = color;
}
