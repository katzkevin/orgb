#!/bin/bash
FILES=$(find src -name "*.hpp" -o -name "*.h")
for file in $FILES; do
  /opt/homebrew/opt/llvm@19/bin/clang-tidy -fix -checks=modernize-use-override "$file" 2>&1 > /dev/null
done
echo "Done adding override keywords"
