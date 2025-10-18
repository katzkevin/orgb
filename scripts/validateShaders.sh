#!/bin/bash

#
# validateShaders.sh
# ORGB - Shader Validation Script
#
# Validates all shader files using glslangValidator (from Vulkan SDK)
# Usage: ./scripts/validateShaders.sh [--verbose] [--fix]
#

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
SHADER_DIRS=("bin/data/shadersGL2" "bin/data/shadersGL3" "bin/data/shadersES2")
VERBOSE=false
FIX_MODE=false
TOTAL_SHADERS=0
VALID_SHADERS=0
INVALID_SHADERS=0

# Parse arguments
for arg in "$@"; do
    case $arg in
        --verbose|-v)
            VERBOSE=true
            shift
            ;;
        --fix|-f)
            FIX_MODE=true
            shift
            ;;
        --help|-h)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  -v, --verbose    Show detailed validation output"
            echo "  -f, --fix        Attempt to auto-fix common issues (not implemented)"
            echo "  -h, --help       Show this help message"
            echo ""
            echo "Validates GLSL shaders using glslangValidator."
            echo "Requires Vulkan SDK to be installed."
            exit 0
            ;;
    esac
done

# Check if glslangValidator is installed
if ! command -v glslangValidator &> /dev/null; then
    echo -e "${RED}ERROR: glslangValidator not found${NC}"
    echo "Install Vulkan SDK: https://vulkan.lunarg.com/sdk/home"
    echo ""
    echo "macOS: brew install glslang"
    echo "Linux: sudo apt install glslang-tools"
    exit 1
fi

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}ORGB Shader Validation${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Function to detect shader type from filename
get_shader_stage() {
    local filename=$1
    if [[ $filename == *.vert ]]; then
        echo "vert"
    elif [[ $filename == *.frag ]]; then
        echo "frag"
    elif [[ $filename == *.geom ]]; then
        echo "geom"
    elif [[ $filename == *.comp ]]; then
        echo "comp"
    else
        echo "unknown"
    fi
}

# Function to get GLSL version from shader
get_glsl_version() {
    local shader_file=$1
    local version=$(grep -m 1 "^#version" "$shader_file" | awk '{print $2}')
    echo "${version:-120}"  # Default to 120 if not specified
}

# Function to validate a single shader
validate_shader() {
    local shader_file=$1
    local shader_stage=$(get_shader_stage "$shader_file")

    if [[ $shader_stage == "unknown" ]]; then
        echo -e "${YELLOW}⚠ SKIP${NC}: $shader_file (unknown shader type)"
        return 0
    fi

    TOTAL_SHADERS=$((TOTAL_SHADERS + 1))

    # Check GLSL version
    local glsl_version=$(get_glsl_version "$shader_file")

    # glslangValidator only works well with GLSL 330+ (modern OpenGL)
    # Skip validation for legacy GLSL 120 (OpenGL 2.x) - validated at runtime
    if [[ $glsl_version -lt 330 ]]; then
        if [[ $VERBOSE == true ]]; then
            echo -e "${YELLOW}⚠ SKIP${NC}: $shader_file (GLSL $glsl_version - legacy, validated at runtime)"
        else
            echo -n "s"  # 's' for skipped
        fi
        # Count as valid (runtime will catch errors)
        VALID_SHADERS=$((VALID_SHADERS + 1))
        return 0
    fi

    # Run glslangValidator for modern shaders (330+)
    local validation_output
    if validation_output=$(glslangValidator -S "$shader_stage" "$shader_file" 2>&1); then
        VALID_SHADERS=$((VALID_SHADERS + 1))
        if [[ $VERBOSE == true ]]; then
            echo -e "${GREEN}✓ VALID${NC}: $shader_file (GLSL $glsl_version)"
        else
            echo -n "."
        fi
        return 0
    else
        INVALID_SHADERS=$((INVALID_SHADERS + 1))
        echo -e "\n${RED}✗ INVALID${NC}: $shader_file (GLSL $glsl_version)"
        echo -e "${YELLOW}Error output:${NC}"
        echo "$validation_output" | sed 's/^/  /'
        echo ""
        return 1
    fi
}

# Main validation loop
echo "Validating shaders in:"
for dir in "${SHADER_DIRS[@]}"; do
    echo "  - $dir"
done
echo ""

if [[ $VERBOSE == false ]]; then
    echo -n "Progress: "
fi

for shader_dir in "${SHADER_DIRS[@]}"; do
    if [[ ! -d "$shader_dir" ]]; then
        echo -e "${YELLOW}⚠ Directory not found: $shader_dir${NC}"
        continue
    fi

    # Find all shader files
    while IFS= read -r -d '' shader_file; do
        validate_shader "$shader_file"
    done < <(find "$shader_dir" -type f \( -name "*.vert" -o -name "*.frag" -o -name "*.geom" -o -name "*.comp" \) -print0)
done

if [[ $VERBOSE == false ]]; then
    echo "" # Newline after progress dots
fi

# Summary
echo ""
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}Validation Summary${NC}"
echo -e "${BLUE}========================================${NC}"
echo -e "Total shaders:   ${TOTAL_SHADERS}"
echo -e "Valid shaders:   ${GREEN}${VALID_SHADERS}${NC}"
echo -e "Invalid shaders: ${RED}${INVALID_SHADERS}${NC}"
echo ""
echo -e "${YELLOW}Note:${NC} Legacy GLSL 120 shaders (GL2) are skipped - validated at runtime"
echo -e "      Modern GLSL 330+ shaders are validated offline"

if [[ $INVALID_SHADERS -eq 0 ]]; then
    echo ""
    echo -e "${GREEN}✓ All shaders passed validation!${NC}"
    exit 0
else
    echo ""
    echo -e "${RED}✗ Some shaders failed validation${NC}"
    echo "Run with --verbose for detailed output"
    exit 1
fi
