#!/bin/bash

# P3 Compiler Build Script
# Uses CMake with Ninja generator

set -e  # Exit on error

BUILD_DIR="_build"

# Create build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating build directory: $BUILD_DIR"
    mkdir -p "$BUILD_DIR"
fi

# Configure with CMake and Ninja
echo "Configuring with CMake..."
cd "$BUILD_DIR"
cmake -G Ninja ..

# Build
echo "Building with Ninja..."
ninja

echo ""
echo "Build complete! Executable: $BUILD_DIR/p3c"
