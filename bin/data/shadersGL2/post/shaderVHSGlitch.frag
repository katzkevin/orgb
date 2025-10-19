#version 120

uniform sampler2DRect tex0;
uniform vec2 resolution;
uniform float time;

// VHS glitch parameters
uniform float intensity;          // 0.0 - 1.0
uniform float lineDisplacement;   // 0.0 - 100.0 pixels
uniform float colorBleed;         // 0.0 - 1.0
uniform float signalNoise;        // 0.0 - 1.0
uniform float trackingError;      // 0.0 - 1.0

// Pseudo-random noise function
float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

// 1D noise (for per-line effects)
float rand1d(float x) {
    return fract(sin(x * 12.9898) * 43758.5453);
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
    vec2 texCoord = uv;

    // Line number for per-line effects
    float lineHeight = 4.0; // pixels per "scanline"
    float line = floor(uv.y / lineHeight);

    // Horizontal line displacement (bad tracking)
    float lineNoise = rand1d(line + floor(time * 3.0));

    // Only displace some lines randomly
    if (lineNoise > 0.7) {
        float displacement = (lineNoise - 0.7) * lineDisplacement * intensity;
        texCoord.x += displacement;
    }

    // Vertical tracking error (wavy distortion)
    float trackingWave = sin(uv.y * 0.05 + time * 2.0) * trackingError * 30.0;
    texCoord.x += trackingWave * intensity;

    // Random line corruption (complete signal loss)
    float lineCorruption = rand1d(line + floor(time * 7.0));
    bool isCorruptLine = lineCorruption > (1.0 - signalNoise * intensity * 0.3);

    // Sample color with displacement
    vec4 color = texture2DRect(tex0, texCoord);

    // Color bleeding (horizontal blur/smear)
    if (colorBleed > 0.0) {
        vec4 bleed = vec4(0.0);
        float bleedAmount = colorBleed * intensity * 10.0;

        // Sample pixels to the left (bleeding effect)
        for (int i = 1; i <= 5; i++) {
            float offset = float(i) * bleedAmount;
            vec4 sample = texture2DRect(tex0, texCoord - vec2(offset, 0.0));
            bleed += sample * (1.0 / float(i + 1));
        }

        color = mix(color, bleed * 0.5, colorBleed * intensity * 0.5);
    }

    // Signal noise (random pixel corruption)
    float pixelNoise = noise(uv + time * 5.0);
    if (pixelNoise < signalNoise * intensity * 0.2) {
        color.rgb = vec3(rand(uv + time));
    }

    // Complete line corruption
    if (isCorruptLine) {
        float corruptionType = rand1d(line * 2.0);
        if (corruptionType < 0.33) {
            // Black line
            color.rgb = vec3(0.0);
        } else if (corruptionType < 0.66) {
            // White noise line
            color.rgb = vec3(rand(vec2(uv.x, line + time)));
        } else {
            // Color bars
            float barWidth = resolution.x / 8.0;
            float bar = floor(uv.x / barWidth);
            color.rgb = vec3(rand(vec2(bar, line)), rand(vec2(bar + 1.0, line)), rand(vec2(bar + 2.0, line)));
        }
    }

    // Slight desaturation (VHS degredation)
    float gray = dot(color.rgb, vec3(0.299, 0.587, 0.114));
    color.rgb = mix(color.rgb, vec3(gray), intensity * 0.2);

    gl_FragColor = color;
}
