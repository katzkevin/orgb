# ORGB Testing Guide

## Overview

ORGB now has a comprehensive unit test suite using Google Test (gtest), covering core functionality including utilities, press lifecycle, keystate management, and color generation.

## Quick Start

```bash
# Run all tests
just test

# Clean and rebuild tests
just test-clean

# Run specific test suite
just test-filter "UtilitiesTest.*"
```

## Test Structure

```
tests/
├── CMakeLists.txt           # CMake configuration for googletest
├── README.md                # Detailed testing documentation
├── run_tests.sh             # Convenience build/run script
├── test_utilities.cpp       # Utilities function tests (40+ tests)
├── test_press.cpp           # Press lifecycle tests (25+ tests)
├── test_keystate.cpp        # KeyState management tests (35+ tests)
└── test_colorprovider.cpp   # ColorProvider tests (30+ tests)
```

## What's Tested

### ✅ Utilities (test_utilities.cpp)
- **CRITICAL FIX**: `kkDisableAlphaBlending()` bug fixed (was calling enable instead of disable)
- Modulo operations (Python-style wrapping)
- Coordinate transformations (polar/spherical)
- MIDI to Hz conversions
- Easing and transition functions
- Exponential mapping with power-law scaling
- Deterministic random number generation
- Environment variable helpers

### ✅ Press Lifecycle (test_press.cpp)
- Press construction and initialization
- Simple press and release
- Sustain pedal logic (4 different scenarios)
- Note percentage calculations
- **ADSR envelope calculations** (attack, decay, sustain, release)
- Instant attack/release edge cases
- Equality operators and hashing

### ✅ KeyState Management (test_keystate.cpp)
- Press tracking (active vs all)
- Cleanup with TTL
- Sustain pedal global behavior
- **Ephemeral presses** (guitar mode with decay)
- Chromatic grouping
- Meta input detection (C, C#, D, D#, E pattern)
- **Arousal and valence** (circumplex model)
- ADSR parameter management

### ✅ ColorProvider (test_colorprovider.cpp)
- HSV palette generation (monochrome, gradient)
- Clockwise/counter-clockwise hue progression
- Cyclical vs non-cyclical note mapping
- Opacity control
- Hue wrapping across boundaries
- Edge cases (black, white, grayscale)
- Custom palette support

## Running Tests

### From Command Line

```bash
# Navigate to tests directory
cd tests

# Build and run all tests
./run_tests.sh

# Clean build and run
./run_tests.sh clean

# Run specific tests
./run_tests.sh --filter="PressTest.ADSRDuringAttack"
./run_tests.sh --filter="UtilitiesTest.*"
```

### Using Just (Recommended)

```bash
# From project root
just test                           # Run all tests
just test-clean                     # Clean rebuild
just test-filter "KeyStateTest.*"  # Run specific suite
```

### Using CMake Directly

```bash
cd tests
mkdir build && cd build
cmake ..
make
./orgb_tests
```

## Test Coverage Summary

| Component | Tests | Coverage |
|-----------|-------|----------|
| Utilities | 40+ | Core math, MIDI, random |
| Press | 25+ | Lifecycle, sustain, ADSR |
| KeyState | 35+ | Tracking, ephemeral, arousal |
| ColorProvider | 30+ | Palettes, mapping, HSV |
| **Total** | **130+** | **Core functionality** |

## Critical Bugs Fixed

1. **src/Utilities.cpp:269** - `kkDisableAlphaBlending()` was calling `ofEnableAlphaBlending()` instead of `ofDisableAlphaBlending()`
   - Impact: High - rendering issues with alpha blending
   - Status: ✅ Fixed and tested

## CI/CD Integration

### GitHub Actions

ORGB uses GitHub Actions for continuous integration with three workflows:

#### 1. CI Workflow (`.github/workflows/ci.yaml`)

**Primary workflow** that runs on all PRs and pushes to main:
- ✅ Runs linting and formatting checks
- ✅ Runs full unit test suite
- ✅ Provides combined status check
- ⚡ Caches GoogleTest dependencies

**Triggers**: All pull requests and pushes to main

#### 2. Lint Workflow (`.github/workflows/lint.yaml`)

Standalone linting workflow:
- Checks C++ formatting with clang-format
- Validates code style (no trailing whitespace, tabs, etc.)
- Uploads format check results as artifacts

**Triggers**: PRs/pushes that modify C++ files or linting configuration

#### 3. Test Workflow (`.github/workflows/test.yaml`)

Standalone testing workflow:
- Builds and runs unit tests
- Generates XML test results
- Uploads test artifacts

**Triggers**: PRs/pushes that modify C++ files or test configuration

### Workflow Features

- **Platform**: macOS 14 (matches development environment)
- **Caching**: GoogleTest dependencies cached for faster builds
- **Artifacts**: Test results and format check logs retained for 7 days
- **Path Filtering**: Only runs when relevant files change
- **Parallel Jobs**: Lint and test jobs run concurrently

### Local Development vs CI

```bash
# Before pushing, verify CI will pass:
just check      # Run linting and formatting checks
just test       # Run unit tests

# Fix any issues:
just fix        # Apply auto-formatting
```

### CI Status Badges

Add to README.md:
```markdown
[![CI](https://github.com/YOUR_USERNAME/orgb/actions/workflows/ci.yaml/badge.svg)](https://github.com/YOUR_USERNAME/orgb/actions/workflows/ci.yaml)
[![Tests](https://github.com/YOUR_USERNAME/orgb/actions/workflows/test.yaml/badge.svg)](https://github.com/YOUR_USERNAME/orgb/actions/workflows/test.yaml)
[![Lint](https://github.com/YOUR_USERNAME/orgb/actions/workflows/lint.yaml/badge.svg)](https://github.com/YOUR_USERNAME/orgb/actions/workflows/lint.yaml)
```

### Manual CI Workflow Example

For custom CI/CD integration:

```yaml
- name: Install Dependencies
  run: brew install boost just cmake

- name: Run Linting
  run: just check

- name: Run ORGB Tests
  run: |
    cd tests
    mkdir build && cd build
    cmake ..
    make
    ./orgb_tests --gtest_output=xml:test_results.xml
```

## Known Limitations

1. **No integration tests** - MQTT, OSC, NDI, and GPU shader tests not yet implemented
2. **Timing-sensitive** - ADSR tests use `sleep_for()` and may be flaky on slow systems
3. **openFrameworks dependency** - Tests require full OF framework (could be mocked)
4. **No performance tests** - Frame rate under load not tested
5. **No memory leak detection** - Should integrate valgrind/ASAN

## Future Testing Priorities

### Short-term (Next Sprint)
- [ ] MQTT message parsing and validation tests
- [ ] OSC message handling tests
- [ ] Input validation tests for network messages

### Medium-term (1-2 Months)
- [ ] DrawManager shader pipeline tests
- [ ] Form initialization and switching tests
- [ ] Parameter serialization tests
- [ ] Performance benchmarks (particle systems, flocking)

### Long-term (3-6 Months)
- [ ] Integration tests with mock MQTT broker
- [ ] GPU shader compilation and execution tests
- [ ] Memory leak detection (valgrind)
- [ ] Thread safety tests for communication handlers
- [ ] End-to-end visualization tests (frame capture comparison)

## Best Practices

### When Adding New Features

1. **Write tests first** (TDD approach)
2. **Run existing tests** to ensure no regressions
3. **Aim for 80%+ coverage** on new code
4. **Document timing-sensitive tests** with comments

### Test Organization

```cpp
// Good: Descriptive test names
TEST_F(PressTest, SustainBeforeRelease) { ... }

// Bad: Vague test names
TEST_F(PressTest, Test1) { ... }
```

### Assertions

```cpp
// Use EXPECT_NEAR for floating-point comparisons
EXPECT_NEAR(value, expected, 0.001f);

// Use EXPECT_EQ for exact matches
EXPECT_EQ(note, 60);

// Provide context in failures
EXPECT_TRUE(condition) << "Expected sustain to be released at time " << time;
```

## Debugging Failed Tests

```bash
# Verbose output
./orgb_tests --gtest_verbose

# Single test with break on failure
./orgb_tests --gtest_filter=PressTest.ADSRDuringAttack --gtest_break_on_failure

# Use gdb
gdb ./orgb_tests
(gdb) run --gtest_filter=TestName
```

## Contributing

When submitting PRs:
1. Ensure all tests pass: `just test`
2. Add tests for new functionality
3. Update this document if adding new test categories
4. Include test results in PR description

## Questions?

See `tests/README.md` for detailed test documentation.
