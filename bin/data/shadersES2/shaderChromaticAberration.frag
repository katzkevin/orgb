precision highp float;
uniform sampler2D tex0;
uniform vec2 aberration; // As pixels
uniform vec2 screenDimensions;
uniform float opacityPct;
varying vec2 texCoordVarying;

// Gaussian weights from http://dev.theomader.com/gaussian-kernel-calculator/

float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

void main()
{
    // Horizontal aberration, but w.r.t dimensions because we don't use rect here
    vec2 aberrationVector = aberration / screenDimensions;
    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
    color = texture2D(tex0, texCoordVarying);
    color.r = texture2D(tex0, texCoordVarying).r * (1.0-opacityPct) + texture2D(tex0, texCoordVarying + aberrationVector).r * opacityPct;
    color.g = texture2D(tex0, texCoordVarying).g * (1.0-opacityPct) + texture2D(tex0, texCoordVarying + aberrationVector).g * opacityPct;
    gl_FragColor = color;
}
