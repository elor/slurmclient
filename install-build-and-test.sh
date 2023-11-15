#!/bin/bash
#
# This script installs the dependencies, builds the project and runs the tests.

set -e -u

mkdir -p build

cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build
(
cd build
ctest || ctest --rerun-failed --output-on-failure
)

