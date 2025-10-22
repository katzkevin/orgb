#!/bin/bash
# Add explicit keyword to single-arg constructors

CLANG_TIDY="/opt/homebrew/opt/llvm@19/bin/clang-tidy"

# Get list of files
FILES=$(grep "google-explicit-constructor" clang_tidy_warnings_v3.txt | cut -d: -f1 | sort -u)

for file in $FILES; do
    echo "Fixing: $file"
    $CLANG_TIDY --fix --fix-errors \
        --checks="-*,google-explicit-constructor,hicpp-explicit-conversions" \
        --config-file=.clang-tidy \
        "$file" -- -std=c++23 \
        -I./src -I./src/Forms -I./src/Forms/Lightning -I./src/Forms/Shapes \
        -I./src/Forms/Particles -I./src/Forms/Waves -I./src/Forms/Glow \
        -I./src/core -I./src/3rdparty -I./src/Effects \
        -I/Users/katz/workspace/of_v0.12.1_osx_release/libs/openFrameworks \
        2>&1 | grep -E "(warning:|FIX-IT)" || true
done

echo "Done!"
