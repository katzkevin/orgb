#version 120

uniform sampler2DRect tex0;

uniform vec4 mixFactors;

varying vec2 texCoordVarying;

void main()
{
    gl_FragColor = texture2DRect(tex0, texCoordVarying) * mixFactors;
}
