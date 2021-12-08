#!/usr/bin/env sh

find lib example-app -name "*.cpp" -or -name "*.hpp" | xargs clang-format -i --verbose
