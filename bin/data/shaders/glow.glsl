float getGlow(float d, float glowIntensity, float dampenRadius) {
    return pow(dampenRadius / d, glowIntensity);
}

vec3 toneMapColor(vec3 color) {
    return 1.0 - exp(-color);
}

vec3 correctGamma(vec3 color, float gamma) {
    return pow(color, vec3(gamma));
}
