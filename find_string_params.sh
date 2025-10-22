#!/bin/bash
CLANG_TIDY="/opt/homebrew/opt/llvm@19/bin/clang-tidy"

echo "Finding string parameters that should be const reference..."

find src -type f \( -name "*.cpp" -o -name "*.hpp" \) | grep -v "src/3rdparty/" | while read file; do
    $CLANG_TIDY --checks="-*,performance-unnecessary-value-param" "$file" -- -std=c++23 \
        -I./src -I./src/Forms -I./src/Forms/Lightning -I./src/Forms/Shapes \
        -I./src/Forms/Particles -I./src/Forms/Waves -I./src/Forms/Glow \
        -I./src/core -I./src/3rdparty -I./src/Effects \
        -I/Users/katz/workspace/of_v0.12.1_osx_release/libs/openFrameworks \
        2>&1 | grep -E "(warning:|note:)"
done | grep -E "std::string|std::__1::basic_string" | head -30

