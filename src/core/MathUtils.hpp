#ifndef ORGB_CORE_MATH_UTILS_HPP
#define ORGB_CORE_MATH_UTILS_HPP

#include <algorithm>
#include <cmath>
#include <limits>


namespace orgb::core {

/**
 * Mathematical utility functions
 * Replacements for openFrameworks ofMap, ofClamp, ofIsFloatEqual
 */
class MathUtils {
   public:
    /**
     * Maps a value from one range to another
     * Equivalent to openFrameworks ofMap()
     *
     * @param value The value to map
     * @param inputMin Minimum value of input range
     * @param inputMax Maximum value of input range
     * @param outputMin Minimum value of output range
     * @param outputMax Maximum value of output range
     * @param clampResult If true, clamp result to output range
     * @return Mapped value
     */
    template <typename T>
    static T map(T value, T inputMin, T inputMax, T outputMin, T outputMax, bool clampResult = false) {
        if (std::abs(inputMax - inputMin) < std::numeric_limits<T>::epsilon()) {
            return outputMin;
        }

        T outVal = ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);

        if (clampResult) {
            if (outputMax < outputMin) {
                return clamp(outVal, outputMax, outputMin);
            }
            return clamp(outVal, outputMin, outputMax);
        }
        return outVal;
    }

    /**
     * Clamps a value between min and max
     * Equivalent to openFrameworks ofClamp()
     *
     * @param value The value to clamp
     * @param min Minimum value
     * @param max Maximum value
     * @return Clamped value
     */
    template <typename T>
    static T clamp(T value, T min, T max) {
        return std::max(min, std::min(value, max));
    }

    /**
     * Checks if two floats are equal within epsilon tolerance
     * Equivalent to openFrameworks ofIsFloatEqual()
     *
     * @param a First value
     * @param b Second value
     * @param epsilon Tolerance (default: 1e-6f)
     * @return True if values are equal within epsilon
     */
    static bool floatEqual(float a, float b, float epsilon = 1e-6f) {
        return std::abs(a - b) < epsilon;
    }

    /**
     * Checks if two doubles are equal within epsilon tolerance
     *
     * @param a First value
     * @param b Second value
     * @param epsilon Tolerance (default: 1e-9)
     * @return True if values are equal within epsilon
     */
    static bool floatEqual(double a, double b, double epsilon = 1e-9) {
        return std::abs(a - b) < epsilon;
    }

    /**
     * Linear interpolation between two values
     *
     * @param start Start value
     * @param end End value
     * @param amount Interpolation amount (0.0 to 1.0)
     * @return Interpolated value
     */
    template <typename T>
    static T lerp(T start, T end, float amount) {
        return start + (end - start) * amount;
    }

    /**
     * Normalize a value from a range to 0-1
     *
     * @param value Value to normalize
     * @param min Minimum of range
     * @param max Maximum of range
     * @return Normalized value (0-1)
     */
    template <typename T>
    static float normalize(T value, T min, T max) {
        if (max == min) {
            return 0.0f;
        }
        return static_cast<float>(value - min) / static_cast<float>(max - min);
    }
};

}  // namespace orgb::core


#endif  // ORGB_CORE_MATH_UTILS_HPP
