#pragma include "./blend.glsl"

vec3 blendMux(int blendMode, vec3 background, vec3 color, float a) {
    if (blendMode == 0) {
        return blendScreen(background, correctGamma(color, 0.4545), a);
    } else if (blendMode == 1) {
        return blendScreen(background, color, a);
    } else if (blendMode == 2) {
        return blendAdd(background, color, a);
    } else {
       return vec3(1.0, 0.0, 1.0);
    }
}

// Former 1
// This is wrong, doesn't render orange
// blendScreen(background, correctGamma(color, 2.2), a);

// This is wrong, doesn't render orange
// return blendScreen(background, correctGamma(color, 2.2), a);

// This is wrong, doesn't render orange
// return blendAdd(background, correctGamma(color, 2.2), a);

