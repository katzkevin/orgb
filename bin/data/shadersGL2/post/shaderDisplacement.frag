#version 120

uniform sampler2DRect tex0;
uniform vec2 resolution;
uniform float time;

// Displacement parameters
uniform float displacementStrength;  // 0.0 - 1.0, default 0.5
uniform float frequency;              // 0.1 - 10.0, default 1.0
uniform float blockSize;              // 1.0 - 50.0, default 10.0

// Pseudo-random function
float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

// Hash function for block-based randomness
float hash(float n) {
    return fract(sin(n) * 43758.5453);
}

// Get displacement for a given block
vec2 getBlockDisplacement(float blockY, float t) {
    // Use block position and time to generate unique random values
    float seed = blockY + floor(t * frequency);
    float r = hash(seed);

    // Only displace some blocks (creates intermittent glitch effect)
    float threshold = 0.7;
    if (r > threshold) {
        float displacementX = (hash(seed + 1.0) - 0.5) * 2.0;
        float displacementY = (hash(seed + 2.0) - 0.5) * 0.3;  // Less vertical displacement
        return vec2(displacementX, displacementY) * displacementStrength * 100.0;
    }

    return vec2(0.0);
}

// Get chromatic aberration offset for glitchy RGB separation
vec2 getChromaticOffset(float blockY, float t) {
    float seed = blockY + floor(t * frequency);
    float r = hash(seed + 3.0);

    // Apply chromatic aberration to displaced blocks
    if (r > 0.7) {
        return vec2(hash(seed + 4.0) * 5.0 * displacementStrength, 0.0);
    }

    return vec2(0.0);
}

void main() {
    vec2 uv = gl_TexCoord[0].xy;

    // Calculate which block this pixel belongs to
    float blockIndex = floor(uv.y / blockSize);

    // Get displacement for this block
    vec2 displacement = getBlockDisplacement(blockIndex, time);

    // Apply displacement
    vec2 displacedUV = uv + displacement;

    // Clamp to valid texture coordinates
    displacedUV = clamp(displacedUV, vec2(0.0), resolution);

    // Get chromatic aberration offset
    vec2 chromatic = getChromaticOffset(blockIndex, time);

    // Sample RGB channels separately for chromatic aberration effect
    float r = texture2DRect(tex0, displacedUV + chromatic).r;
    float g = texture2DRect(tex0, displacedUV).g;
    float b = texture2DRect(tex0, displacedUV - chromatic).b;
    float a = texture2DRect(tex0, displacedUV).a;

    vec4 color = vec4(r, g, b, a);

    // Add some digital noise to displaced blocks for extra glitch effect
    if (length(displacement) > 0.0) {
        float noise = rand(uv + time) * 0.1 * displacementStrength;
        color.rgb += vec3(noise);
    }

    gl_FragColor = color;
}
