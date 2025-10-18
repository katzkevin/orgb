#version 120

uniform sampler2DRect tex0;
uniform vec2 resolution;
uniform float time;

// Glitch parameters
uniform float glitchIntensity;     // 0.0 - 1.0, overall effect strength
uniform float blockSize;           // 8.0 - 64.0, size of glitch blocks
uniform float colorShiftAmount;    // 0.0 - 20.0, RGB channel displacement
uniform float noiseAmount;         // 0.0 - 1.0, random pixel noise

// Random function
float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

// Random 2D value
vec2 rand2(vec2 co) {
    return vec2(
        rand(co),
        rand(co + vec2(0.5, 0.5))
    );
}

// Block-based random (same random value per block)
float blockRand(vec2 uv, float blockSize) {
    vec2 blockPos = floor(uv / blockSize);
    return rand(blockPos + vec2(time));
}

void main() {
    vec2 uv = gl_TexCoord[0].xy;
    vec2 normalizedUV = uv / resolution;

    // Calculate which block we're in
    vec2 blockPos = floor(normalizedUV * resolution / blockSize);
    float blockRandom = rand(blockPos + vec2(floor(time * 10.0)));

    // Glitch trigger - only glitch some blocks
    float glitchTrigger = step(1.0 - glitchIntensity * 0.5, blockRandom);

    // Horizontal displacement (classic digital glitch effect)
    float displacement = 0.0;
    if (glitchTrigger > 0.5) {
        float rowRandom = rand(vec2(normalizedUV.y * 100.0, floor(time * 5.0)));
        displacement = (rowRandom - 0.5) * glitchIntensity * 50.0;
    }

    vec2 displacedUV = uv + vec2(displacement, 0.0);

    // RGB channel separation (chromatic aberration on steroids)
    vec4 color;
    if (glitchTrigger > 0.5 && colorShiftAmount > 0.0) {
        float shift = colorShiftAmount * glitchIntensity;
        float r = texture2DRect(tex0, displacedUV + vec2(shift, 0.0)).r;
        float g = texture2DRect(tex0, displacedUV).g;
        float b = texture2DRect(tex0, displacedUV - vec2(shift, 0.0)).b;
        color = vec4(r, g, b, 1.0);
    } else {
        color = texture2DRect(tex0, displacedUV);
    }

    // Add random pixel noise
    if (noiseAmount > 0.0) {
        float pixelNoise = rand(normalizedUV * resolution + vec2(time * 100.0));
        if (pixelNoise > (1.0 - noiseAmount * glitchIntensity * 0.1)) {
            color.rgb = vec3(pixelNoise);
        }
    }

    // Random color inversion blocks
    float invertTrigger = blockRand(normalizedUV * resolution, blockSize * 2.0);
    if (invertTrigger > (1.0 - glitchIntensity * 0.2)) {
        color.rgb = vec3(1.0) - color.rgb;
    }

    gl_FragColor = color;
}
