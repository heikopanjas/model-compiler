#!/bin/bash

# BBFM Model Compiler Build Script
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
cmake -G Ninja -B "$BUILD_DIR"

# Build
echo "Building with CMake..."
cmake --build "$BUILD_DIR"

echo ""
echo "Build complete! Executable: $BUILD_DIR/model-compiler"
