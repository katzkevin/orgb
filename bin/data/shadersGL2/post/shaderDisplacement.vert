#version 120

// Basic passthrough vertex shader for post-processing effects

void main() {
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
