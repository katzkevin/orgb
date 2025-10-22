#!/bin/bash
# Run clang-tidy on all source files, one at a time to avoid crashes

CLANG_TIDY="/opt/homebrew/opt/llvm@19/bin/clang-tidy"
OUTPUT_FILE="clang_tidy_full_analysis.txt"

> "$OUTPUT_FILE"

echo "Running comprehensive clang-tidy analysis on all source files..."
echo "Results will be saved to $OUTPUT_FILE"
echo ""

total=$(find src -type f \( -name "*.cpp" -o -name "*.hpp" \) | wc -l | tr -d ' ')
count=0

find src -type f \( -name "*.cpp" -o -name "*.hpp" \) | sort | while read file; do
    count=$((count + 1))
    echo "[$count/$total] Analyzing $file..."
    echo "=================================" >> "$OUTPUT_FILE"
    echo "FILE: $file" >> "$OUTPUT_FILE"
    echo "=================================" >> "$OUTPUT_FILE"
    # Run clang-tidy with our config, but without compilation database
    # This will still catch many issues even without full compilation context
    $CLANG_TIDY --config-file=.clang-tidy "$file" -- -std=c++23 \
        -I./src -I./src/Forms -I./src/Forms/Lightning -I./src/Forms/Shapes \
        -I./src/Forms/Particles -I./src/Forms/Waves -I./src/Forms/Glow \
        -I./src/core -I./src/3rdparty -I./src/Effects \
        -I/Users/katz/workspace/of_v0.12.1_osx_release/libs/openFrameworks \
        2>&1 >> "$OUTPUT_FILE" || echo "  (analysis failed)" >> "$OUTPUT_FILE"
    echo "" >> "$OUTPUT_FILE"
done

echo ""
echo "Analysis complete! Filtering warnings..."
# Extract just the warnings (lines with "warning:" in them)
grep -E "(warning:|error:)" "$OUTPUT_FILE" | grep -v "^Error while processing" | sort | uniq > clang_tidy_warnings_summary.txt

echo "Summary saved to clang_tidy_warnings_summary.txt"
wc -l clang_tidy_warnings_summary.txt
