#ifndef ORGB_CORE_COLOR_HPP
#define ORGB_CORE_COLOR_HPP

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace orgb {
namespace core {

/**
 * Simple color representation with RGB and HSV support
 * Compatible with openFrameworks ofColor but OF-independent
 */
struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    // Constructors
    Color() : r(255), g(255), b(255), a(255) {}

    Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255) : r(red), g(green), b(blue), a(alpha) {}

    Color(float red, float green, float blue, float alpha = 255.0f)
        : r(static_cast<uint8_t>(red)),
          g(static_cast<uint8_t>(green)),
          b(static_cast<uint8_t>(blue)),
          a(static_cast<uint8_t>(alpha)) {}

    // Factory method for HSB/HSV colors
    static Color fromHsb(int hue, int saturation, int brightness) {
        return fromHsb(static_cast<float>(hue), static_cast<float>(saturation), static_cast<float>(brightness));
    }

    static Color fromHsb(float hue, float saturation, float brightness) {
        // Normalize to 0-1 range
        float h = hue / 255.0f;
        float s = saturation / 255.0f;
        float v = brightness / 255.0f;

        float r, g, b;

        if (s == 0.0f) {
            // Achromatic (grey)
            r = g = b = v;
        } else {
            h = h * 6.0f;  // sector 0 to 5
            int i = static_cast<int>(floor(h));
            float f = h - i;  // factorial part of h
            float p = v * (1.0f - s);
            float q = v * (1.0f - s * f);
            float t = v * (1.0f - s * (1.0f - f));

            switch (i % 6) {
                case 0:
                    r = v;
                    g = t;
                    b = p;
                    break;
                case 1:
                    r = q;
                    g = v;
                    b = p;
                    break;
                case 2:
                    r = p;
                    g = v;
                    b = t;
                    break;
                case 3:
                    r = p;
                    g = q;
                    b = v;
                    break;
                case 4:
                    r = t;
                    g = p;
                    b = v;
                    break;
                case 5:
                    r = v;
                    g = p;
                    b = q;
                    break;
                default:
                    r = v;
                    g = t;
                    b = p;
                    break;
            }
        }

        return Color(static_cast<uint8_t>(r * 255.0f), static_cast<uint8_t>(g * 255.0f),
                     static_cast<uint8_t>(b * 255.0f));
    }

    // Get HSB values
    void getHsb(float & hue, float & saturation, float & brightness) const {
        float r_norm = r / 255.0f;
        float g_norm = g / 255.0f;
        float b_norm = b / 255.0f;

        float max = std::max({r_norm, g_norm, b_norm});
        float min = std::min({r_norm, g_norm, b_norm});
        float delta = max - min;

        // Brightness
        brightness = max * 255.0f;

        // Saturation
        if (max == 0.0f) {
            saturation = 0.0f;
            hue = 0.0f;
            return;
        }
        saturation = (delta / max) * 255.0f;

        // Hue
        if (delta == 0.0f) {
            hue = 0.0f;
        } else {
            if (max == r_norm) {
                hue = 60.0f * fmod(((g_norm - b_norm) / delta), 6.0f);
            } else if (max == g_norm) {
                hue = 60.0f * (((b_norm - r_norm) / delta) + 2.0f);
            } else {
                hue = 60.0f * (((r_norm - g_norm) / delta) + 4.0f);
            }

            if (hue < 0.0f) hue += 360.0f;
            hue = (hue / 360.0f) * 255.0f;  // Normalize to 0-255
        }
    }

    // Linear interpolation
    Color getLerped(const Color & target, float amount) const {
        float invAmount = 1.0f - amount;
        return Color(static_cast<uint8_t>(r * invAmount + target.r * amount),
                     static_cast<uint8_t>(g * invAmount + target.g * amount),
                     static_cast<uint8_t>(b * invAmount + target.b * amount),
                     static_cast<uint8_t>(a * invAmount + target.a * amount));
    }

    // Operators
    bool operator==(const Color & other) const { return r == other.r && g == other.g && b == other.b && a == other.a; }

    bool operator!=(const Color & other) const { return !(*this == other); }

    // Common colors
    static Color red() { return Color(255, 0, 0); }
    static Color green() { return Color(0, 255, 0); }
    static Color blue() { return Color(0, 0, 255); }
    static Color yellow() { return Color(255, 255, 0); }
    static Color cyan() { return Color(0, 255, 255); }
    static Color magenta() { return Color(255, 0, 255); }
    static Color white() { return Color(255, 255, 255); }
    static Color black() { return Color(0, 0, 0); }
};

}  // namespace core
}  // namespace orgb

#endif  // ORGB_CORE_COLOR_HPP
