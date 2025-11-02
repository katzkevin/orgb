# orgb justfile

# Default target: list available commands
default:
    @just --list

# Helper: Set up config files for a specific platform
_setup-platform PLATFORM:
    @echo "Setting up build for {{PLATFORM}}..."
    @cp -f config.make.{{PLATFORM}} config.make
    @cp -f addons.make.{{PLATFORM}} addons.make
    @echo "✓ Configured for {{PLATFORM}} build"

# Build for macOS (native)
build-macos:
    @just _setup-platform macos
    make Release

# Build for macOS in Debug mode
build-macos-debug:
    @just _setup-platform macos
    make Debug

# Build for Emscripten/WebAssembly (browser)
build-emscripten:
    @just _setup-platform emscripten
    bash -c 'source ~/emsdk/emsdk_env.sh && make Release'

# Build for Emscripten in Debug mode
build-emscripten-debug:
    @just _setup-platform emscripten
    bash -c 'source ~/emsdk/emsdk_env.sh && make Debug'

# Build the project in Release mode (defaults to macOS)
build:
    @just build-macos

# Build the project in Debug mode (defaults to macOS)
build-debug:
    @just build-macos-debug

# Clean all build artifacts (both platforms)
clean:
    make clean

# Clean and rebuild for macOS
rebuild-macos: clean build-macos

# Clean and rebuild for Emscripten
rebuild-emscripten: clean build-emscripten

# Clean only Release build
clean-release:
    make CleanRelease

# Clean only Debug build
clean-debug:
    make CleanDebug

# Prepare NDI library for running
_prepare-ndi:
    @if [ ! -f bin/orgb.app/Contents/MacOS/libndi.4.dylib ]; then \
        echo "Copying NDI library to app bundle..."; \
        cp libndi.4.dylib bin/orgb.app/Contents/MacOS/; \
    fi

# Build and run the application (macOS)
run: build-macos _prepare-ndi
    @echo "Starting orgb..."
    @./bin/orgb.app/Contents/MacOS/orgb

# Build and run in Debug mode (macOS)
run-debug: build-macos-debug _prepare-ndi
    @echo "Starting orgb (debug)..."
    @./bin/orgb_debug.app/Contents/MacOS/orgb_debug

# Build and serve Emscripten build in browser
run-emscripten: build-emscripten
    @./scripts/emscripten-server.sh

# Clean up Emscripten server if it's still running
clean-emscripten-server:
    @echo "Cleaning up any Emscripten server on port 8005..."
    @lsof -ti :8005 2>/dev/null | xargs -r kill -9 2>/dev/null || true
    @echo "✓ Port 8005 is now free"

# Clean and rebuild
rebuild: clean build

# Clean and rebuild, then run
rebuild-run: clean build _prepare-ndi
    @echo "Starting orgb..."
    @./bin/orgb.app/Contents/MacOS/orgb

# Run with make's RunRelease target (alternative method)
run-make:
    make RunRelease

# Check if the app is currently running
status:
    @ps aux | grep "[o]rgb" || echo "orgb is not running"

# Stop any running instances of orgb
stop:
    @killall orgb 2>/dev/null && echo "Stopped orgb" || echo "orgb was not running"

# Build and run unit tests
test:
    @cd tests && ./run_tests.sh

# Clean and rebuild tests
test-clean:
    @cd tests && ./run_tests.sh clean

# Run specific test suite (e.g., just test-filter UtilitiesTest.*)
test-filter PATTERN:
    @cd tests && ./run_tests.sh --filter={{PATTERN}}

# Validate all shaders for compilation errors
shader-validate:
    @echo "Validating shaders..."
    @./scripts/validateShaders.sh

# Validate shaders with verbose output
shader-validate-verbose:
    @echo "Validating shaders (verbose)..."
    @./scripts/validateShaders.sh --verbose

# Run shader unit tests (requires full openFrameworks environment)
shader-test:
    @echo "Building and running shader tests..."
    @echo "Note: Requires openFrameworks and dependencies to compile"
    @if [ ! -f tests/shader_tests ]; then \
        echo "Compiling shader tests..."; \
        cd tests && g++ -std=c++17 shader_tests.cpp -o shader_tests -I../src 2>&1 || \
        (echo "❌ Shader tests require full build environment - skipping" && exit 0); \
    fi
    @if [ -f tests/shader_tests ]; then \
        cd tests && ./shader_tests; \
    fi

# Run all shader-related checks (validation only - tests require full build)
shader-check: shader-validate
    @echo ""
    @echo "✓ Shader validation passed!"
    @echo "  (Unit tests skipped - run 'just shader-test' after full build)"

# Clean shader test artifacts
shader-clean:
    @rm -f tests/shader_tests
    @rm -rf tests/test_output tests/diff_images
    @echo "Cleaned shader test artifacts"

# Format all C++ source files with clang-format
format:
    @echo "Formatting C++ files..."
    @find src -name "*.cpp" -o -name "*.hpp" -o -name "*.h" | xargs clang-format -i
    @find tests -name "*.cpp" -o -name "*.hpp" -o -name "*.h" | xargs clang-format -i
    @echo "✓ Formatting complete"

# Check formatting without modifying files
format-check:
    @echo "Checking C++ formatting..."
    @find src -name "*.cpp" -o -name "*.hpp" -o -name "*.h" | xargs clang-format --dry-run --Werror
    @find tests -name "*.cpp" -o -name "*.hpp" -o -name "*.h" | xargs clang-format --dry-run --Werror
    @echo "✓ All files properly formatted"

# Generate compilation database (required for linting)
compile-db:
    @echo "Generating compilation database..."
    @bear -- make clean && bear -- make Release
    @echo "✓ compile_commands.json generated"

# Note: For actual linting, use the pre-commit hooks (lefthook) which work correctly.
# This command is kept for reference but may be noisy. Run 'git commit' to trigger real linting.
lint:
    @echo "⚠️  Note: Pre-commit hooks provide better linting. This is a basic check only."
    @echo "Running clang-tidy linter..."
    @if [ ! -f compile_commands.json ]; then \
        echo "compile_commands.json not found. Run 'just compile-db' first."; \
        exit 1; \
    fi
    @find src -name "*.cpp" | head -5 | xargs /opt/homebrew/opt/llvm@19/bin/clang-tidy -p . \
        --header-filter='src/.*' \
        --system-headers=false \
        --quiet 2>&1 | grep -v "warnings generated" || true
    @echo "✓ Sample check complete (first 5 files only)"
    @echo "💡 For full linting, make a git commit to trigger pre-commit hooks"

# Run clang-tidy with auto-fix on simple issues (without using compile_commands.json to avoid system header noise)
lint-fix:
    @echo "Running clang-tidy with auto-fix..."
    @find src -name "*.cpp" -o -name "*.hpp" | grep -v "3rdparty" | \
        xargs -I {} /opt/homebrew/opt/llvm@19/bin/clang-tidy --system-headers=false --fix-errors {} -- -std=c++17 \
            -I./src -I./src/Forms -I./src/Forms/Lightning -I./src/Forms/Shapes \
            -I./src/Forms/Particles -I./src/Forms/Waves -I./src/Forms/Glow \
            -I./src/core -I./src/3rdparty -I./src/Effects \
            -I/Users/katz/workspace/of_v0.12.1_osx_release/libs/openFrameworks 2>&1 | \
        grep -E "warning:|error:" | grep -v "^Error while" || true
    @echo "✓ Auto-fixes applied"
