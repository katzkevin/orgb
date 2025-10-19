#version 120

uniform sampler2DRect tex0;
uniform vec2 resolution;
uniform float time;

// Displacement parameters
uniform float intensity;       // 0.0 - 100.0 pixels
uniform float noiseScale;      // 0.001 - 0.1
uniform float noiseSpeed;      // 0.0 - 5.0
uniform float warpAmount;      // 0.0 - 1.0
uniform float turbulence;      // 0.0 - 1.0

// Pseudo-random noise function
float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

// 2D noise function (Perlin-like)
float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);

    // Smooth interpolation (smoothstep)
    f = f * f * (3.0 - 2.0 * f);

    // Four corners of the pixel
    float a = rand(i);
    float b = rand(i + vec2(1.0, 0.0));
    float c = rand(i + vec2(0.0, 1.0));
    float d = rand(i + vec2(1.0, 1.0));

    // Bilinear interpolation
    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

// Fractal Brownian Motion - multi-octave noise for turbulence
float fbm(vec2 p, int octaves) {
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 1.0;

    for (int i = 0; i < octaves; i++) {
        value += amplitude * noise(p * frequency);
        frequency *= 2.0;
        amplitude *= 0.5;
    }

    return value;
}

// Domain warping - uses noise to distort the noise lookup itself
vec2 domainWarp(vec2 p, float amount) {
    float warpX = fbm(p + vec2(0.0, 0.0), 3);
    float warpY = fbm(p + vec2(5.2, 1.3), 3);

    return p + vec2(warpX, warpY) * amount * 10.0;
}

void main() {
    vec2 uv = gl_TexCoord[0].xy;

    // Animate noise field
    float t = time * noiseSpeed;

    // Scale UV for noise lookup
    vec2 noiseUV = uv * noiseScale;

    // Apply domain warping if enabled
    if (warpAmount > 0.0) {
        noiseUV = domainWarp(noiseUV + t * 0.1, warpAmount);
    }

    // Calculate number of octaves based on turbulence
    int octaves = int(mix(1.0, 6.0, turbulence));

    // Get displacement vectors using FBM
    float displaceX = fbm(noiseUV + vec2(t, 0.0), octaves);
    float displaceY = fbm(noiseUV + vec2(0.0, t) + 100.0, octaves);

    // Center the noise (0.5 is center, not 0)
    displaceX = (displaceX - 0.5) * 2.0;
    displaceY = (displaceY - 0.5) * 2.0;

    // Create displacement vector
    vec2 displacement = vec2(displaceX, displaceY) * intensity;

    // Add some swirling motion
    float angle = fbm(noiseUV * 0.5 + t * 0.3, 2) * 3.14159 * 2.0;
    float swirl = turbulence * 0.3;
    displacement += vec2(cos(angle), sin(angle)) * intensity * swirl;

    // Sample texture at displaced position
    vec2 displacedUV = uv + displacement;

    // Sample with bounds checking (avoid black edges)
    vec4 color;
    if (displacedUV.x >= 0.0 && displacedUV.x <= resolution.x &&
        displacedUV.y >= 0.0 && displacedUV.y <= resolution.y) {
        color = texture2DRect(tex0, displacedUV);
    } else {
        // Wrap or use original color at edges
        color = texture2DRect(tex0, uv);
    }

    gl_FragColor = color;
}
