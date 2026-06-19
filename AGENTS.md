# Project Instructions for AI Coding Agents

**Last updated:** 2026-06-19 (shared library ABI rationale)

<!-- {preamble} -->

# ⚠️ Before You Start

Run `/init-session` at the beginning of each new session, OR read this entire file before proceeding.

**DO NOT** make code changes or commits until you have done one of the above.

<!-- {mission} -->

## Mission Statement

Model Compiler is a C++23 compiler for the Big Bad Feed Machine modeling DSL. It lets developers
describe podcast-domain object models, relationships, invariants, computed features, aliases, and
namespaces in `.fm` files, then validates those models and generates target-language source code.

The project currently implements parsing, semantic analysis, and C++ code generation, with additional
target languages such as Rust evolving alongside the core compiler infrastructure.

## Technology Stack

- **Primary Language:** C++23, with C++17 as the minimum compatibility target
- **Domain Language:** BBFM `.fm` modeling DSL
- **Build System:** CMake 3.20+ with Ninja recommended for local builds
- **Parser Toolchain:** Flex lexer and Bison parser
- **CLI Library:** cxxopts
- **Runtime Libraries:** Standard C++ library plus platform UUID support for `Guid`
- **Testing:** CTest with plain C++ test executables
- **Version Control:** Git
- **License:** MIT

## Session Protocol

When starting a new session, read this entire file and confirm you have
understood the project instructions before proceeding. Summarize the project
purpose and key conventions briefly. Do not make changes until you have
confirmed your understanding.

<!-- {principles} -->

## Primary Instructions

- Avoid making assumptions. If you need additional context to accurately answer the user, ask the user for the missing information. Be specific about which context you need.
- Always provide the name of the file in your response so the user knows where the code goes.
- Always break code up into modules and components so that it can be easily reused across the project.
- All code you write MUST be fully optimized. ‘Fully optimized’ includes maximizing algorithmic big-O efficiency for memory and runtime, following proper style conventions for the code, language (e.g. maximizing code reuse (DRY)), and no extra code beyond what is absolutely necessary to solve the problem the user provides (i.e. no technical debt). If the code is not fully optimized, you will be fined $100.

### Working Together

This file (`AGENTS.md`) is the primary instructions file for AI coding assistants working on this project. Agent-specific instruction files (such as `.github/copilot-instructions.md`, `CLAUDE.md`) reference this document, maintaining a single source of truth.

When initializing a session or analyzing the workspace, refer to instruction files in this order:

1. `AGENTS.md` (this file - primary instructions and single source of truth)
2. Agent-specific reference file (if present - points back to AGENTS.md)

**Skills in `.agents/skills/` are authoritative for their domain.** When a skill
and this file disagree, follow the skill. Load a skill only when starting work
in that domain — do not preload skills listed elsewhere in this file.

### Update Protocol (CRITICAL)

**PROACTIVELY update this file (`AGENTS.md`) as we work together.** Whenever you make a decision, choose a technology, establish a convention, or define a standard, you MUST update AGENTS.md immediately in the same response.

**Update ONLY this file (`AGENTS.md`)** when coding standards, conventions, or project decisions evolve. Do not modify agent-specific reference files unless the reference mechanism itself needs changes.

**Put detailed conventions in skills**, not in this file. When a skill-owned rule changes, update the skill file and log the decision here in Recent Updates.

**When to update** (do this automatically, without being asked):

- Technology choices (build tools, languages, frameworks)
- Directory structure decisions
- Coding conventions and style guidelines
- Architecture decisions
- Naming conventions
- Build/test/deployment procedures

**How to update AGENTS.md:**

- Maintain the "Last updated" timestamp at the top
- Add content to the relevant section (Project Overview, Coding Standards, etc.)
- Add entries to the "Recent Updates & Decisions" log at the bottom with:
  - Date (with time if multiple updates per day)
  - Brief description
  - Reasoning for the change
- Preserve this structure: title header → timestamp → main instructions → "Recent Updates & Decisions" section

## Best Practices

### When Updating This Repository

1. **Maintain Consistency**: Keep code style consistent across the codebase
2. **Test First**: Write tests before implementing features when applicable
3. **Document Changes**: Update documentation when changing functionality
4. **Code Review**: [Describe your code review process]
5. **Date Changes**: Update the "Last updated" timestamp in this file when making changes
6. **Log Updates**: Add entries to "Recent Updates & Decisions" section below

### Security & Safety

- Never include API keys, tokens, or credentials in code
- Always require explicit human confirmation before commits
- [Add project-specific security guidelines]

### Testing

Load `cpp-testing-conventions` before writing, reviewing, or refactoring C++ tests.

### Documentation

Load `cpp-documentation-conventions` before writing, reviewing, or updating C++ documentation.

<!-- {languages} -->

## C++ Coding Standards

Load `cpp-coding-conventions` before writing, reviewing, or refactoring C++ code.

## Build Commands

Load `cmake-build-commands` when configuring, building, testing, formatting, or packaging the project.

## Shared Library ABI

The compiler executable, `compiler-runtime`, and generator plugins are separate shared-library boundaries (`.dylib` / `.so` / `.dll`). **STL types must not appear in APIs that cross those boundaries.** `std::string`, `std::vector`, `std::map`, and similar standard-library types have no stable ABI across compiler versions, C++ runtimes, or independently built plugin binaries.

**Policy:**

- **Plugin exports** — C-compatible types only (`const char*`, raw pointers, `size_t`) plus PIMPL runtime types (`runtime::CapabilitiesDictionary`, `runtime::ICodeGenerator*`). See `GeneratorPlugin.h`.
- **Shared compiler model** — `runtime::String`, `runtime::Array<T>`, and `runtime::Dictionary<K,V>` in `compiler-runtime` so the host and plugins share AST, semantic analysis, and generator interfaces without STL in headers they both include.
- **Implementation internals** — STL may remain inside PIMPL bodies, parser/lexer code, iostream usage, and CLI parsing (cxxopts) that never crosses a DLL interface.

<!-- {integration} -->

## Semantic Versioning

Automatically bump the project version after every code change and include it in the same commit. Load `semantic-versioning` when deciding PATCH/MINOR/MAJOR version bumps.

## Commit Protocol

- **NEVER commit automatically** — always wait for explicit user confirmation
- Whenever the user asks you to commit, read and follow `git-workflow` in full before staging or committing
- Skill rules override any summary in this file or in slash commands such as `/init-session`

---

<!-- {changelog} -->

## Recent Updates & Decisions

### 2026-06-19

- **Shared Library ABI Rationale**: Documented that STL types cannot guarantee a stable ABI across DLL/dylib interfaces; this is the primary reason for `runtime::String`/`Array`/`Dictionary`, PIMPL runtime types, and C-compatible plugin exports
- **Version Bump**: Updated the visible project version to `v4.1.1`
- **Reasoning**: Makes the architectural constraint explicit for future contributors and agents

- **Runtime Containers in Compiler**: Replaced `std::string`, `std::vector`, and `std::map` with `runtime::String`, `runtime::Array<T>`, and `runtime::Dictionary<K,V>` throughout compiler source (AST, semantic analyzer, driver, generators, parser); `std::set` and STL at CLI/iostream boundaries remain; C++ generator emits `runtime::String` and `runtime::Array` for modeled collection types
- **Version Bump**: Updated the visible project version to `v4.1.0`
- **Reasoning**: STL containers cannot cross shared-library boundaries with a stable ABI; runtime types keep the host/plugin contract and compiler internals aligned with generated code

- **Runtime Include Guard Prefix**: Standardized all `include/runtime/` headers on `__RUNTIME_<NAME>_H_INCL__` guards
- **Version Bump**: Updated the visible project version to `v4.0.1`
- **Reasoning**: Aligns runtime header guards with the directory layout and avoids collisions with top-level include names

- **Model Namespace Removed**: Dropped the outer `model` C++ namespace; compiler and plugin types are now at global scope and runtime types use the top-level `runtime` namespace (`runtime::AST`, `runtime::Fabric`, etc.) instead of `model::runtime::…`
- **Version Bump**: Updated the visible project version to `v4.0.0`
- **Reasoning**: Removes redundant namespace nesting from compiler internals while keeping FM-declared domain namespaces in generated output unchanged

- **Include Guard Prefix Removed**: Dropped the `MODEL_` segment from include guards; format is now `__CLASS_NAME_H_INCL__` (generated headers use `__GENERATED_…`) instead of `__MODEL_…`
- **Version Bump**: Updated the visible project version to `v3.0.1`
- **Reasoning**: Removes redundant project-prefix branding from guard macros while keeping the `_H_INCL__` suffix convention

- **Runtime Namespace Consolidation**: Moved all types under `include/runtime/` into the `runtime` namespace, including `AST`, `SemanticAnalyzer`, `ICodeGenerator`, `Array`, and `Dictionary`; compiler and plugin code now references them as `runtime::…` or `runtime::…` within `namespace model`
- **Version Bump**: Updated the visible project version to `v3.0.0`
- **Reasoning**: Unifies compiler-runtime and generated-code runtime types under one namespace and marks the removed top-level `` runtime symbols as a breaking API change

- **ICodeGenerator in Runtime**: Moved `ICodeGenerator` header into `include/runtime/` and source into `src/runtime/`
- **Version Bump**: Updated the visible project version to `v2.0.4`
- **Reasoning**: Keeps the generator interface alongside other compiler-runtime APIs consumed by generator plugins

- **AST and SemanticAnalyzer in Runtime**: Moved `AST` and `SemanticAnalyzer` headers into `include/runtime/` and sources into `src/runtime/` alongside other compiler-runtime components
- **Version Bump**: Updated the visible project version to `v2.0.3`
- **Reasoning**: Treats the validated model and semantic analysis APIs as part of the shared compiler runtime consumed by generator plugins

- **Runtime Header Layout**: Moved `Array.h` and `Dictionary.h` into `include/runtime/` alongside the other runtime headers; all runtime implementation files remain in `src/runtime/`
- **Version Bump**: Updated the visible project version to `v2.0.2`
- **Reasoning**: Keeps all runtime public headers under one directory and matches the existing `runtime/` include path used by generated code and tests

- **Generator Source Layout**: Moved self-contained generator plugin implementations into `src/generators/c++/`, `src/generators/rust/`, and `src/generators/swift/` with private headers colocated beside each `.cpp`; public `include/` no longer lists generator headers
- **Version Bump**: Updated the visible project version to `v2.0.1`
- **Reasoning**: Keeps generator plugins as isolated implementation units while preserving the public header directory for shared compiler and runtime APIs

- **BBFM Prefix Removed**: Dropped the `BBFM` prefix from source identifiers: `bbfm::` namespace is now ``, include guards initially used `__MODEL_`, plugin macros are `PLUGIN_EXPORT` and `GENERATOR_PLUGIN_API_VERSION`, and generated C++ uses `runtime::` types
- **Version Bump**: Updated the visible project version to `v2.0.0`
- **Reasoning**: Removes redundant project-prefix branding from code identifiers and generated output while keeping the public runtime and plugin APIs consistent under the `model` namespace

- **Runtime Source Layout**: Moved runtime type implementation files (`Fabric`, `Guid`, `String`, `Date`, `Dictionary`, and `Array`) into `src/runtime/`; public runtime headers remain in `include/runtime/` or `include/`
- **Version Bump**: Updated the visible project version to `v1.0.3`
- **Reasoning**: Keeps public include directories header-only and places runtime implementation files under the source tree consistently

- **Production Type File Split**: Split top-level reusable production types into individual files: `TypeSymbol`, `GeneratorLanguageInfo`, `GeneratorInstance`, and runtime field-wrapper templates now have dedicated headers/sources where applicable; `AST` and plugin ABI bundles remain grouped because they are tightly coupled contracts
- **Date Runtime Source Added**: Added `Date.cpp` definitions and included `Date` in `compiler-runtime`
- **Version Bump**: Updated the visible project version to `v1.0.2`
- **Reasoning**: Aligns reusable production types with the one-type-per-file convention without creating empty template `.cpp` files or fragmenting tightly coupled AST and plugin ABI declarations

- **ICodeGenerator in compiler-runtime**: Moved `ICodeGenerator` implementation from the `model-compiler` executable target into the `compiler-runtime` shared library
- **Version Bump**: Updated the visible project version to `v1.0.1`
- **Reasoning**: Lets generator plugins use the generator interface from the shared runtime alongside `AST`, `SemanticAnalyzer`, and `Console`, instead of depending on executable-owned interface implementation code

- **Code Generator Interface Rename**: Renamed the generator base class and files from `CodeGenerator` to `ICodeGenerator` and updated the plugin ABI to use `ICodeGenerator*`
- **Version Bump**: Updated the visible project version to `v1.0.0`
- **Reasoning**: Makes the generator contract explicit in naming and marks the source-level plugin API rename as a breaking public API change

- **AST and SemanticAnalyzer in compiler-runtime**: Moved `AST`, `SemanticAnalyzer`, and `Console` implementations into the `compiler-runtime` shared library so generator plugins can link against the validated model and semantic services through the runtime library
- **Version Bump**: Updated the visible project version to `v0.5.0`
- **Reasoning**: Makes the compiler model and semantic analysis APIs available to generator shared libraries without relying on duplicate object code in `model-compiler`

- **compiler-runtime Shared Library**: Added a `compiler-runtime` shared library containing `Fabric`, `Guid`, `String`, `Dictionary`, and the new `Array` runtime/template types
- **Array Template**: Added `Array<TValue>` with PIMPL storage and explicit `Array<const char*>` instantiation for ABI-friendly value arrays
- **Runtime Linking**: `model-compiler`, generator plugins, and runtime tests now link against `compiler-runtime` instead of compiling runtime implementation files directly into each target
- **Version Bump**: Updated the visible project version to `v0.4.0`
- **Reasoning**: Centralizes reusable runtime and plugin metadata types in a shared library, avoids duplicate runtime object code, and provides an array counterpart to `Dictionary`

- **Dictionary Template**: Converted `Dictionary` into `Dictionary<TKey, TValue>` and introduced `CapabilitiesDictionary` as the explicit `Dictionary<const char*, const char*>` specialization used by generator plugin capabilities
- **Version Bump**: Updated the visible project version to `v0.3.3`
- **Reasoning**: Keeps the plugin capability exchange STL-free while making the dictionary abstraction reusable for other key/value types

- **Plugin Capability Gate**: Plugin discovery now scans platform shared libraries in the plugin directory and treats only libraries exporting `GetCapabilities` as generator plugins; shared libraries without that function are ignored as non-plugins
- **Version Bump**: Updated the visible project version to `v0.3.2`
- **Reasoning**: Allows plugin directories to contain unrelated shared libraries without noisy warnings or failed generator discovery

- **Generator Library Suffixes**: Generator plugins now build as shared libraries with platform-native suffixes: `.dylib` on macOS, `.dll` on Windows, and `.so` on Linux
- **Version Bump**: Updated the visible project version to `v0.3.1`
- **Reasoning**: Aligns plugin artifacts with platform conventions while keeping generator libraries executable-adjacent for development

- **List Languages CLI**: Added `--list-languages` to scan generator plugins and print discovered generator languages without requiring an input file
- **Version Bump**: Updated the visible project version to `v0.3.0`
- **Reasoning**: Makes plugin discovery observable from the CLI and simplifies checking which generator shared libraries are available during development

- **Generator Plugin Shared Libraries**: Moved code generation behind dynamically loaded in-tree generator modules named `c++-generator`, `rust-generator`, and `swift-generator`
- **Plugin Location**: Generator shared libraries are built directly beside `model-compiler` during development; `--plugin-dir` can point the compiler at another generator directory
- **Plugin ABI Boundary**: Generator plugins export `GetCapabilities`, `DestroyCapabilities`, `CreateGenerator`, and `DestroyGenerator`; exported function signatures avoid STL types and use the PIMPL `Dictionary` for `String:String` capability metadata
- **Swift Generator Added**: Added an initial experimental Swift generator plugin with `Language = "Swift"` and `.swift` output
- **Version Bump**: Updated the visible project version to `v0.2.0`
- **Reasoning**: Decouples generator selection from the compiler executable while keeping compiler frontend and driver code in `model-compiler`, making generator loading testable without introducing a separate compiler-core shared library

- **String Runtime PIMPL**: Converted `bbfm::runtime::String` from header-owned `std::string` storage to an out-of-line PIMPL implementation with explicit copy/move operations and focused CTest coverage
- **Version Bump**: Updated the visible project version from `v0.1.0` to `v0.1.1`
- **Reasoning**: Hides runtime string storage details behind a stable public header while preserving source-compatible value semantics for generated code

### 2026-05-24

- **Skill Loading Decentralized**: Removed central skill table from AGENTS.md; restored just-in-time load instructions in each domain section
- **Reasoning**: Avoids agents preloading all skills during init-session and filling context with unrelated rules

- **Commit Protocol Consolidated**: Moved commit instructions from `/init-session` into AGENTS.md Commit Protocol; init-session now references AGENTS.md
- **Reasoning**: Keeps commit workflow in one canonical place while preserving skill authority over summaries

- **Skill Loading Made CRITICAL**: Added a skill loading table to `AGENTS.md`; skills in `.agents/skills/` are authoritative over inline summaries in this file
- **Reasoning**: Prevents agents from following incomplete commit/build/test rules copied into AGENTS.md instead of the full skill files

- **Commit Rules Moved to git-workflow Skill**: Removed commit message format details from `AGENTS.md`; `git-workflow` now owns bullets, limits, examples, and HEREDOC invocation
- **Reasoning**: Eliminates conflicting instructions that caused prose commit bodies instead of required bullet lists

- **git-workflow Skill Strengthened**: Mandatory bulleted bodies, docs example, bad prose example, HEREDOC as preferred commit invocation, explicit skill authority statement
- **Reasoning**: Makes the bullet rule impossible to miss and aligns commit workflow with how agents actually run git in Cursor

- **Mission Statement Updated**: Replaced the placeholder mission with the BBFM Model Compiler purpose, DSL scope, validation/code-generation workflow, and current/future target-language direction
- **Reasoning**: Keeps the primary agent instructions aligned with the actual project described by README.md

- **Technology Stack Updated**: Replaced placeholder stack entries with C++23/C++17, BBFM `.fm`, CMake/Ninja, Flex/Bison, cxxopts, CTest, Git, and MIT
- **Reasoning**: Documents the actual project tooling and dependencies so agents start from accurate assumptions

- **Testing Approach Updated**: Replaced placeholder testing guidance with CTest, `.fm` fixture, and plain C++ runtime test conventions
- **Reasoning**: Reflects the actual test structure in `CMakeLists.txt`, `examples/`, and `tests/runtime/GuidTest.cpp`

- **Documentation Requirements Updated**: Replaced placeholder documentation guidance with Doxygen, README, generated-example, and `AGENTS.md` update-log expectations
- **Reasoning**: Keeps documentation work tied to real implementation details and the repository's existing agent-governance workflow

- **Inline Guidance Moved to Skills**: Replaced duplicated inline C++ coding conventions and CMake build commands with references to `cpp-coding-conventions` and `cmake-build-commands`
- **Reasoning**: Keeps `AGENTS.md` concise while preserving the full guidance in dedicated, reusable skill files

- **Testing and Documentation Skills Added**: Created reusable `cpp-testing-conventions` and `cpp-documentation-conventions` skills, then replaced the inline `AGENTS.md` Testing and Documentation guidance with skill references
- **Reasoning**: Reduces repeated context in `AGENTS.md` while establishing general-purpose C++ testing and documentation conventions for future use

- **CMake Rebuild Guidance Updated**: Added explicit rebuild instructions to `cmake-build-commands` covering missing, unconfigured, clean, and stale build directories without assuming a project-specific script
- **Reasoning**: Clarifies that rebuild means clean plus build while keeping the skill generally applicable to CMake projects

- **CMake Build Skill Expanded**: Added guidance for documented build directories, `_build` fallback, presets, single- vs multi-config generators, `ctest --test-dir`, `compile_commands.json`, stale build directories, and cautious formatting/static-analysis workflows
- **Reasoning**: Makes the general CMake build skill robust across projects without assuming local helper scripts or a `build/` directory

- **Contracts.h**: Added `include/Contracts.h` with two programming-by-contract macros for input parameter validation at function entry
  - `Require(expr)` — for void functions; returns immediately if precondition is false
  - `RequireReturn(expr, retval)` — for non-void functions; returns `retval` if precondition is false
  - **CRITICAL**: both macros may **only** appear at the very top of a function or method, before any other statement — they validate input parameters on entry, not mid-function conditions
  - **One call per parameter**: use exactly one `Require`/`RequireReturn` call per input parameter — never combine multiple parameter checks with `&&` in a single call
  - **Context-dependent validity**: validity is determined by what the function does, not by the parameter type — a null pointer, an empty string, a negative index, and a zero count are all equally invalid if the function cannot meaningfully proceed with that value; the same value (e.g. index `-1`) can be valid in one function and a caller bug in another
  - **Naming exception**: these macros use PascalCase (not UPPER_SNAKE_CASE) so they read as natural-language assertions at the call site
  - `do { ... } while (false)` wrapper ensures safe use after naked `if` and requires a trailing semicolon
  - `(expr) == false` follows the project's explicit-condition convention

- **Const Local Variables**: Added rule that local variables not modified after initial definition must be declared `const`
- **Applied to main.cpp**: `sourceFiles`, `generateCode`, `targetClassPrefix`, `targetNamespace`, `namespaces`, `lastDot` now declared `const`
- **Reasoning**: Enforces intent at compile time, improves readability (reader immediately knows the variable won't change), and enables compiler optimizations
- **Refined Comparison Convention**: Constant-left applies only to `==` and `!=`; relational operators (`<`, `>`, `<=`, `>=`) use natural order (constant on right)
- **Const-operand Exception**: When the variable operand is itself `const` or is an **rvalue** (function return value), the literal goes on the right (neither can be accidentally assigned to)
- **Applied to main.cpp**: `0 < result.count("lang")` → `result.count("lang") > 0`; `true == generateCode` → `generateCode == true`; `false == targetClassPrefix.empty()` → `targetClassPrefix.empty() == false`; `std::string::npos != lastDot` → `lastDot != std::string::npos`; `0 != result.count(...)` → `result.count(...) != 0` (rvalue)
- **Contracts.h applied broadly**: Added `Require`/`RequireReturn` guards to all remaining unguarded functions across SemanticAnalyzer.cpp (`ValidateClassDeclaration`, `HasInheritanceCycle`, `ValidateFieldUniqueness`, `ValidateInvariants`, `ValidateComputedFeatures`, `ValidateAliasField`, `ValidateComputedFeatureExpression`, `ValidateMemberAccess`) and RustCodeGenerator.cpp (`GenerateGetter`, `GenerateSetter`, `GenerateComputedGetter`)
- **Removed VSCode extension**: Deleted `vscode/` directory, `.vscode/settings.json`, `.vscode/extensions.json`, and `[*.fm]` section from `.editorconfig` — the extension was an experiment and is no longer part of the project

### 2026-05-22

- **Session Initialization**: Analyzed workspace on `develop` branch (up to date with `origin/develop`)
- **Compilation Phases**: Phase 0 (parsing), Phase 1 (semantic analysis), and Phase 2 (C++ code generation) are complete and integrated
- **CLI**: `--lang c++` enables code generation; omitting `--lang` runs validation-only mode
- **Build**: CMake 3.20+, C++23, Flex/Bison, cxxopts; build via `./build.sh` → `_build/model-compiler`
- **Work in Progress (untracked)**: `RustCodeGenerator.h/cpp` and `examples/test_computed_simple.rs` exist locally but are not wired into `CMakeLists.txt`, `Driver.cpp`, or `--lang` validation yet
- **Reasoning**: Capture current project state at session start so subsequent work builds on accurate context

### 2025-12-26 (Evening)

- **Added --lang Option for Code Generator Selection**: Implemented optional command-line option to select target language
- **Command-line Option**: `--lang <language>` (optional, no default value)
- **Validation Mode**: When --lang is omitted, compiler performs only syntax and semantic validation (Phases 0 and 1)
- **Code Generation Mode**: When --lang is specified, compiler performs full compilation including code generation (Phases 0, 1, and 2)
- **Language Validation**: Only 'c++' is currently supported; unsupported languages are rejected with clear error message
- **Status Reporting**: Displays "Validation mode" or "Target language: c++" depending on whether --lang is specified
- **Completion Messages**: "Validation completed successfully!" vs "Compilation completed successfully!"
- **Documentation**: Updated README.md with examples of both validation-only and code generation modes
- **Use Case**: Enables quick error checking without committing to code generation
- **Reasoning**: Two-phase workflow allows users to validate input files before generating output, improving development efficiency and preventing unnecessary file generation during error checking

### 2025-12-26 (Morning)

- **Unix/Windows isatty Compatibility Fix**: Fixed compilation error on Unix systems
- **Problem**: Windows compatibility changes added `%option nounistd` to lexer, which prevented automatic inclusion of `<unistd.h>` on Unix
- **Impact**: `isatty` and `fileno` functions became undeclared on Unix/macOS builds
- **Solution**: Added explicit `#include <unistd.h>` for non-Windows platforms in lexer
- **Implementation**: Updated `model-compiler.l` with platform-specific includes:
  - Windows: `#include <io.h>` with `_isatty`/`_fileno` macros
  - Unix/macOS: `#include <unistd.h>` for standard `isatty`/`fileno` functions
- **Verification**: Build tested successfully on macOS, maintains Windows compatibility
- **Reasoning**: Ensures cross-platform compilation after Windows-specific changes while maintaining clean separation of platform-specific code

### 2025-12-25

- Implemented Guid runtime class with PIMPL pattern for platform isolation
- Platform-native UUID generation: CoCreateGuid (Windows), CFUUIDCreate (macOS), uuid_generate (Linux libuuid)
- Internal storage: raw uint8_t[16] with lazy cached string formatting
- Added bbfm_runtime INTERFACE CMake target carrying platform link deps (ole32, CoreFoundation, uuid)
- Linux libuuid discovered via PkgConfig in CMakeLists.txt
- Added comprehensive CTest unit tests (tests/runtime/GuidTest.cpp) using plain main() with custom CHECK macros (no external framework)
- Guid semantics: default-constructed is uninitialized (IsValid=false, IsNil=true); Guid::Nil() is explicit nil (IsValid=true, IsNil=true)
- Full Rule of Five, comparison operators (==, !=, <), and std::hash specialization

### 2025-10-05

- Initial AGENTS.md setup
- Established core coding standards and conventions
- Created agent-specific reference files
- Defined repository structure and governance principles
