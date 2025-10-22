#!/bin/bash
# Automatically fix uninitialized variables using clang-tidy

CLANG_TIDY="/opt/homebrew/opt/llvm@19/bin/clang-tidy"

echo "Fixing uninitialized variables..."

# Get list of files with init-variables warnings
FILES=$(grep "cppcoreguidelines-init-variables" clang_tidy_warnings_v3.txt | cut -d: -f1 | sort -u)

for file in $FILES; do
    echo "Fixing: $file"
    $CLANG_TIDY --fix --fix-errors \
        --checks="-*,cppcoreguidelines-init-variables" \
        --config-file=.clang-tidy \
        "$file" -- -std=c++23 \
        -I./src -I./src/Forms -I./src/Forms/Lightning -I./src/Forms/Shapes \
        -I./src/Forms/Particles -I./src/Forms/Waves -I./src/Forms/Glow \
        -I./src/core -I./src/3rdparty -I./src/Effects \
        -I/Users/katz/workspace/of_v0.12.1_osx_release/libs/openFrameworks \
        2>&1 | grep -E "(warning:|error:|note:|fixed)" || true
done

echo "Done!"
