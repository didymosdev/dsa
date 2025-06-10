#!/bin/zsh
# Apply clang-format to all C/C++ source and header files recursively
find . -type f \( -name "*.cpp" -o -name "*.cc" -o -name "*.h" \) -exec clang-format -i {} +
