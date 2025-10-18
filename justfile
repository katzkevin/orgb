# orgb justfile

# Default target: list available commands
default:
    @just --list

# Build the project in Release mode
build:
    make Release

# Build the project in Debug mode
build-debug:
    make Debug

# Clean all build artifacts
clean:
    make clean

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

# Build and run the application
run: build _prepare-ndi
    @echo "Starting orgb..."
    @./bin/orgb.app/Contents/MacOS/orgb

# Build and run in Debug mode
run-debug: build-debug _prepare-ndi
    @echo "Starting orgb (debug)..."
    @./bin/orgb_debug.app/Contents/MacOS/orgb_debug

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
