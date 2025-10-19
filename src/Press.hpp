//
//  press.hpp
//  firstSketch
//
//  Created by Kevin Katz on 9/23/21.
//

#ifndef press_hpp
#define press_hpp

#include <stdio.h>

#include <boost/optional.hpp>
#include <iostream>
#include <string>

#include "ofMain.h"

class Press {
   public:
    enum PressType { PIANO = 0, GUITAR = 1 };

    unsigned int id;
    int note;
    float velocityPct;
    double tSystemTimeSeconds;
    PressType pressType;

    Press(int n, float vPct, double pressTime, PressType pt, unsigned int messageId);

    [[nodiscard]] boost::optional<double> getReleaseTime() const;
    void setReleased(double time);

    void setSustained(double sustainTimeS);
    void releaseSustain(double sustainReleasedTimeS);

    [[nodiscard]] double audibleAmplitudePct(double attackTimeS, double decayTimeS, double sustainLevelPct,
                                              double releaseTimeS) const;

    // Percent across its entire range
    [[nodiscard]] float noteOverallPct() const;

    // Percent across A-G#
    [[nodiscard]] float noteChromaticPct() const;

    bool operator<(const Press & other) const { return id < other.id; }

    friend std::ostream & operator<<(std::ostream & os, const Press & p);

    // Allows using the Press as a key in a unordered_map (also see defined hash below)
    // https://stackoverflow.com/a/17017281
    bool operator==(const Press & other) const { return (id == other.id); }

   private:
    boost::optional<double> sustainReleasedTimeS;
    boost::optional<double> sustainTimeS;

    // This should be accessed through getReleaseTime, which incorporates sustains
    boost::optional<double> t_released;
};

// https://stackoverflow.com/a/1102720/1371634
// Allows using the Press as a key in a map
namespace std {
template <>
struct less<Press> {
    bool operator()(const Press & lhs, const Press & rhs) const { return lhs.id < rhs.id; }
};

// https://stackoverflow.com/a/17017281
// Allows using the Press as a key in an unordered_map

template <>
struct hash<Press> {
    std::size_t operator()(const Press & k) const {
        return (std::hash<unsigned int>()(k.id));  // Note, this depends on press ID being unique.
    }
};

}  // namespace std

#endif /* press_hpp */
