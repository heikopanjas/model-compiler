---
name: cmake-build-commands
description: CMake commands for configuring, building, testing, and packaging a CMake-based project. Load when working with CMake build files or running a CMake build.
license: MIT
metadata:
  author: Heiko Panjas
  version: "1.0"
---

# CMake Build Commands

Read this skill when you need to configure, build, test, or deploy a CMake-based project.

---

## Build Commands

### Choose the Build Entry Point

- Prefer documented project commands, `CMakePresets.json`, or `cmake --preset` workflows when present.
- Do not assume a project-specific build script exists.
- Use the project's documented build directory if one is specified.
- If no build directory is documented, default to `_build`.
- Use Debug configuration for development unless the user asks for Release or packaging.
- For single-config generators, configure with `-DCMAKE_BUILD_TYPE=Debug`.
- For multi-config generators, build with `--config Debug`; `CMAKE_BUILD_TYPE` is ignored.

### Setup

```bash
# Check CMake version
cmake --version

# Check whether presets exist
ls CMakePresets.json
```

Install missing tools only when needed, based on the configure error. Common tools include CMake,
Ninja, a C++ compiler, package managers such as pkg-config, and project-specific generators.

### Preset Workflow

If `CMakePresets.json` exists, prefer the documented preset names:

```bash
cmake --list-presets
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

If preset names differ, use the names listed by `cmake --list-presets`.

### Configure and Build

```bash
# Default build directory when the project does not specify one
BUILD_DIR=_build

# Configure a single-config Debug build
cmake -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build "$BUILD_DIR"

# Build with verbose output
cmake --build "$BUILD_DIR" --verbose

# Build a specific target
cmake --build "$BUILD_DIR" --target target_name
```

Generate `compile_commands.json` when tools such as clangd, clang-tidy, or IDE integrations need it:

```bash
cmake -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

### Rebuild

Treat "rebuild" as clean plus build. Configure first if the build directory is missing or
unconfigured:

```bash
BUILD_DIR=_build

if [ ! -f "$BUILD_DIR/CMakeCache.txt" ]; then
    cmake -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug
fi

cmake --build "$BUILD_DIR" --target clean
cmake --build "$BUILD_DIR"
```

If the build directory exists but is stale or broken, remove it only after confirming it is a
generated build directory:

```bash
BUILD_DIR=_build

rm -rf "$BUILD_DIR"
cmake -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug
cmake --build "$BUILD_DIR"
```

### Tests

Use `ctest --test-dir` instead of changing directories so shell state stays predictable:

```bash
BUILD_DIR=_build

# Run all tests
ctest --test-dir "$BUILD_DIR"

# Run tests with verbose output
ctest --test-dir "$BUILD_DIR" --verbose

# Run tests matching a name or regex
ctest --test-dir "$BUILD_DIR" -R test_name
```

### Release, Install, and Package

```bash
BUILD_DIR=_build

# Single-config Release build
cmake -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD_DIR"

# Install, if configured
cmake --build "$BUILD_DIR" --target install

# Package, if configured
cpack --config "$BUILD_DIR/CPackConfig.cmake"
```

### Multi-Configuration Generators

```bash
BUILD_DIR=_build

# For Visual Studio, Xcode, or Ninja Multi-Config
cmake -B "$BUILD_DIR" -G "Ninja Multi-Config"

# Build Debug or Release explicitly
cmake --build "$BUILD_DIR" --config Debug
cmake --build "$BUILD_DIR" --config Release

# Test a specific configuration
ctest --test-dir "$BUILD_DIR" -C Debug
```

### Static Analysis and Formatting

Prefer project-defined targets when available:

```bash
BUILD_DIR=_build

cmake --build "$BUILD_DIR" --target help
cmake --build "$BUILD_DIR" --target clang-tidy
cmake --build "$BUILD_DIR" --target clang-format
```

If no formatting target exists, use the repository's documented formatting command. Do not apply
formatting across the tree unless the user requested it.
