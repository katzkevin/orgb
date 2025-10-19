#!/bin/bash

echo "========================================"
echo "CODE QUALITY ASSESSMENT FOR ORGB"
echo "========================================"
echo ""

# 1. File Statistics
echo "📊 CODEBASE SIZE"
echo "----------------------------------------"
echo "C++ Source Files: $(find src -name "*.cpp" | wc -l | xargs)"
echo "C++ Header Files: $(find src -name "*.hpp" -o -name "*.h" | wc -l | xargs)"
echo "Total Lines of Code: $(find src -name "*.cpp" -o -name "*.hpp" -o -name "*.h" | xargs wc -l | tail -1 | awk '{print $1}')"
echo ""

# 2. Current Warning Level
echo "⚙️  CURRENT BUILD FLAGS"
echo "----------------------------------------"
make -n 2>&1 | grep -oE '\-W[^ ]*' | sort -u
echo ""

# 3. Build Warnings
echo "⚠️  CURRENT BUILD WARNINGS"
echo "----------------------------------------"
WARNINGS=$(make -j4 2>&1 | grep "warning:" | grep -v "jobserver" | wc -l | xargs)
echo "Total warnings during build: $WARNINGS"
if [ "$WARNINGS" -gt 0 ]; then
    echo ""
    echo "Warning breakdown:"
    make -j4 2>&1 | grep "warning:" | grep -v "jobserver" | sed 's/.*warning://' | sort | uniq -c | sort -rn
fi
echo ""

# 4. Clang-Tidy Issues (sample)
echo "🔍 CLANG-TIDY ANALYSIS (Sample of 10 files)"
echo "----------------------------------------"
SAMPLE_FILES=$(find src -name "*.cpp" | head -10)
TIDY_OUTPUT=$(echo "$SAMPLE_FILES" | xargs /opt/homebrew/opt/llvm@19/bin/clang-tidy 2>&1)
TIDY_WARNINGS=$(echo "$TIDY_OUTPUT" | grep -c "warning:")
echo "Warnings found: $TIDY_WARNINGS"
echo ""
echo "Top issues:"
echo "$TIDY_OUTPUT" | grep "warning:" | sed 's/.*\[/[/' | sort | uniq -c | sort -rn | head -10
echo ""

# 5. Code Formatting
echo "✨ CODE FORMATTING STATUS"
echo "----------------------------------------"
UNFORMATTED=$(find src -name "*.cpp" -o -name "*.hpp" -o -name "*.h" | xargs clang-format --dry-run --Werror 2>&1 | grep -c "warning:" || echo 0)
echo "Files needing formatting: $UNFORMATTED"
echo ""

# 6. Missing Best Practices
echo "📋 BEST PRACTICES CHECK"
echo "----------------------------------------"
echo "Files with #include <stdio.h>: $(grep -r "#include <stdio.h>" src/ --include="*.hpp" --include="*.h" | wc -l | xargs)"
echo "Virtual destructors without override: $(grep -r "virtual ~" src/ --include="*.hpp" --include="*.h" | grep -v "override" | grep -v "= default" | wc -l | xargs)"
echo "Functions without override keyword: $(find src -name "*.hpp" -o -name "*.h" | xargs grep -E "^\s+void .*(KeyState|ColorProvider|DrawManager)" | grep -v "override" | grep -v "virtual" | wc -l | xargs)"
echo ""

