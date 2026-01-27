#!/bin/sh
cd "$(dirname "$0")"

# Find all C++ source files, pruning any directory named 'build' or '.venv'
find . \( -name "build" -o -name ".venv" \) -prune -o -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.h" -o -name "*.c" -o -name "*.cc" -o -name "*.cxx" \) -print0 | xargs -0 clang-format -i
