#version 120

varying vec2 texCoordVarying;
uniform vec2 resolution;
uniform float time;

// Creds to Danilo Guanabara
// https://www.shadertoy.com/view/XsXXDn
void main()
{
    vec3 c;
    float l,z=time;
    for(int i=0;i<3;i++) {
        vec2 uv,p=texCoordVarying.xy/resolution;
        uv=p;
        p-=.5;
        p.x*=resolution.x/resolution.y;
        z+=.07;
        l=length(p);
        l*=l;
        uv+=p/l*(sin(z)+1.)*abs(sin(l*9.-z-z));
        c[i]=.01/length(mod(uv,1.0)-.5);
    }
    gl_FragColor=vec4(c/l,time);
}


