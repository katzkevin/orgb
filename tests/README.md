# Test Suite

This directory contains the test suite for the orgb project, organized into unit tests and integration tests.

## Directory Structure

```
tests/
├── unit/                          # Pure logic tests (no GL context)
│   ├── test_flock.cpp            # Flocking behavior
│   ├── test_colorprovider.cpp    # Color palette generation
│   ├── test_press.cpp            # Press data structures
│   ├── test_keystate.cpp         # KeyState ADSR logic
│   ├── test_utilities.cpp        # Utility functions
│   └── CMakeLists.txt
│
├── integration/                   # GL-dependent tests
│   ├── fixtures/
│   │   ├── GLTestFixture.hpp     # GL context setup
│   │   └── FormTestFixture.hpp   # Form testing utilities
│   ├── test_forms_rendering.cpp  # Form rendering tests
│   ├── test_shader_compilation.cpp # Shader compilation tests
│   ├── test_postprocessing.cpp   # Post-processing pipeline tests
│   └── CMakeLists.txt
│
├── CMakeLists.txt                # Orchestrates all tests
└── README.md                     # This file
```

## Test Types

### Unit Tests (`tests/unit/`)

**Fast, lightweight tests** that verify business logic without requiring OpenGL context.

Tests include:
- Flock behavior calculations
- Color palette math (HSV interpolation, hue wrapping)
- KeyState ADSR envelope logic
- Press data structures
- Utility functions

**Run with:**
```bash
just test-unit
```

**Characteristics:**
- ✅ Fast execution (~seconds)
- ✅ No GL context required
- ✅ Can run in CI without display
- ✅ Ideal for TDD workflow

### Integration Tests (`tests/integration/`)

**Comprehensive tests** that verify rendering, shader compilation, and GL-dependent functionality.

Tests include:
- Form setup and rendering (all VisualForm types)
- Shader compilation and execution
- Post-processing pipeline
- FBO operations
- DrawManager integration
- Form switching and state management

**Run with:**
```bash
just test-integration
```

**Characteristics:**
- ⚠️ Slower execution (~10-30 seconds)
- ⚠️ Requires valid GL context
- ⚠️ May require display on macOS (can use Xvfb on Linux)
- ✅ Catches GL-specific bugs
- ✅ Verifies shaders actually compile

## Running Tests

### Quick Commands

```bash
# Run only unit tests (fast)
just test-unit

# Run only integration tests (requires GL)
just test-integration

# Run all tests
just test-all

# Run specific test by pattern
just test-filter "RadialParticles*"

# Run with verbose output
just test-verbose

# Clean and rebuild tests
just test-clean
```

See justfile for all available test commands.
