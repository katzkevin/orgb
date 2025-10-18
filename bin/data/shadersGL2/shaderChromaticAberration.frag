#version 120

uniform sampler2DRect tex0;
uniform vec2 aberration; // As pixels
uniform vec2 screenDimensions; // Not used for GL2, only necessary for ES2 where we need pct coords.
uniform float opacityPct;
varying vec2 texCoordVarying;

// Gaussian weights from http://dev.theomader.com/gaussian-kernel-calculator/

float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

void main()
{
    // Horizontal aberration, coming in as a pct so we don't need to adjust. For ES2, we don't use a Rect so we do.
    vec2 aberrationVector = aberration;
    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
    color = texture2DRect(tex0, texCoordVarying);
    color.r = texture2DRect(tex0, texCoordVarying).r * (1.0-opacityPct) + texture2DRect(tex0, texCoordVarying + aberrationVector).r * opacityPct;
    color.g = texture2DRect(tex0, texCoordVarying).g * (1.0-opacityPct) + texture2DRect(tex0, texCoordVarying + aberrationVector).g * opacityPct;
    gl_FragColor = color;
}
