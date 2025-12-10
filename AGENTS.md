# Project Instructions for AI Coding Agents

**Last updated:** 2025-12-11 (morning - checker function naming refactor)

<!-- {mission} -->

## Mission Statement

**Model Compiler (BBFM)** is a Domain-Specific Language compiler for the Big Bad Feed Machine (BBFM) project. It compiles podcast object models defined in a UML-inspired modeling syntax into target programming languages. The compiler provides a type-safe, expressive language for defining data structures, relationships, constraints (invariants), and computed features, which are then translated into production-ready source code.

**Key Features:**

- UML-inspired DSL for podcast domain modeling
- Type-safe modeling with primitives, enumerations, and user-defined types
- Single inheritance support with cycle detection
- Computed features with expression system (arithmetic, logical, member access)
- Invariant constraints for domain validation
- Namespace support for code organization (command-line and source file)
- Multi-phase compilation: parsing, semantic analysis, code generation
- Currently targets C++ (additional languages planned)

## Technology Stack

- **Language:** C++23 (minimum C++17 for broader compiler support)
- **Build System:** CMake 3.20+ with Ninja
- **Parser/Lexer:** Flex 2.6+ (lexical analysis), Bison 3.8+ (parsing)
- **CLI Library:** cxxopts 3.3.1 (command-line argument parsing)
- **Version Control:** Git
- **License:** MIT
- **Target Code Generation:** C++ (Phase 2 - Complete ✅)

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

### Development Guidelines

**Project-Specific Guidelines for BBFM Model Compiler:**

- **Namespace**: All code must be in the `bbfm` namespace
- **Include Guard Format**: Use `__BBFM_CLASS_NAME_H_INCL__` format (e.g., `__BBFM_DRIVER_H_INCL__`)
- **AST Design**: Use visitor pattern for AST traversal when implementing new phases
- **Error Reporting**: Use file:line:column format with visual source context pointers
- **Compilation Phases**:
  - Phase 0: Lexical analysis and parsing (AST construction) - Complete ✅
  - Phase 1: Semantic analysis (symbol table, type checking) - Complete ✅
  - Phase 2: Code generation (C++ output) - Complete ✅
- **Smart Pointers**: Use `std::unique_ptr` for AST nodes and owned resources
- **Expression System**: Support arithmetic, comparison, logical operators, member access
- **Testing**: Test with example `.fm` files in `examples/` directory
- **Checker Function Naming**: Use `Require_<field>_<invariant>` pattern (e.g., `Require_timestamp_validTimestamp`)
  - Semantics: "Require" emphasizes that invariant must hold (not just "checking")
  - Field-first: More natural to read ("requiring timestamp to satisfy validTimestamp")
  - Bound at compile-time as template parameters in `BoundedValue<T, ParentT, ...Checkers>`

### Security & Safety

- Never include API keys, tokens, or credentials in code
- Always require explicit human confirmation before commits
- Maintain conventional commit message standards
- Keep change history transparent through commit messages
- [Add project-specific security guidelines]

### Testing

**Testing Approach for Model Compiler:**

- **Integration tests**: Example `.fm` files in `examples/` directory
- **Test files**: Files prefixed with `test_` for specific scenarios
- **Error test suite**: `error_test_suite.fm` for error handling validation
- **Comprehensive test**: `comprehensive_test.fm` for full language feature coverage
- **Testing workflow**:
  - Compile example files: `./_build/model-compiler examples/file.fm`
  - Dump AST: `./_build/model-compiler --dump-syntax-tree examples/file.fm`
  - Dump symbol table: `./_build/model-compiler --dump-symbol-table examples/file.fm`
- **Testing framework**: Manual integration testing (unit test framework TBD)

### Documentation

**Documentation Requirements for Model Compiler:**

- **Code comments**: Use Doxygen format (`///`, `\brief`, `\param`, `\return`) for all public APIs
- **API documentation**: Header files in `include/` directory contain full Doxygen documentation
- **README updates**: Update when adding language features, compilation phases, or usage examples
- **Language specification**: Document new keywords, types, or syntax in README.md
- **Example files**: Add commented `.fm` examples in `examples/` directory for new features
- **AST documentation**: Document new AST node types with their purpose and structure
- **Changelog**: Track major changes in Recent Updates & Decisions section of AGENTS.md

<!-- {languages} -->

## C++ Coding Conventions

**General Principles:**

- Follow modern C++ best practices (C++23 standard preferred, C++17 minimum)
- Use RAII principles for resource management
- Prefer smart pointers (`std::unique_ptr`, `std::shared_ptr`) over raw pointers
- Apply const-correctness throughout the codebase
- Write self-documenting code with clear naming and structure
- Keep functions focused and modular
- Leverage the type system for compile-time safety
- Ensure platform portability (Linux, macOS, Windows)

**C++ Standard and Compatibility:**

- Use **C++23 standard** when possible for latest features
- Maintain **C++17 minimum** for broader compiler support
- Use standard library features over custom implementations
- Avoid compiler-specific extensions unless necessary
- Test on multiple compilers (GCC, Clang, MSVC)
- Use feature test macros for conditional compilation
- Handle platform differences through standard mechanisms

**Const Correctness:**

- All input parameters should be `const` when not modified
- Member functions that don't modify state should be `const`
- Use `const` references for complex types in parameters
- Apply `const` to return values when appropriate
- Examples:
  - ✅ Correct: `void SetTitle(const std::string& title);`
  - ✅ Correct: `std::string GetTitle() const;`
  - ✅ Correct: `const Data& GetData() const;`
  - ❌ Incorrect: `void SetTitle(std::string title);` (unnecessary copy)
- Const correctness improves maintainability and enables compiler optimization

**Comparison Conventions:**

- **Always place constants on the left side of comparisons** (constant-left style)
- Use explicit `nullptr` comparisons instead of implicit boolean conversion
- This prevents accidental assignment when `=` is used instead of `==`
- Examples:
  - ✅ Correct: `if (nullptr == ptr)`, `if (0 == value)`, `if (true == condition)`
  - ❌ Incorrect: `if (!ptr)`, `if (ptr == nullptr)`, `if (value == 0)`
- Apply to all comparisons including pointer checks, numeric values, and booleans
- Benefits: Compiler error if `=` is mistakenly used instead of `==`

**RAII and Resource Management:**

- Use RAII for all resource management (memory, files, locks, etc.)
- Prefer smart pointers over raw pointers:
  - `std::unique_ptr` for exclusive ownership
  - `std::shared_ptr` for shared ownership
  - `std::weak_ptr` to break circular dependencies
- Use standard containers instead of manual memory management
- Examples:

  ```cpp
  // Good: RAII with smart pointers
  auto data = std::make_unique<Data>();
  auto shared = std::make_shared<SharedObject>();

  // Good: RAII with containers
  std::vector<int> numbers;
  std::string text;

  // Avoid: Raw pointers requiring manual cleanup
  Data* data = new Data();  // Must remember to delete
  ```

- Let destructors handle cleanup automatically

**Classes and Destructors:**

- All destructors should be virtual (even when deleted)
- All abstract/interface classes should have a protected virtual destructor
- Use the Rule of Zero when possible (let compiler generate special members)
- When implementing special members, follow the Rule of Five
- Declare move constructor and move assignment operator when beneficial
- Examples:

  ```cpp
  // Rule of Zero: Compiler generates all special members
  class Simple
  {
  public:
      Simple() = default;
      std::string name;
      std::vector<int> data;
  };

  // Rule of Five: Custom resource management
  class Resource
  {
  public:
      Resource();
      ~Resource();
      Resource(const Resource& other);
      Resource& operator=(const Resource& other);
      Resource(Resource&& other) noexcept;
      Resource& operator=(Resource&& other) noexcept;
  };
  ```

**File Organization:**

- **Header files (.h)**: Class declarations, inline functions, templates
- **Implementation files (.cpp)**: Method implementations, non-template code
- Each class should have a separate header and implementation file
- Filename must match the class name exactly (e.g., `Driver` class → `Driver.h` and `Driver.cpp`)
- Header files go in `include/` directory
- Implementation files go in `src/` directory
- Exceptions:
  - Template classes may have implementation in header if needed
  - Tightly coupled class hierarchies (like AST nodes) may share files

**Implementation Separation:**

- Method implementations should be in .cpp files, not inline in headers
- Reduces recompilation of dependencies when implementation changes
- Only these may remain inline in headers:
  - Constructors (if trivial)
  - Destructors (if trivial)
  - One-line getters/setters for performance
  - Template functions (required)
- Prefer out-of-line implementations for better compilation times

**Class Structure and Scope Order:**

- Always declare scopes in the order: `public`, `protected`, `private`
- This makes the public interface immediately visible when reading class definitions
- Group related members together within each section
- Example:

  ```cpp
  class MyClass
  {
  public:
      // Constructors and destructor
      MyClass();
      virtual ~MyClass();

      // Public interface
      void PublicMethod();
      int GetValue() const;

  protected:
      // Protected interface for derived classes
      virtual void ProtectedMethod();

  private:
      // Private implementation details
      void PrivateHelper();
      int privateData_;
      std::string privateName_;
  };
  ```

**Naming Conventions:**

- **Types** (classes, structs, enums, typedefs): Upper PascalCase (e.g., `Episode`, `MediaType`)
- **Functions/methods**: Upper PascalCase (e.g., `GetTitle`, `SetDuration`, `ParseInput`)
- **Variables and function parameters**: camelCase (e.g., `bufferSize`, `episodeCount`)
- **Member variables**: camelCase with underscore postfix (e.g., `dataSize_`, `title_`)
- **Constants**: UPPER_SNAKE_CASE (e.g., `MAX_EPISODE_LENGTH`, `DEFAULT_TIMEOUT`)
- **Namespaces**: lowercase (e.g., `myproject`, `utils`)
- **Template parameters**: Single uppercase letter or PascalCase (e.g., `T`, `ValueType`)
- Remove redundant prefixes from class names (e.g., use `Model` instead of `P3Model`)

**Include Guards:**

- Use format `__PROJECT_CLASS_NAME_H_INCL__` where CLASS_NAME matches the class
- Must start with project-specific prefix to identify namespace
- Single word class: `Driver` → `__MYPROJECT_DRIVER_H_INCL__`
- Multi-word class: `TestTools` → `__MYPROJECT_TEST_TOOLS_H_INCL__`
- Insert underscore between each word in PascalCase class names
- Examples:

  ```cpp
  #ifndef __MYPROJECT_DRIVER_H_INCL__
  #define __MYPROJECT_DRIVER_H_INCL__

  // Class declaration

  #endif // __MYPROJECT_DRIVER_H_INCL__
  ```

- Alternative: Use `#pragma once` if all target compilers support it

**Header File Structure:**

- Include guard or `#pragma once` at top
- Includes (system headers first, then project headers)
- Forward declarations (to minimize includes)
- Type definitions and aliases
- Class declarations
- Inline function definitions
- Example:

  ```cpp
  #ifndef __MYPROJECT_CLASS_H_INCL__
  #define __MYPROJECT_CLASS_H_INCL__

  #include <string>
  #include <vector>

  #include "BaseClass.h"

  // Forward declarations
  class Helper;

  class MyClass : public BaseClass
  {
  public:
      // ... class definition
  };

  #endif // __MYPROJECT_CLASS_H_INCL__
  ```

**Alignment Pragmas:**

- All header files must use 8-byte alignment for types using `#pragma pack`
- Include alignment pragmas at the top and restore at the bottom
- Use cross-compiler compatible pragmas for MSVC, GCC, and Clang:

  ```cpp
  // At top of header (after include guard, before includes)
  #pragma pack(push, 8)

  // ... class declarations ...

  // At bottom of header (before closing include guard)
  #pragma pack(pop)
  ```

**Implementation File Organization:**

- Include corresponding header first
- Include system headers
- Include project headers
- Anonymous namespace for file-local helpers
- Class member function implementations
- Example:

  ```cpp
  #include "MyClass.h"

  #include <algorithm>
  #include <iostream>

  #include "Helper.h"

  namespace
  {
      // File-local helper functions
      void LocalHelper()
      {
          // ...
      }
  }

  // Class member implementations
  MyClass::MyClass()
  {
      // ...
  }
  ```

**Namespaces:**

- Use namespaces to organize code logically
- Avoid `using` directives in headers (e.g., `using namespace std;`)
- Use `using` declarations sparingly in implementation files
- Prefer explicit namespace qualification for clarity
- Use nested namespaces for hierarchical organization
- Examples:

  ```cpp
  namespace myproject
  {
      namespace utils
      {
          class Helper { };
      }

      class MainClass { };
  }

  // C++17 nested namespace syntax
  namespace myproject::utils
  {
      class Helper { };
  }
  ```

**Function and Method Design:**

- Keep functions short and focused on single responsibility
- Use early returns to reduce nesting depth
- Pass by const reference for complex types, by value for primitives
- Use trailing return types when it improves clarity (e.g., with `auto`)
- For intentionally unused parameters, use `[[maybe_unused]]` attribute or comment
- Examples:

  ```cpp
  // Good: Clear parameter passing
  void ProcessData(const std::vector<int>& data, int threshold);

  // Good: Trailing return type with auto
  auto GetValue() -> std::optional<int>;

  // Good: Unused parameter handling
  void Handler([[maybe_unused]] int eventType)
  {
      // Implementation doesn't use eventType
  }
  ```

**Type Definitions and Aliases:**

- Use `using` instead of `typedef` for type aliases
- Create meaningful aliases for complex types
- Document the purpose of type aliases
- Examples:

  ```cpp
  // Good: Clear type aliases
  using UserId = uint64_t;
  using ErrorCallback = std::function<void(const std::string&)>;
  using DataMap = std::unordered_map<std::string, std::shared_ptr<Data>>;

  // Avoid: Obscure typedef
  typedef unsigned long long int ull;
  ```

**Enums:**

- Prefer `enum class` over `enum` for type safety
- Use explicit underlying types when needed
- Prefix enum values with enum name for clarity (only if not using `enum class`)
- Examples:

  ```cpp
  // Best: enum class (scoped and type-safe)
  enum class Color : uint8_t
  {
      Red,
      Green,
      Blue
  };

  // Usage: Color::Red

  // Acceptable: Traditional enum with prefix
  enum MediaType
  {
      MEDIA_TYPE_AUDIO,
      MEDIA_TYPE_VIDEO,
      MEDIA_TYPE_SUBTITLE
  };
  ```

**Error Handling:**

- Use exceptions for exceptional conditions
- Use `std::optional` for values that may not exist
- Use `std::expected` (C++23) or similar for expected errors
- Never throw from destructors
- Document exceptions in function comments
- Examples:

  ```cpp
  // Good: Optional for nullable values
  std::optional<User> FindUser(const std::string& name);

  // Good: Exception for errors
  void LoadFile(const std::string& path)
  {
      if (path.empty())
      {
          throw std::invalid_argument("Path cannot be empty");
      }
      // ... load file
  }

  // Good: Error handling with optional
  auto user = FindUser("john");
  if (user.has_value())
  {
      ProcessUser(user.value());
  }
  ```

**Memory Management:**

- Prefer stack allocation over heap allocation when possible
- Use smart pointers for heap-allocated objects
- Use `std::make_unique` and `std::make_shared` for construction
- Avoid naked `new` and `delete`
- Use containers for collections of objects
- Examples:

  ```cpp
  // Good: Smart pointers
  auto data = std::make_unique<Data>();
  auto shared = std::make_shared<Config>();

  // Good: Stack allocation
  Data localData;
  std::array<int, 10> numbers;

  // Good: Containers
  std::vector<std::unique_ptr<Item>> items;
  ```

**Comments:**

- Use `//` for all comments (single-line and multi-line)
- Document public APIs with Doxygen-style comments in header files
- Use traditional Doxygen syntax:
  - `///` for Doxygen comments
  - `\brief` for brief descriptions
  - `\param` for parameters
  - `\return` for return values
- Implementation files should use inline `//` comments for logic explanation
- Comment the "why" not the "what"
- Examples:

  ```cpp
  /// \brief Sets the episode title
  /// \param title The new title for the episode
  void SetTitle(const std::string& title);

  // Implementation comment explaining reasoning
  // Use binary search because data is sorted
  auto it = std::lower_bound(data.begin(), data.end(), target);
  ```

**Code Formatting:**

- Use consistent indentation (4 spaces preferred)
- Braces: Opening brace on next line for functions and blocks
- Example:

  ```cpp
  // Function: opening brace on next line
  void MyClass::ProcessData(const std::vector<int>& data)
  {
      // Control structure: opening brace on next line
      if (nullptr == data_)
      {
          Initialize();
      }

      for (const auto& item : data)
      {
          ProcessItem(item);
      }
  }
  ```

- Line length: Keep under 120 characters when practical
- Use `.clang-format` configuration for automatic formatting

**Modern C++ Features:**

- Use `auto` for type deduction when type is obvious from context
- Use range-based for loops instead of iterators when possible
- Use structured bindings (C++17) for multiple return values
- Use `std::string_view` for non-owning string references
- Use `constexpr` for compile-time constants
- Examples:

  ```cpp
  // Good: auto for obvious types
  auto config = std::make_unique<Config>();
  auto it = container.find(key);

  // Good: Range-based for
  for (const auto& item : items)
  {
      ProcessItem(item);
  }

  // Good: Structured bindings
  auto [success, value] = TryParse(input);

  // Good: string_view
  void ProcessName(std::string_view name);

  // Good: constexpr
  constexpr int MAX_SIZE = 1024;
  ```

**Templates:**

- Keep template code in headers (required by C++ standard)
- Use concepts (C++20) to constrain template parameters
- Provide clear error messages for template failures
- Document template parameters and requirements
- Examples:

  ```cpp
  // C++20 concepts
  template<typename T>
  concept Drawable = requires(T obj)
  {
      obj.Draw();
  };

  template<Drawable T>
  void Render(const T& object)
  {
      object.Draw();
  }

  // Traditional template with static_assert
  template<typename T>
  class Container
  {
      static_assert(std::is_default_constructible_v<T>,
                    "T must be default constructible");
  };
  ```

**Lambda Expressions:**

- Use lambdas for short, local operations
- Capture by reference `[&]` for local scope, by value `[=]` when needed
- Be explicit with captures when clarity is important
- Use `mutable` when lambda needs to modify captured values
- Examples:

  ```cpp
  // Good: Short algorithm
  std::sort(items.begin(), items.end(),
            [](const Item& a, const Item& b)
            {
                return a.priority > b.priority;
            });

  // Good: Explicit captures
  int threshold = 10;
  auto filter = [threshold](int value)
  {
      return value > threshold;
  };

  // Good: Mutable lambda
  int counter = 0;
  auto increment = [counter]() mutable
  {
      return ++counter;
  };
  ```

**Standard Library Usage:**

- Prefer standard library over custom implementations
- Use algorithms from `<algorithm>` header
- Use standard containers (`vector`, `map`, `set`, etc.)
- Use `<string>` for string handling
- Use `<filesystem>` (C++17) for file operations
- Examples:

  ```cpp
  // Good: Standard algorithms
  std::sort(data.begin(), data.end());
  auto it = std::find_if(items.begin(), items.end(), predicate);

  // Good: Standard containers
  std::vector<int> numbers;
  std::unordered_map<std::string, Data> cache;

  // Good: Filesystem operations
  std::filesystem::path filePath = "/path/to/file";
  if (std::filesystem::exists(filePath))
  {
      // Process file
  }
  ```

**Const and Constexpr:**

- Use `const` for runtime constants
- Use `constexpr` for compile-time constants
- Use `consteval` (C++20) to force compile-time evaluation
- Mark functions `constexpr` when possible for compile-time optimization
- Examples:

  ```cpp
  // Runtime constant
  const int bufferSize = GetBufferSize();

  // Compile-time constant
  constexpr int MAX_USERS = 100;

  // Constexpr function
  constexpr int Square(int x)
  {
      return x * x;
  }

  // C++20 consteval (must be compile-time)
  consteval int Factorial(int n)
  {
      return (n <= 1) ? 1 : n * Factorial(n - 1);
  }
  ```

**Platform Portability:**

- Use standard C++ features when possible
- Handle platform differences through preprocessor or runtime checks
- Test on multiple platforms (Linux, macOS, Windows)
- Use standard integer types from `<cstdint>`
- Examples:

  ```cpp
  #ifdef _WIN32
      // Windows-specific code
      #include <windows.h>
  #else
      // POSIX code
      #include <unistd.h>
  #endif

  // Use standard fixed-size types
  uint32_t value32;
  int64_t offset;
  ```

**Compiler Warnings:**

- Build with strict warnings enabled:
  - GCC/Clang: `-Wall -Wextra -Wpedantic`
  - MSVC: `/W4`
- Treat warnings as errors in development builds
- Fix all warnings - don't suppress them unless absolutely necessary
- Document any warning suppressions with reasoning

**Testing Strategy:**

- Write unit tests for all public APIs
- Test edge cases: null pointers, empty containers, boundary values
- Use test frameworks (Google Test, Catch2, etc.)
- Mock dependencies for isolated testing
- Test on all target platforms
- Examples:

  ```cpp
  TEST(MyClassTest, ConstructorInitializesCorrectly)
  {
      MyClass obj;
      EXPECT_EQ(0, obj.GetValue());
  }

  TEST(MyClassTest, SetValueUpdatesCorrectly)
  {
      MyClass obj;
      obj.SetValue(42);
      EXPECT_EQ(42, obj.GetValue());
  }
  ```

**Documentation:**

- Document all public APIs in header files
- Include purpose, parameters, return values, and exceptions
- Use Doxygen format for API documentation
- Examples:

  ```cpp
  /// \brief Creates a new user account
  /// \param username The unique username for the account
  /// \param email The user's email address
  /// \return A unique pointer to the created User object
  /// \throws std::invalid_argument if username is empty
  std::unique_ptr<User> CreateUser(
      const std::string& username,
      const std::string& email);
  ```

**Documentation Tools:**

- Use Doxygen for API documentation generation
- Use Graphviz DOT for class diagrams and dependency diagrams
- Use `@dot...@enddot` blocks for custom graphs
- Keep diagrams clean and focused on domain relationships
- Treat standard types (String, etc.) as primitives in diagrams

**Documentation Accuracy:**

- **CRITICAL: Always verify documentation against actual implementation**
- README.md must show real API patterns, not fictional functions
- Use actual class names and member names from header files
- Integration examples must use real function signatures
- Keep documentation synchronized with code changes

**Code Review Checklist:**

- [ ] All public APIs have Doxygen documentation
- [ ] Const correctness applied throughout
- [ ] Constant-left comparisons used consistently
- [ ] Smart pointers used instead of raw pointers
- [ ] RAII principles applied for resource management
- [ ] Rule of Zero or Rule of Five followed correctly
- [ ] No memory leaks (verified with valgrind or similar)
- [ ] Code compiles without warnings on all platforms
- [ ] Unit tests pass
- [ ] Include guards or pragma once used correctly
- [ ] Namespaces used appropriately
- [ ] Modern C++ features used where beneficial
- [ ] Code formatted according to project standards

**Build System (CMake):**

- Use CMake 3.20+ for modern features
- Support multiple platforms (Linux, macOS, Windows)
- Support multiple compilers (GCC, Clang, MSVC)
- Generate both shared and static libraries
- Use CMake targets and properties
- Example CMakeLists.txt structure:

  ```cmake
  cmake_minimum_required(VERSION 3.20)
  project(MyProject VERSION 1.0.0 LANGUAGES CXX)

  set(CMAKE_CXX_STANDARD 23)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)

  # Library target
  add_library(mylib
      src/MyClass.cpp
      src/Helper.cpp
  )

  target_include_directories(mylib
      PUBLIC include
      PRIVATE src
  )

  # Executable target
  add_executable(myapp
      src/main.cpp
  )

  target_link_libraries(myapp PRIVATE mylib)
  ```

## Build Commands

### Setup

```bash
# Install CMake (if not already installed)
# macOS
brew install cmake

# Linux (Debian/Ubuntu)
sudo apt-get install cmake

# Linux (Fedora/RHEL)
sudo dnf install cmake

# Check CMake version
cmake --version
```

### Development

```bash
# Configure project (debug build - use during development)
cmake -B build -DCMAKE_BUILD_TYPE=Debug

# Build the project (debug)
cmake --build build

# Build with verbose output
cmake --build build --verbose

# Build specific target
cmake --build build --target target_name

# Run tests (if using CTest)
cd build && ctest

# Run tests with verbose output
cd build && ctest --verbose

# Run specific test
cd build && ctest -R test_name

# Clean build artifacts
cmake --build build --target clean

# Reconfigure from scratch
rm -rf build
cmake -B build -DCMAKE_BUILD_TYPE=Debug
```

### Build & Deploy

```bash
# Configure for release (optimized - use for final testing/deployment only)
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build release version
cmake --build build --config Release

# Install to system (requires proper CMAKE_INSTALL_PREFIX)
cmake --build build --target install

# Create distributable package (if configured)
cd build && cpack
```

### Advanced Options

```bash
# Configure with custom compiler
cmake -B build -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++

# Configure with custom install prefix
cmake -B build -DCMAKE_INSTALL_PREFIX=/path/to/install

# Configure with additional flags
cmake -B build -DCMAKE_CXX_FLAGS="-Wall -Wextra"

# Enable/disable specific features (example)
cmake -B build -DENABLE_FEATURE=ON

# Generate compile_commands.json for IDE/tools
cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# List available targets
cmake --build build --target help
```

### Static Analysis & Formatting

```bash
# Run clang-tidy (if configured)
cmake --build build --target clang-tidy

# Run clang-format to check formatting
find src include -name '*.cpp' -o -name '*.h' | xargs clang-format -n

# Apply clang-format
find src include -name '*.cpp' -o -name '*.h' | xargs clang-format -i

# Run cppcheck (if installed)
cppcheck --enable=all --project=build/compile_commands.json
```

### Multi-Configuration Generators

```bash
# For Visual Studio, Xcode, or Ninja Multi-Config
cmake -B build -G "Ninja Multi-Config"

# Build debug configuration
cmake --build build --config Debug

# Build release configuration
cmake --build build --config Release

# Build with multiple jobs (parallel)
cmake --build build --config Release -j 8
```

**Important**: Always use Debug builds (`-DCMAKE_BUILD_TYPE=Debug`) during development. Debug builds compile faster, include debugging symbols, and provide better error diagnostics. Only use Release builds (`-DCMAKE_BUILD_TYPE=Release`) for final testing or deployment.

<!-- {integration} -->

## Commit Protocol (CRITICAL)

- **NEVER commit automatically** - always wait for explicit confirmation

Whenever asked to commit changes:

- Stage the changes
- Write a detailed but concise commit message using conventional commits format
- Commit the changes

This is **CRITICAL**!

## **Commit Message Guidelines - CRITICAL**

Follow these rules to prevent VSCode terminal crashes and ensure clean git history:

**Message Format (Conventional Commits):**

```text
<type>(<scope>): <subject>

<body>

<footer>
```

**Character Limits:**

- **Subject line**: Maximum 50 characters (strict limit)
- **Body lines**: Wrap at 72 characters per line
- **Total message**: Keep under 500 characters total
- **Blank line**: Always add blank line between subject and body

**Subject Line Rules:**

- Use conventional commit types: `feat`, `fix`, `docs`, `style`, `refactor`, `test`, `chore`, `build`, `ci`, `perf`
- Scope is optional but recommended: `feat(api):`, `fix(build):`, `docs(readme):`
- Use imperative mood: "add feature" not "added feature"
- No period at end of subject line
- Keep concise and descriptive

**Body Rules (if needed):**

- Add blank line after subject before body
- Wrap each line at 72 characters maximum
- Explain what and why, not how
- Use bullet points (`-`) for multiple items with lowercase text after bullet
- Keep it concise

**Special Character Safety:**

- Avoid nested quotes or complex quoting
- Avoid special shell characters: `$`, `` ` ``, `!`, `\`, `|`, `&`, `;`
- Use simple punctuation only
- No emoji or unicode characters

**Best Practices:**

- **Break up large commits**: Split into smaller, focused commits with shorter messages
- **One concern per commit**: Each commit should address one specific change
- **Test before committing**: Ensure code builds and works
- **Reference issues**: Use `#123` format in footer if applicable

**Examples:**

Good:

```text
feat(api): add KStringTrim function

- add trimming function to remove whitespace from
  both ends of string
- supports all encodings
```

Good (short):

```text
fix(build): correct static library output name
```

Bad (too long):

```text
feat(api): add a new comprehensive string trimming function that handles all edge cases including UTF-8, UTF-16LE, UTF-16BE, and ANSI encodings with proper boundary checking and memory management
```

Bad (special characters):

```text
fix: update `KString` with "nested 'quotes'" & $special chars!
```

## Semantic Versioning Protocol

**AUTOMATICALLY track version changes using semantic versioning (SemVer) in Cargo.toml.**

The current version is defined in `Cargo.toml` under `[package]` section as `version = "X.Y.Z"`.

### Version Format: MAJOR.MINOR.PATCH

**When to increment:**

1. **PATCH version** (X.Y.Z → X.Y.Z+1)
   - Bug fixes and minor corrections
   - Performance improvements without API changes
   - Documentation updates
   - Internal refactoring that doesn't affect public API
   - Example: `1.0.0` → `1.0.1`

2. **MINOR version** (X.Y.Z → X.Y+1.0)
   - New features added
   - New CLI commands or options
   - New functionality that maintains backward compatibility
   - Example: `1.0.1` → `1.1.0`

3. **MAJOR version** (X.Y.Z → X+1.0.0)
   - Breaking changes to public API
   - Removal of features or commands
   - Changes that require user action or code updates
   - Incompatible CLI changes
   - Example: `1.1.0` → `2.0.0`

### Process

After making ANY code changes:

1. Determine the type of change (fix, feature, or breaking change)
2. Update the version in `Cargo.toml` accordingly
3. Include the version change in the same commit as the code change
4. Mention version bump in commit message footer if significant

**Note:** Version changes should be included in the commit with the actual code changes, not as a separate commit.

---

## Recent Updates & Decisions

### 2025-12-11 (Morning)

- **Checker Function Naming Refactor**: Changed invariant checker function naming convention for clarity
- **Old Pattern**: `Check` + invariantName + `_` + fieldName (e.g., `CheckvalidTimestamp_timestamp`)
- **New Pattern**: `Require_` + fieldName + `_` + invariantName (e.g., `Require_timestamp_validTimestamp`)
- **Semantics**: "Require" better expresses that invariant must hold (not just passive checking)
- **Readability**: Field-first order is more natural ("requiring timestamp to satisfy validTimestamp")
- **Implementation**: Updated CppCodeGenerator.cpp in three locations (lines 188, 500, 1257)
- **Template Parameters**: Checkers bound at compile-time via non-type template parameters in BoundedValue
- **Zero Overhead**: Function pointer approach maintains compile-time binding (rejected std::function for performance)
- **Reasoning**: Cleaner generated code while preserving zero-runtime-overhead design for invariant validation

### 2025-12-10 (Deep Night)

- **Alias Feature Implementation**: Added full support for field aliases using `alias` keyword
- **Syntax**: `alias aliasName = targetFieldName;` creates read-write alias to another field
- **AliasValue Wrapper**: Created template that forwards reads/writes to target field wrapper
- **Invariant Propagation**: Writing to alias triggers target field's invariant validation
- **Semantic Validation**: Aliases must reference simple fields (not expressions), no alias chains
- **Code Generation**: Generates `AliasValue<TargetWrapperType>` with correct template parameters
- **Type Inference**: GetFieldType recursively resolves alias target types for type checking
- **Defensive Checks**: Added null checks for alias fields throughout semantic analyzer
- **Use Case**: Enables creating convenient names for inherited fields (e.g., `alias startTime = timestamp`)
- **Reasoning**: Provides read-write access to inherited fields with full invariant validation, solving the problem where computed properties couldn't be written to

### 2025-12-10 (Very Late Night)

- **DynamicValue Wrapper**: Implemented `DynamicValue<T, ParentT>` wrapper for computed properties
- **Consistent Field Approach**: Computed properties now stored as fields with lambda initialization
- **Constructor Lambdas**: Computed fields initialized with `[](const Parent& parent) { return expr; }` lambdas
- **Expression Context**: Updated `ExpressionToCpp` to distinguish computed vs regular fields
- **Implicit Conversion**: Computed fields use implicit conversion, regular fields use `.value_` accessor
- **Optional Field Restriction**: Added semantic validation to prohibit optional fields in computed expressions
- **Validation Rule**: Computed features cannot reference optional fields (may not have value at computation time)
- **FindFieldInClass**: Added public method to SemanticAnalyzer for field lookup with inheritance
- **Reasoning**: DynamicValue provides consistent field-based interface for computed properties with on-demand evaluation; optional field restriction prevents undefined behavior

### 2025-12-10 (Late Night)

- **Invariant Checker Refactoring**: Improved separation of concerns in validation architecture
- **Checker Functions**: Now return `bool` instead of `void`, no side effects
- **Parameter Naming**: Changed `obj` to `object` (no abbreviations)
- **Const Correctness**: Checker functions use `const ClassName& object` parameter
- **Exception Handling**: Moved from checker functions to BoundedValue wrapper classes
- **Modification Date**: Moved UpdateModificationDate() to wrapper assignment operators
- **Clean Design**: Checker functions are now pure validation predicates
- **Reasoning**: Better separation of concerns - validation logic separate from error handling and state updates

### 2025-12-10 (Night)

- **Phase 2 Implementation Complete**: Full C++ code generation now functional
- **Abstract Code Generator**: Created base class architecture for multi-language support
- **CppCodeGenerator**: Concrete C++ implementation with all features
- **Runtime Types**: Created stub implementations for String, Date, Guid in bbfm::runtime namespace
- **Fabric Base Class**: Universal metadata fields (typeId, id, cardinality, etc.) provided via inheritance
- **Complete Feature Set**: Enums, classes, inheritance, computed features, invariant validation, getters
- **Command-line Options**: Added --output option for specifying output file path
- **Type Mappings**: String/Date/Guid map to bbfm::runtime types, others to standard C++
- **Code Quality**: Proper include guards, alignment pragmas, const correctness, Doxygen comments
- **Reasoning**: Complete code generation enables end-to-end compilation from BBFM models to C++ headers

### 2025-12-10 (Evening)

- **Namespace Support Implementation**: Added full namespace support to the language
- **Language Features**: Added `namespace` keyword for source file namespace declarations
- **Command-line Option**: `--target-namespace` option for specifying target namespace
- **Namespace Combination**: Source and target namespaces combine as nested namespaces in generated code
- **Lexer/Parser Updates**: Extended grammar to support `namespace identifier;` syntax
- **AST Changes**: Added `sourceNamespace_` field to AST class with getter method
- **Driver Enhancement**: Added `GetCombinedNamespaces()` method to combine CLI and source namespaces
- **Documentation**: Updated README.md with namespace section and examples
- **Reasoning**: Namespaces provide code organization and prevent naming conflicts in generated C++ code

### 2025-12-10 (Later)

- **Target Language Change**: Changed target language from Swift to C++
- **Updated Documentation**: All mentions of Swift replaced with C++ throughout project
- **Type Mappings**: Updated type mappings table to show C++ types (std::string, int64_t, double, etc.)
- **Reasoning**: C++ is a better fit as the target language for this C++-based compiler project

### 2025-12-10

- **Session Initialization**: Analyzed workspace and updated AGENTS.md with project-specific information
- **Mission Statement**: Documented BBFM Model Compiler purpose and key features
- **Technology Stack**: Updated with actual technologies (C++23, CMake, Flex/Bison, cxxopts, C++ target)
- **Development Guidelines**: Added project-specific guidelines (namespace, include guards, compilation phases)
- **Testing Approach**: Documented integration testing workflow with example files
- **Documentation Requirements**: Specified Doxygen format and documentation locations
- **README.md Updates**: Comprehensive updates to improve accuracy and completeness:
  - Added project header with license, language, and version information
  - Clarified C++17 minimum requirement (C++23 preferred)
  - Fixed type mappings table to include Date and Guid with proper descriptions
  - Updated project structure to accurately reflect examples directory contents
  - Added comprehensive "Example Files" section documenting all test files
  - Added "Development" section with code organization, coding standards, and contributing info
  - Added "Project Status" section showing current phase progress
  - Fixed build.sh comment (was "P3 Compiler", now "BBFM Model Compiler")
- **Reasoning**: Initialize AGENTS.md to accurately reflect the Model Compiler project structure and conventions, and ensure README.md is comprehensive, accurate, and helpful for developers

### 2025-10-05

- Initial AGENTS.md setup
- Established core coding standards and conventions
- Created agent-specific reference files
- Defined repository structure and governance principles
