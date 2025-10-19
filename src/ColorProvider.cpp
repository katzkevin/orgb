//
//  ColorProvider.cpp
//  orgb
//
//  Created by Kevin Katz on 3/5/22.
//

#include "ColorProvider.hpp"

#include "core/MathUtils.hpp"

void ColorProvider::floatParamChanged(float & v) {
    setPalette(baseHue, baseSaturation, baseValue, maxHue, maxSaturation, maxValue, clockwise);
}
void ColorProvider::boolParamChanged(bool & v) {
    setPalette(baseHue, baseSaturation, baseValue, maxHue, maxSaturation, maxValue, clockwise);
}

ColorProvider::ColorProvider() {
    baseHue.set("Base Hue", 0, 0, 255);                  // 0-255
    baseSaturation.set("Base Saturation", 255, 0, 255);  // 0-255
    baseValue.set("Base Value", 255, 0, 255);            // 0-255
    maxHue.set("Max Hue", 255, 0, 255);                  // 0-255
    maxSaturation.set("Max Saturation", 255, 0, 255);    // 0-255
    maxValue.set("Max Value", 255, 0, 255);              // 0-255
    clockwise.set("Clockwise", true);                    // if True, cycle per octave
    cyclical.set("Cyclical", true);                      // if True, cycle per octave

    baseHue.addListener(this, &ColorProvider::floatParamChanged);
    baseSaturation.addListener(this, &ColorProvider::floatParamChanged);
    baseValue.addListener(this, &ColorProvider::floatParamChanged);
    maxHue.addListener(this, &ColorProvider::floatParamChanged);
    maxSaturation.addListener(this, &ColorProvider::floatParamChanged);
    maxValue.addListener(this, &ColorProvider::floatParamChanged);
    clockwise.addListener(this, &ColorProvider::boolParamChanged);

    setPalette(baseHue, baseSaturation, baseValue, maxHue, maxSaturation, maxValue, clockwise);
}

ofColor ColorProvider::color(const Press & p) const {
    ofColor c;
    float indexPct;
    if (cyclical) {
        // notePct = (p.note % NUM_NOTES) / static_cast<float>(NUM_NOTES);
        indexPct = (p.note % NUM_NOTES) / static_cast<float>(NUM_NOTES);
    } else {
        indexPct = orgb::core::MathUtils::map(static_cast<float>(p.note), static_cast<float>(PIANO_MIDI_MIN),
                                              static_cast<float>(PIANO_MIDI_MAX), 0.0f, 1.0f, true);
    }

    float indexInPalette = indexPct * palette.size();

    if (abs(indexInPalette - round(indexInPalette)) < (1 / 255.0)) {
        // ofLogNotice() << "Using rounded color";
        c = palette.at(static_cast<int>(round(indexInPalette)));
    } else {
        // ofLogNotice() << "Using interpolated color: delta" << abs(indexInPalette - round(indexInPalette));
        int lo = floor(indexInPalette);
        int hi = ceil(indexInPalette);
        float between = indexPct * (palette.size() - 1) - static_cast<float>(lo);
        c = palette.at(lo).getLerped(palette.at(hi), between);
    }

    return c;
}

ofColor ColorProvider::color(const Press & p, double opacityPct) const {
    // assert(0 <= opacityPct && opacityPct <= 1.0); // TODO Assert
    ofColor c = color(p);
    return ofColor(c.r, c.g, c.b, opacityPct * 255);
}

void ColorProvider::setPalette(std::vector<ofColor> & colors) {
    palette.clear();
    palette = colors;
}

void ColorProvider::setPalette(float baseHue, float baseSaturation, float baseValue, float maxHue, float maxSaturation,
                               float maxValue, bool clockwise) {
    std::vector<ofColor> colors;
    //    if (baseHue <= maxHue) {
    //        for (int i = 0; i < NUM_NOTES; i++) {
    //            colors.push_back(ofColor::fromHsb(static_cast<int>(ofMap(i, 0, NUM_NOTES, baseHue, maxHue)), 255,
    //            255));
    //        }
    //    } else {
    //        for (int i = 0; i < NUM_NOTES; i++) {
    //            colors.push_back(ofColor::fromHsb(static_cast<int>(ofMap(i, 0, NUM_NOTES, baseHue, maxHue)), 255,
    //            255));
    //        }
    //    }

    float h = baseHue;
    float s = baseSaturation;
    float v = baseValue;

    float hueStepSize;

    if (clockwise) {
        if (baseHue <= maxHue) {
            hueStepSize = (maxHue - baseHue) / static_cast<float>(NUM_NOTES);
        } else {
            hueStepSize = (256.0f - (baseHue - maxHue)) / static_cast<float>(NUM_NOTES);
        }
    } else {
        if (baseHue <= maxHue) {
            hueStepSize = -(256.0f - (maxHue - baseHue)) / static_cast<float>(NUM_NOTES);
        } else {
            hueStepSize = -(baseHue - maxHue) / static_cast<float>(NUM_NOTES);
        }
    }
    float satStepSize = (maxSaturation - baseSaturation) / static_cast<float>(NUM_NOTES);
    float valStepSize = (maxValue - baseValue) / static_cast<float>(NUM_NOTES);

    for (int i = 0; i < NUM_NOTES; i++) {
        colors.push_back(ofColor::fromHsb(static_cast<int>(h), static_cast<int>(s), static_cast<int>(v)));
        h = positiveModulo(h + hueStepSize, 256.0f);
        s = orgb::core::MathUtils::clamp(s + satStepSize, 0.0f, 255.0f);
        v = orgb::core::MathUtils::clamp(v + valStepSize, 0.0f, 255.0f);
    }
    setPalette(colors);
}
