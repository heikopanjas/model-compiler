# BBFM Model Compiler Build Script
# Uses CMake with Ninja generator

$ErrorActionPreference = "Stop"  # Exit on error

$BUILD_DIR = "_build"

# Create build directory if it doesn't exist
if (-not (Test-Path $BUILD_DIR)) {
  Write-Host "Creating build directory: $BUILD_DIR"
  New-Item -ItemType Directory -Path $BUILD_DIR | Out-Null
}

# Configure with CMake and Ninja
Write-Host "Configuring with CMake..."
Set-Location $BUILD_DIR
cmake -G Ninja ..

# Build
Write-Host "Building with Ninja..."
ninja

Write-Host ""
Write-Host "Build complete! Executable: $BUILD_DIR/model-compiler.exe"
