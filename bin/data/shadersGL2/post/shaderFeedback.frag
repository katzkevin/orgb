#version 120

uniform sampler2DRect tex0;
uniform vec2 resolution;
uniform float time;

// Feedback parameters
uniform float intensity;         // 0.0 - 1.0 blend with transformed version
uniform float displacementX;     // -10.0 - 10.0 pixels
uniform float displacementY;     // -10.0 - 10.0 pixels
uniform float scale;             // 0.95 - 1.05
uniform float rotation;          // -5.0 - 5.0 degrees
uniform float feedbackColor;     // 0.0 - 1.0 color shift

// 2D rotation matrix
mat2 rotate2D(float angle) {
    float s = sin(angle);
    float c = cos(angle);
    return mat2(c, -s, s, c);
}

// Pseudo-random noise function
float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    vec2 uv = gl_TexCoord[0].xy;
    vec2 center = resolution * 0.5;

    // Sample current pixel
    vec4 currentColor = texture2DRect(tex0, uv);

    // Transform coordinates for feedback sample
    vec2 feedbackUV = uv;

    // Translate to center for rotation/scale
    feedbackUV -= center;

    // Apply rotation
    float rotRad = radians(rotation);
    feedbackUV = rotate2D(rotRad) * feedbackUV;

    // Apply scale
    feedbackUV *= scale;

    // Translate back and apply displacement
    feedbackUV += center;
    feedbackUV.x += displacementX;
    feedbackUV.y += displacementY;

    // Sample transformed position
    vec4 feedbackColor = texture2DRect(tex0, feedbackUV);

    // Apply color shift to feedback
    if (feedbackColor > 0.0) {
        // Rotate RGB channels for weird color bleeding
        vec3 shiftedColor = feedbackColor.rgb;
        float shiftAmount = feedbackColor * 0.1;

        shiftedColor.r = feedbackColor.r + shiftAmount * sin(time * 0.5);
        shiftedColor.g = feedbackColor.g + shiftAmount * sin(time * 0.5 + 2.094); // 120 degrees
        shiftedColor.b = feedbackColor.b + shiftAmount * sin(time * 0.5 + 4.189); // 240 degrees

        feedbackColor.rgb = mix(feedbackColor.rgb, shiftedColor, feedbackColor);
    }

    // Blend current with transformed feedback
    vec4 finalColor = mix(currentColor, feedbackColor, intensity);

    // Add slight accumulation over time (creates trails)
    // This simulates true feedback by making brighter areas persist
    float brightness = dot(currentColor.rgb, vec3(0.299, 0.587, 0.114));
    if (brightness > 0.1) {
        finalColor.rgb = max(finalColor.rgb, currentColor.rgb * 0.95);
    }

    gl_FragColor = finalColor;
}
