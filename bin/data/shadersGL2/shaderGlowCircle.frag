#version 120

#pragma include "../shaders/glow.glsl"
#pragma include "../shaders/sdf.glsl"
#pragma include "../shaders/blend_mux.glsl"

uniform sampler2DRect tex0;

uniform vec2 screenDimensions; // Not used for GL2, only necessary for ES2 where we need pct coords.
uniform vec3 center;
uniform float radius;
uniform float r;
uniform float g;
uniform float b;
uniform float a;
uniform float glowIntensity;
uniform float glowDampenRadius;
uniform int blendMode;
uniform bool toneMap;
varying vec2 texCoordVarying;

void main()
{
    float d = sdCircle(vec3(texCoordVarying, 0) - center, radius);
    d = max(0.000001, d); // Don't allow zero distance
    
    // https://www.shadertoy.com/view/3s3GDn
    float glow = getGlow(d, glowIntensity, glowDampenRadius);
    vec3 color = glow * vec3(r,g,b);
    color = toneMap ? toneMapColor(color) : color;
    
    vec4 background = texture2DRect(tex0, texCoordVarying);
    gl_FragColor = vec4(blendMux(blendMode, background.rgb, color, a), 1.0);
}
