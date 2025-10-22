#!/bin/bash
# Analyze all source files and categorize warnings

CLANG_TIDY="/opt/homebrew/opt/llvm@19/bin/clang-tidy"
TEMP_OUTPUT="/tmp/clang_tidy_warnings_$$.txt"

echo "Analyzing all source files..."

find src -type f \( -name "*.cpp" -o -name "*.hpp" \) | while read file; do
    $CLANG_TIDY --config-file=.clang-tidy-analysis "$file" -- -std=c++23 \
        -I./src -I./src/Forms -I./src/Forms/Lightning -I./src/Forms/Shapes \
        -I./src/Forms/Particles -I./src/Forms/Waves -I./src/Forms/Glow \
        -I./src/core -I./src/3rdparty -I./src/Effects \
        -I/Users/katz/workspace/of_v0.12.1_osx_release/libs/openFrameworks \
        2>&1 | grep "warning:" | grep -v "^Error while"
done > "$TEMP_OUTPUT"

echo ""
echo "=== WARNING SUMMARY ==="
echo ""
echo "Total warnings: $(wc -l < "$TEMP_OUTPUT" | tr -d ' ')"
echo ""
echo "Top 20 warning types:"
sed 's/.*\[\(.*\)\]$/\1/' "$TEMP_OUTPUT" | sort | uniq -c | sort -rn | head -20
echo ""

# Save full output
cp "$TEMP_OUTPUT" clang_tidy_warnings.txt
echo "Full warnings saved to clang_tidy_warnings.txt"
rm "$TEMP_OUTPUT"
