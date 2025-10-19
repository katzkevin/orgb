#ifndef ORGB_CORE_RANDOM_HPP
#define ORGB_CORE_RANDOM_HPP

#include <chrono>
#include <random>


namespace orgb::core {

/**
 * Random number generator using C++ <random>
 * Replacement for openFrameworks ofRandomuf()
 *
 * Thread-safe singleton pattern
 */
class Random {
   public:
    /**
     * Get uniform float in range [0, 1)
     * Equivalent to ofRandomuf()
     */
    static float uniform() { return uniformFloat(0.0f, 1.0f); }

    /**
     * Get uniform float in range [min, max)
     */
    static float uniformFloat(float min, float max) {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(getEngine());
    }

    /**
     * Get uniform int in range [min, max]
     */
    static int uniformInt(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(getEngine());
    }

    /**
     * Seed the random number generator
     * @param seed Seed value (use 0 for time-based seed)
     */
    static void seed(unsigned int seedValue = 0) {
        if (seedValue == 0) {
            // Use current time as seed
            seedValue = static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        }
        getEngine().seed(seedValue);
    }

    /**
     * Generate a random seed value (for creating deterministic random generators)
     */
    static unsigned int generateSeed() {
        return static_cast<unsigned int>(uniformFloat(0.0f, static_cast<float>(std::numeric_limits<unsigned int>::max())));
    }

   private:
    // Get the thread-local random engine
    static std::mt19937 & getEngine() {
        static thread_local std::mt19937 engine(
            static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
        return engine;
    }
};

} // namespace orgb::core


#endif  // ORGB_CORE_RANDOM_HPP
