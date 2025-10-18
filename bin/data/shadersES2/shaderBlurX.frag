
precision highp float;

uniform sampler2D tex0;
uniform vec2 screenDimensions; // Not used for GL2, only necessary for ES2 where we need pct coords.
uniform float blurOffsetStepPixels;
uniform float gain; // 1 default
varying vec2 texCoordVarying;

// Gaussian weights from http://dev.theomader.com/gaussian-kernel-calculator/

void main()
{
    // Horizontal aberration, but w.r.t dimensions because we don't use rect here (see ES2 for use with / screenDimensions)
    float blurOffset = blurOffsetStepPixels / screenDimensions.x;
    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);

	color += 0.000229 * texture2D(tex0, texCoordVarying + blurOffset * vec2(-4.0, 0.0));
	color += 0.005977 * texture2D(tex0, texCoordVarying + blurOffset * vec2(-3.0, 0.0));
	color += 0.060598 * texture2D(tex0, texCoordVarying + blurOffset * vec2(-2.0, 0.0));
	color += 0.241732 * texture2D(tex0, texCoordVarying + blurOffset * vec2(-1.0, 0.0));
	color += 0.382928 * texture2D(tex0, texCoordVarying);
	color += 0.241732 * texture2D(tex0, texCoordVarying + blurOffset * vec2(1.0, 0.0));
	color += 0.060598 * texture2D(tex0, texCoordVarying + blurOffset * vec2(2.0, 0.0));
	color += 0.005977 * texture2D(tex0, texCoordVarying + blurOffset * vec2(3.0, 0.0));
	color += 0.000229 * texture2D(tex0, texCoordVarying + blurOffset * vec2(4.0, 0.0));
    
    gl_FragColor = color * gain;   
}
