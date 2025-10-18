# ORGB Unit Tests

Comprehensive unit test suite for the ORGB audio-reactive visualization engine using Google Test.

## Overview

This test suite covers the core components of ORGB:

- **Utilities** - Mathematical functions, coordinate transformations, MIDI conversions
- **Press** - Note press lifecycle, sustain pedal logic, ADSR envelope calculations
- **KeyState** - Press tracking, cleanup, ephemeral presses, arousal/valence
- **ColorProvider** - HSV palette generation, color mapping from MIDI notes

## Prerequisites

- CMake 3.14 or higher
- C++17 compatible compiler
- openFrameworks v0.12.1
- Boost library
- Internet connection (for initial googletest download)

## Building and Running Tests

### Quick Start

From the `tests/` directory:

```bash
# Create build directory
mkdir -p build && cd build

# Configure with CMake
cmake ..

# Build tests
make

# Run all tests
./orgb_tests

# Run with verbose output
./orgb_tests --gtest_verbose
```

### Running Specific Tests

```bash
# Run only Utilities tests
./orgb_tests --gtest_filter=UtilitiesTest.*

# Run only Press tests
./orgb_tests --gtest_filter=PressTest.*

# Run only KeyState tests
./orgb_tests --gtest_filter=KeyStateTest.*

# Run only ColorProvider tests
./orgb_tests --gtest_filter=ColorProviderTest.*

# Run a specific test
./orgb_tests --gtest_filter=UtilitiesTest.PositiveModuloIntNegative
```

### Alternative: Using run_tests.sh Script

A convenience script is provided:

```bash
# From the tests/ directory
chmod +x run_tests.sh
./run_tests.sh

# Clean and rebuild
./run_tests.sh clean

# Run specific test suite
./run_tests.sh --filter=UtilitiesTest.*
```

## Test Coverage

### Utilities Tests (40+ tests)

- **Modulo operations**: Python-style wrapping for positive and negative values
- **Coordinate transforms**: Polar/spherical to rectangular conversions
- **MIDI conversions**: Note to Hz, relative frequencies, octave relationships
- **Easing functions**: Cubic, exponential, and custom transition curves
- **Exponential mapping**: Power-law scaling with clamping
- **Gain/kurtosis**: Circumplex model modulation
- **Deterministic randomness**: Reproducible pseudo-random values
- **MIDI type parsing**: String to enum conversions
- **Environment variables**: Helpers with defaults and optional values

**Critical bug fix tested**: `kkDisableAlphaBlending()` now correctly disables blending

### Press Tests (25+ tests)

- **Construction**: Proper initialization of note, velocity, time, type
- **Simple lifecycle**: Press and release tracking
- **Sustain pedal logic**:
  - Sustain before key release
  - Sustain after key release
  - Sustain released before key release
  - Multiple sustain cycles
- **Note calculations**: Overall and chromatic percentage mappings
- **ADSR envelope**:
  - Attack phase behavior
  - Instant attack (0 time)
  - Decay to sustain
  - Release phase
  - Zero release time
  - Full envelope completion
- **Equality and hashing**: Proper comparison and hash functions

### KeyState Tests (35+ tests)

- **Press tracking**: Adding, retrieving, releasing presses
- **Active vs all presses**: Distinction between held and released
- **Cleanup mechanism**: TTL-based removal of old presses
- **Sustain pedal**:
  - Global sustain on/off
  - Effect on new presses
  - Batch release on sustain off
- **Ephemeral presses** (guitar mode):
  - Creation and update
  - Velocity decay over time
  - New note detection on large velocity change
  - Pruning below threshold
  - Map-based batch updates
- **Chromatic grouping**: Group presses by note (mod 12)
- **Meta input detection**: Sequential semitone pattern (C, C#, D, D#, E)
- **Arousal and valence**:
  - Get/set circumplex coordinates
  - Gain calculation with kurtosis
  - Symmetric behavior at extremes
- **ADSR parameters**: Initialization and modification

### ColorProvider Tests (30+ tests)

- **Palette generation**:
  - Monochrome (same HSV)
  - Clockwise hue progression
  - Counter-clockwise hue progression
  - Saturation and value gradients
- **Color mapping**:
  - Cyclical mode (octaves have same color)
  - Non-cyclical mode (linear mapping)
  - Full chromatic scale (12 distinct hues)
- **Opacity control**: Full, half, zero, RGB preservation
- **Hue wrapping**: Across 0/255 boundary in both directions
- **Edge cases**:
  - Black palette (value = 0)
  - White palette (saturation = 0, value = 255)
  - Grayscale (saturation = 0)
  - Extreme MIDI notes (0 and 127)
- **Parameter modification**: Bounds checking, toggle states
- **Custom palettes**: User-defined color lists

## Test Assertions

Tests use Google Test assertions:

- `EXPECT_EQ/NE`: Equality comparisons
- `EXPECT_LT/GT/LE/GE`: Relational comparisons
- `EXPECT_NEAR`: Floating-point comparisons with epsilon
- `EXPECT_TRUE/FALSE`: Boolean checks
- `EXPECT_FLOAT_EQ/DOUBLE_EQ`: Exact floating-point equality (use sparingly)

## Continuous Integration

To integrate with CI/CD:

```yaml
# Example GitHub Actions workflow
- name: Build and run tests
  run: |
    cd tests
    mkdir build && cd build
    cmake ..
    make
    ./orgb_tests --gtest_output=xml:test_results.xml
```

## Known Limitations

1. **openFrameworks dependency**: Some tests require ofMain.h which pulls in the entire OF framework
2. **Timing-sensitive tests**: ADSR tests use `sleep_for()` and may be flaky on slow systems
3. **Random number tests**: Deterministic tests only; true randomness not tested
4. **Integration tests missing**: No end-to-end MQTT, OSC, or GPU shader tests yet

## Future Test Additions

- [ ] MQTT message parsing and validation
- [ ] OSC message handling
- [ ] DrawManager shader pipeline
- [ ] Form initialization and switching
- [ ] Parameter serialization/deserialization
- [ ] Performance benchmarks (frame rate under load)
- [ ] Memory leak detection (valgrind integration)
- [ ] Thread safety tests for communication handlers

## Debugging Failed Tests

### Enable verbose output
```bash
./orgb_tests --gtest_verbose
```

### Run single test with detailed failure info
```bash
./orgb_tests --gtest_filter=UtilitiesTest.MidiToHzA440 --gtest_break_on_failure
```

### Use gdb
```bash
gdb ./orgb_tests
(gdb) run --gtest_filter=PressTest.ADSRDuringAttack
```

### Check test build configuration
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
make VERBOSE=1
```

## Contributing

When adding new features to ORGB, please:

1. Write tests first (TDD approach)
2. Ensure all existing tests still pass
3. Aim for >80% code coverage on new code
4. Document any timing-sensitive or platform-specific behavior

## License

Same as ORGB project.
