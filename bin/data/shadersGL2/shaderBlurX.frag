#version 120

uniform sampler2DRect tex0;
uniform vec2 screenDimensions; // Not used for GL2, only necessary for ES2 where we need pct coords.
uniform float blurOffsetStepPixels;
uniform float gain;
varying vec2 texCoordVarying;

// Gaussian weights from http://dev.theomader.com/gaussian-kernel-calculator/

void main()
{
    // Horizontal aberration, coming in as a pct so we don't need to adjust. For ES2, we don't use a Rect so we do.aaaa
    float blurOffset = blurOffsetStepPixels;
	vec4 color = vec4(0.0, 0.0, 0.0, 0.0);

    color += 0.000229 * texture2DRect(tex0, texCoordVarying + blurOffset * vec2(-4.0, 0.0));
	color += 0.005977 * texture2DRect(tex0, texCoordVarying + blurOffset * vec2(-3.0, 0.0));
	color += 0.060598 * texture2DRect(tex0, texCoordVarying + blurOffset * vec2(-2.0, 0.0));
	color += 0.241732 * texture2DRect(tex0, texCoordVarying + blurOffset * vec2(-1.0, 0.0));

	color += 0.382928 * texture2DRect(tex0, texCoordVarying);

	color += 0.241732 * texture2DRect(tex0, texCoordVarying + blurOffset * vec2(1.0, 0.0));
	color += 0.060598 * texture2DRect(tex0, texCoordVarying + blurOffset * vec2(2.0, 0.0));
	color += 0.005977 * texture2DRect(tex0, texCoordVarying + blurOffset * vec2(3.0, 0.0));
	color += 0.000229 * texture2DRect(tex0, texCoordVarying + blurOffset * vec2(4.0, 0.0));

    
    gl_FragColor = color * gain;
}
