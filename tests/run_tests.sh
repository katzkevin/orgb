#!/bin/bash
#
# Convenience script to build and run ORGB unit tests
#
# Usage:
#   ./run_tests.sh              # Build and run all tests
#   ./run_tests.sh clean        # Clean build artifacts and rebuild
#   ./run_tests.sh --filter=*   # Run specific tests

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Parse arguments
CLEAN=false
FILTER=""

for arg in "$@"; do
    case $arg in
        clean)
            CLEAN=true
            ;;
        --filter=*)
            FILTER="${arg#*=}"
            ;;
        *)
            echo "Unknown argument: $arg"
            echo "Usage: $0 [clean] [--filter=TestPattern.*]"
            exit 1
            ;;
    esac
done

# Clean if requested
if [ "$CLEAN" = true ]; then
    echo -e "${YELLOW}Cleaning build directory...${NC}"
    rm -rf "$BUILD_DIR"
fi

# Create build directory
if [ ! -d "$BUILD_DIR" ]; then
    echo -e "${YELLOW}Creating build directory...${NC}"
    mkdir -p "$BUILD_DIR"
fi

cd "$BUILD_DIR"

# Configure with CMake
if [ ! -f "Makefile" ]; then
    echo -e "${YELLOW}Configuring with CMake...${NC}"
    cmake ..
fi

# Build tests
echo -e "${YELLOW}Building unit tests...${NC}"
cmake --build . --target unit-tests

# Check if build succeeded
if [ $? -ne 0 ]; then
    echo -e "${RED}Build failed!${NC}"
    exit 1
fi

echo -e "${GREEN}Build succeeded!${NC}"
echo ""

# Run tests
if [ -n "$FILTER" ]; then
    echo -e "${YELLOW}Running tests matching: $FILTER${NC}"
    ctest -L unit --output-on-failure -R "$FILTER"
else
    echo -e "${YELLOW}Running unit tests...${NC}"
    ctest -L unit --output-on-failure
fi

# Check test results
TEST_RESULT=$?

echo ""
if [ $TEST_RESULT -eq 0 ]; then
    echo -e "${GREEN}✓ All tests passed!${NC}"
else
    echo -e "${RED}✗ Some tests failed!${NC}"
    exit 1
fi
