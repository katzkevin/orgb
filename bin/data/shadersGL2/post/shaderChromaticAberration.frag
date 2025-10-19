#version 120

uniform sampler2DRect tex0;
uniform vec2 resolution;
uniform float time;

// Chromatic aberration parameters
uniform float intensity;       // 0.0 - 50.0 pixels of separation
uniform float direction;       // 0.0 - 360.0 degrees
uniform float radialAmount;    // 0.0 - 1.0, radial distortion from center
uniform float noiseAmount;     // 0.0 - 1.0, chaos/instability

// Pseudo-random noise function
float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

// 2D noise function
float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);

    float a = rand(i);
    float b = rand(i + vec2(1.0, 0.0));
    float c = rand(i + vec2(0.0, 1.0));
    float d = rand(i + vec2(1.0, 1.0));

    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

void main() {
    vec2 uv = gl_TexCoord[0].xy;
    vec2 center = resolution * 0.5;

    // Calculate base offset direction
    float angleRad = radians(direction);
    vec2 baseOffset = vec2(cos(angleRad), sin(angleRad));

    // Add radial component (stronger at edges)
    vec2 toCenter = center - uv;
    float distFromCenter = length(toCenter) / length(center);
    vec2 radialOffset = normalize(toCenter) * distFromCenter * radialAmount;

    // Add noise-based chaos
    float noiseX = noise(uv * 0.01 + time * 0.5) - 0.5;
    float noiseY = noise(uv * 0.01 + time * 0.7 + 100.0) - 0.5;
    vec2 chaosOffset = vec2(noiseX, noiseY) * noiseAmount;

    // Combine offsets
    vec2 totalOffset = (baseOffset + radialOffset + chaosOffset);

    // Sample RGB channels with different offsets
    // Red shifted in positive direction
    vec2 rOffset = uv + totalOffset * intensity;
    // Green stays centered (or slight offset)
    vec2 gOffset = uv + totalOffset * intensity * 0.5;
    // Blue shifted in negative direction
    vec2 bOffset = uv - totalOffset * intensity * 0.5;

    // Sample each channel
    float r = texture2DRect(tex0, rOffset).r;
    float g = texture2DRect(tex0, gOffset).g;
    float b = texture2DRect(tex0, bOffset).b;

    // Get alpha from center sample
    float a = texture2DRect(tex0, uv).a;

    gl_FragColor = vec4(r, g, b, a);
}
