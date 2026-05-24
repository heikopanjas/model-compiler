# Project Instructions for AI Coding Agents

**Last updated:** 2026-05-24 (Testing and docs moved to skills)

<!-- {preamble} -->

# ⚠️ Before You Start

Run `/init-session` at the beginning of each new session, OR read this entire file before proceeding.

**DO NOT** make code changes or commits until you have done one of the above.

<!-- {mission} -->

## Mission Statement

BBFM Model Compiler is a C++23 compiler for the Big Bad Feed Machine modeling DSL. It lets developers
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

### Update Protocol (CRITICAL)

**PROACTIVELY update this file (`AGENTS.md`) as we work together.** Whenever you make a decision, choose a technology, establish a convention, or define a standard, you MUST update AGENTS.md immediately in the same response.

**Update ONLY this file (`AGENTS.md`)** when coding standards, conventions, or project decisions evolve. Do not modify agent-specific reference files unless the reference mechanism itself needs changes.

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
- Maintain conventional commit message standards
- Keep change history transparent through commit messages
- [Add project-specific security guidelines]

### Testing

Load the `cpp-testing-conventions` skill before writing, reviewing, or refactoring C++ tests.

### Documentation

Load the `cpp-documentation-conventions` skill before writing, reviewing, or updating C++ documentation.

<!-- {languages} -->

## C++ Coding Standards

Load the `cpp-coding-conventions` skill before writing, reviewing, or refactoring C++ code.

## Build Commands

Load the `cmake-build-commands` skill when configuring, building, testing, formatting, or packaging the project.

<!-- {integration} -->

## Semantic Versioning

Automatically bump the project version after every code change and include it in the same commit. Load the `semantic-versioning` skill for the full PATCH/MINOR/MAJOR decision rules.

## Commit Protocol

- **NEVER commit automatically** — always wait for explicit user confirmation
- Stage changes, write a conventional commits message (max 50-char subject, 72-char body lines), then commit
- Load the `git-workflow` skill for the full message format, character limits, and examples before committing

---

<!-- {changelog} -->

## Recent Updates & Decisions

### 2026-05-24

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
