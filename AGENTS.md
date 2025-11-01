# AI Agent Operating Instructions

**Last updated:** November 1, 2025 (22:00)

## Primary Instructions

### File Structure & References

This file (`AGENTS.md`) is the primary instructions file for AI agents working on this project. The `.github/copilot-instructions.md` file references this document, maintaining a single source of truth.

### Working Together

When analyzing the workspace, read the following instruction files in order:

1. `AGENTS.md` (this file - primary instructions)
2. `.github/copilot-instructions.md` (references AGENTS.md)

### Update Protocol (CRITICAL)

**PROACTIVELY update this file (`AGENTS.md`) as we work together.** Whenever you make a decision, choose a technology, establish a convention, or define a standard, you MUST update AGENTS.md immediately in the same response.

**Update ONLY this file (`AGENTS.md`)** when coding standards, conventions, or project decisions evolve. Do not modify reference files unless the reference mechanism itself needs changes.

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

### Commit Protocol (CRITICAL)

Whenever asked to commit changes:

- Stage the changes
- Write a detailed but concise commit message using conventional commits format
- **NEVER commit automatically** - always wait for explicit confirmation
- This is CRITICAL!

### **Commit Message Guidelines - CRITICAL**

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

---

## Project Overview

This is a Domain-Specific Language (DSL) compiler for defining podcast object models and their relationships. Part of the Big Bad Feed Machine (BBFM) project, this compiler provides common infrastructure for podcast-related applications. The compiler is written in C++23 using Flex (lexical analyzer) and Bison (parser generator), built with CMake and Ninja.

### Project Goals

The compiler enables users to define data types and relationships for podcast domains (podcasts, episodes, audio assets, transcripts, etc.) using a simple, expressive language. The compiler then generates:

1. Source code in target programming languages

### Target Outputs (Initial)

- **Source Code**: Swift (with plans to support additional languages in the future)

## Coding Standards & Conventions

### Build System

- **Build Tool**: CMake (version 3.20+)
- **Generator**: Ninja (preferred over Make for faster parallel builds)
- **Build Directory**: `_build/` (not `build/`)
- **C++ Standard**: C++23
- **External Dependencies**:
  - **cxxopts**: Command line argument parser (v3.3.1)
    - Fetched via CMake FetchContent
    - Linked as static library (no runtime dependencies)
    - Disabled features: examples, tests, install, warnings, unicode help

### C++ Coding Standards

**General Principles:**

- Follow modern C++ best practices (C++23 standard)
- Use RAII principles for resource management
- Prefer smart pointers (`std::unique_ptr`, `std::shared_ptr`) over raw pointers
- Use const-correctness throughout the codebase
- This style provides extra safety and consistency throughout the codebase
- **Namespace**: All compiler code uses the `bbfm` namespace

**Functions and Methods:**

- All function input parameters should be const (e.g., `void SetTitle(const std::string& title)`)
- All functions that return class data without modification should be const (e.g., `std::string GetTitle() const`)
- Pass by const reference for complex types, by value for primitives
- Use trailing return types when it improves clarity
- For intentionally unused parameters, use the `UNREFERENCED_PARAMETER(param)` macro from `Common.h`

**Classes and Destructors:**

- All destructors should be virtual (even when deleted)
- All abstract/interface classes should have a protected virtual destructor
- Use the Rule of Zero when possible (let compiler generate special members)
- When implementing special members, follow the Rule of Five
- **File organization**: Each class should have a separate header (.h) and implementation (.cpp) file
  - Filename must match the class name exactly (e.g., `Driver` class → `Driver.h` and `Driver.cpp`)
  - Header files go in `include/` directory
  - Implementation files go in `src/` directory
  - Exception: Template classes may have implementation in header if needed
  - Exception: Tightly coupled class hierarchies (like AST nodes) may share a single header/implementation file pair
- **Implementation separation**: Method implementations should be in .cpp files, not inline in headers
  - Reduces recompilation of dependencies when implementation changes
  - Only constructors and trivial one-line getters may remain inline in headers if needed for performance
  - Prefer out-of-line implementations for better compilation times
- **Scope declaration order**: Always declare scopes in the order: `public`, `protected`, `private`
  - This makes the public interface immediately visible when reading class definitions
  - Example:

    ```cpp
    class MyClass {
    public:
        MyClass();
        virtual ~MyClass();
        void PublicMethod();

    protected:
        void ProtectedMethod();

    private:
        int privateData_;
    };
    ```

**Comparison and Conditional Expressions:**

- Always place constants on the left side of comparisons
- Use explicit `nullptr` comparisons instead of implicit boolean conversion
- Examples:
  - ✅ Correct: `if (nullptr == ast_)` or `if (0 == value)`
  - ❌ Incorrect: `if (!ast_)` or `if (ast_ == nullptr)`
- Reasoning: Prevents accidental assignment (`=`) instead of comparison (`==`); compiler will error on `nullptr = ast_` but may allow `ast_ = nullptr`
- Apply to all comparisons with literals, nullptr, and constants

**Naming Conventions:**

- Types (classes, structs, enums, typedefs): Upper PascalCase (e.g., `Episode`, `SharedObject`, `MediaType`)
- Functions/methods: Upper PascalCase (e.g., `GetTitle`, `SetDuration`, `ParseInput`)
- Variables and function parameters: camelCase (e.g., `bufferSize`, `episodeCount`, `userName`)
- Member variables: camelCase with underscore postfix (e.g., `dataSize_`, `title_`, `description_`)
- Constants: UPPER_SNAKE_CASE (e.g., `MAX_EPISODE_LENGTH`, `DEFAULT_TIMEOUT`)
- Remove redundant prefixes from class names (e.g., use `Model` instead of `P3Model`)
- **Include guards**: Use format `__BBFM_CLASS_NAME_H_INCL__` where CLASS_NAME matches the class declared in the file
  - Must start with `__BBFM_` prefix to identify project namespace
  - Single word class: `Driver` → `__BBFM_DRIVER_H_INCL__`
  - Multi-word class: `TestTools` → `__BBFM_TEST_TOOLS_H_INCL__`
  - Insert underscore between each word in PascalCase class names
  - Examples: `Driver` → `__BBFM_DRIVER_H_INCL__`, `SemanticAnalyzer` → `__BBFM_SEMANTIC_ANALYZER_H_INCL__`, `AST` → `__BBFM_AST_H_INCL__`

**Header File Structure:**

- All header files must use 8-byte alignment for types using `#pragma pack`
- Include alignment pragmas at the top (after include guard) and restore at the bottom (before closing include guard)
- Use cross-compiler compatible pragmas for MSVC, GCC, and Clang:

  ```cpp
  // At top of header (after include guard, before includes)
  #pragma pack(push, 8)

  // At bottom of header (before closing include guard)
  #pragma pack(pop)
  ```

**Comments and Documentation:**

- Always use modern C++ line comments (`//`) even for multi-line comments
- Document public APIs with clear Doxygen-style comments in header files
- Use traditional Doxygen syntax:
  - `///` for Doxygen comments
  - `\brief` for brief descriptions
  - `\param` for parameters
  - `\return` for return values
  - Example:

    ```cpp
    /// \brief Sets the episode title
    /// \param title The new title for the episode
    void SetTitle(const std::string& title);
    ```

- Implementation files (.cpp) should use inline `//` comments for logic explanation
- Keep comments concise and focused on "why" rather than "what"

**Documentation Tools:**

- Use Graphviz DOT for class diagrams and dependency diagrams:
  - Use `@dot...@enddot` blocks for custom graphs
  - Good for showing data flow, component relationships, and class structures
  - Use professional styling with white backgrounds and clear fonts
  - Example: `@dot digraph example { ... } @enddot`
- **UML Diagram Guidelines**: Treat String, Guid, Timestamp, Timespan as primitive types
  - These should appear as attributes in class diagrams, not as separate class boxes
  - Focus diagrams on domain model relationships, not implementation utility types
  - Keep diagrams clean by treating runtime utilities as built-in primitives

**Documentation Accuracy:**

- **CRITICAL: Always verify documentation against actual implementation**
- README.md must show real API patterns, not fictional functions
- Use actual struct member names and types from header files
- Integration examples must use real function signatures and member access patterns
- All type names must match implementation (e.g., `Guid` not `GUID`)
- Keep documentation synchronized with code changes

### Project Structure

```text
model-compiler/
├── CMakeLists.txt          # CMake configuration
├── src/                    # Source files
│   ├── model-compiler.l   # Flex lexer specification
│   ├── model-compiler.y   # Bison parser specification
│   ├── Driver.cpp         # Compiler driver implementation
│   ├── AST.cpp            # AST implementation
│   ├── SemanticAnalyzer.cpp # Semantic analysis implementation
│   ├── Console.cpp        # Console output utilities
│   └── main.cpp           # Main entry point (C++)
├── include/               # Header files
│   ├── Common.h           # Common macros and utilities
│   ├── Driver.h           # Compiler driver interface
│   ├── AST.h              # AST node definitions
│   ├── SemanticAnalyzer.h # Semantic analyzer interface
│   └── Console.h          # Console output interface
├── examples/              # Test programs
└── _build/                # Build artifacts (gitignored)
```

### Language & Compiler Design

- **Project Context**: Part of Big Bad Feed Machine (BBFM) - provides common infrastructure for podcast-related applications
- **DSL Purpose**: Define podcast object models and relationships using a simple, expressive language
- **Syntax Style**: C++-like, UML-inspired but simplified - focuses on data modeling without UML's full complexity
- **File Extension**: `.fm` for source files
- **Lexer**: Flex 2.6+
- **Parser**: Bison 3.8+
- **Keywords**: `class` (define types), `enum` (define enumerations), `inherits` (specify inheritance), `feature` (declare class attributes/fields), `invariant` (declare boolean constraints)
- **Naming Conventions**:
  - All type names use PascalCase
  - User-defined types: PascalCase (e.g., `Podcast`, `Episode`)
  - Primitive types: PascalCase (e.g., `String`, `Int`, `Real`)
  - **Case Sensitivity**: Keywords are lowercase and case-sensitive (`class`, `enum`, `feature`, etc.); type names are PascalCase and case-sensitive (`String`, `Int`, `Timestamp`, etc.)
- **Field Declaration Syntax**:
  - Fields are declared using the `feature` keyword followed by name, colon, type, and optional modifiers
  - Syntax: `feature fieldName: TypeName [modifiers];`
  - Example: `feature author: String [optional];`
  - The `feature` keyword makes field declarations explicit and self-documenting
- **Field Modifiers**:
  - Modifiers are specified in square brackets `[]` after the type specification
  - **Default modifier**: `[1]` (mandatory single value) - applied when no modifiers are specified
  - Shorthand syntax: `feature name: String;` is equivalent to `feature name: String [1];`
  - Cardinality: `[1]` (mandatory), `[0..1]` (optional), `[0..*]` (optional array), `[1..*]` (required array with at least one element)
  - Optional modifier: `[optional]` is equivalent to `[0..1]`
  - Constraints: `[unique]` for unique fields
  - Modifiers can be combined: `[optional,unique]` for optional unique field
- **Invariants**:
  - Boolean constraints on class attributes declared with `invariant` keyword
  - Syntax: `invariant name: expression;`
  - Example: `invariant validWidth: width <= 3000;`
  - Supported operators: `<=`, `>=`, `<`, `>`, `==`, `!=`
  - Currently supports simple comparison expressions (attribute operator literal)
  - Invariants are type-checked as boolean expressions

### Language Features

**Type System:**

- User-defined types (for podcast domain objects)
- **Universal metadata fields** - all types automatically have these fields (not user-declared):
  - `typeId` (Guid) - unique identifier for the type itself (same for all instances of a type)
  - `id` (Guid) - unique identifier for the type instance (unique per instance)
  - `cardinality` (Int) - cardinality of the type instance
  - `creationDate` (Timestamp) - when instance was created
  - `modificationDate` (Timestamp) - when instance was last modified
  - `comment` (String) - user comment/notes
- **Inheritance**: Types can inherit from other user types using `class AudioAsset inherits Asset` syntax
- **Primitive types** (String, Int, Real, Bool, Timestamp, Timespan, Date, Guid)
  - `Guid` type for globally unique identifiers (maps to UUID/string in implementation)
  - Used for instance identifiers (e.g., `Podcast.id`)
  - `Timestamp` type for points in time (maps to double/seconds since epoch)
  - `Timespan` type for durations (maps to double/seconds)
  - `Int` for integers
  - `Real` for floating-point numbers
  - `Bool` for boolean values (true/false)
  - `String` for text
- Enums
- Dynamic arrays
- Nested types

**Relationships:**

- One-to-one
- One-to-many
- Many-to-many

**Constraints:**

- Cardinality modifiers:
  - `[1]` - mandatory single value
  - `[0..1]` - optional single value
  - `[1..*]` - array with at least one element
  - `[0..*]` - array that may be empty
- Unique constraints: `[unique]`
- Modifiers can be combined: `[1,unique]`, `[0..1,unique]`, etc.

**Compilation Phases:**

1. Lexical Analysis (tokenization) ✅ Implemented
2. Syntax Analysis (parsing) ✅ Implemented
3. **AST Construction** ✅ Implemented
4. **Semantic Analysis** ✅ Implemented
   - Symbol table construction
   - Type checking and validation
   - Inheritance cycle detection
   - Field uniqueness validation (including inherited fields)
   - Invariant validation (field reference checking)
5. Code Generation:
   - Swift class generation - To be implemented

**Current Status:** The lexer, parser, AST construction, and semantic analysis are complete. The compiler successfully parses the BBFM modeling language, builds an Abstract Syntax Tree, and performs comprehensive semantic validation including type checking, inheritance validation, and constraint verification.

**Architecture:** The compiler uses a `Driver` class to orchestrate compilation phases. The `main.cpp` file only handles command-line argument parsing and delegates all compilation work to the Driver:

- **Phase 0** (Driver::Phase0): Lexical analysis and parsing - returns the AST as a unique_ptr
- **Phase 1** (Driver::Phase1): Semantic analysis - validates the AST and builds symbol table, returns SemanticAnalyzer as unique_ptr
- **Phase 2** (to be implemented): Code generation - Swift classes

The Driver does not store the AST or analyzer internally; instead, each phase returns ownership to the caller, allowing for flexible resource management. The root AST node is represented by the `AST` class.

### Example BBFM Syntax

```cpp
// All types automatically have these universal metadata fields (not user-declared):
//   - Guid typeId (unique per type)
//   - Guid id (unique per instance)
//   - Int cardinality (cardinality of instance)
//   - Timestamp creationDate (when instance created)
//   - Timestamp modificationDate (when instance modified)
//   - String comment (user notes)

enum MediaType {
    AUDIO,
    VIDEO
}

class Asset {
    // Universal metadata fields are automatic (not declared)
    feature url: String;
}

class AudioAsset inherits Asset {
    // Inherits from Asset + has universal metadata
    feature format: String;
    feature fileSize: Int;

    invariant maxFileSize: fileSize <= 500000000;  // Max 500MB
}

class VideoAsset inherits Asset {
    // Inherits from Asset + has universal metadata
    feature width: Int;
    feature height: Int;
    feature duration: Timespan;

    invariant validWidth: width <= 3840;    // Max 4K width
    invariant validHeight: height <= 2160;  // Max 4K height
}

class Podcast {
    // Universal metadata fields are automatic (not declared)
    feature title: String;
    feature description: String;
    feature author: String [optional];         // optional field using optional modifier
    feature rssUrl: String [1,unique];         // mandatory + unique (explicit modifiers)
    feature episodes: Episode [0..*];          // one-to-many (may be empty)
}

class Episode {
    feature title: String;
    feature publishedAt: Date;
    feature duration: Timespan;
    feature mediaType: MediaType;
    feature audio: AudioAsset;                // one-to-one relationship
    feature transcript: Transcript [optional]; // optional one-to-one using optional modifier
}

class Transcript {
    feature text: String;
    feature language: String;
}
```

### Type Mapping

| BBFM Type | Swift |
|---------|-------|
| `String` | `String` |
| `Int` | `Int64` |
| `Real` | `Double` |
| `Bool` | `Bool` |
| `Timestamp` | `Double` |
| `Timespan` | `Double` |
| `Guid` | `String` |

### Build Commands

```bash
# Build using the build script
./build.sh

# Or manually with CMake and Ninja
cd _build
cmake -G Ninja ..
ninja

# Clean
ninja clean

# Test
_build/model-compiler examples/podcast.fm
```

---

## Recent Updates & Decisions

### November 1, 2025 (22:00)

- **Expression system complete**: Fully integrated expression system with lexer, parser, and semantic analysis
- **Lexer additions**:
  - Added arithmetic operator tokens: `PLUS` (+), `MINUS` (-), `SLASH` (/), `PERCENT` (%)
  - Added logical operator tokens: `AND` (&&), `OR` (||), `NOT` (!)
  - Note: `ASTERISK` (*) and parentheses already existed
- **Parser enhancements**:
  - Added expression union type for AST node pointers
  - Implemented complete expression grammar with operator precedence
  - Precedence levels (low to high): OR, AND, comparison (==, !=), relational (<, >, <=, >=), additive (+, -), multiplicative (*, /, %), unary (-, !)
  - Binary operators: arithmetic (+, -, *, /, %), comparison (<, >, <=, >=, ==, !=), logical (&&, ||)
  - Unary operators: negation (-), logical NOT (!)
  - Parenthesized expressions: `(expr)`
  - Primary expressions: literals (int, real, string, bool) and field references
- **Invariant class updated**:
  - Changed from storing `std::string expression_` to `std::unique_ptr<Expression> expression_`
  - Constructor now takes `std::unique_ptr<Expression>` instead of string
  - `GetExpression()` returns `const Expression*` instead of `const std::string&`
  - `Dump()` method uses `expression_->ToString()` for display
- **Invariant parser simplified**:
  - Replaced 6 separate invariant rules (one per operator) with single rule: `INVARIANT IDENTIFIER COLON expression SEMICOLON`
  - Parser now builds complete Expression AST nodes from grammar
  - Much cleaner and more extensible architecture
- **Semantic analyzer updated**:
  - Replaced string parsing in `ValidateInvariants()` with AST traversal
  - Added `CollectFieldReferences()` method to recursively extract field names from expressions
  - Handles all expression types: BinaryExpression, UnaryExpression, FieldReference, LiteralExpression, FunctionCall, ParenthesizedExpression
  - `DumpSymbolTable()` updated to call `expression->ToString()` instead of displaying string
- **Files modified**:
  - `src/model-compiler.l` - Added operator tokens (+20 lines)
  - `src/model-compiler.y` - Added expression grammar, operator precedence, updated invariant rule (+110 lines)
  - `include/AST.h` - Updated Invariant class signature
  - `src/AST.cpp` - Updated Invariant implementation
  - `include/SemanticAnalyzer.h` - Added CollectFieldReferences declaration
  - `src/SemanticAnalyzer.cpp` - Implemented expression-based invariant validation (+50 lines)
  - `examples/test_expressions.fm` - New comprehensive expression test file
- **Testing**: ✅ All expression types work correctly
  - Arithmetic: `width * height <= maxArea`
  - Complex: `(width + height) * 2 <= 1000`
  - Logical: `width >= 10 && height >= 10`
  - Unary: `-273.15`, `!(minLength == 0)`
  - Real numbers: `fahrenheit == celsius * 1.8 + 32.0`
- **Build status**: ✅ Project builds successfully, all tests pass
- **Impact**: Invariants are now fully expression-based, enabling complex constraints beyond simple comparisons
- **Next steps**: This expression system can be extended to support function bodies, computed properties, and default values
- **Reasoning**: Integrating the expression system with the parser and semantic analyzer completes the foundation for a powerful constraint and computation system. Moving from string-based to AST-based expressions enables proper type checking, optimization, and code generation. The grammar-based approach is much more maintainable than the previous string pattern matching and allows for natural language-like constraint expressions.

### November 1, 2025 (21:30)

- **Expression system foundation**: Implemented complete expression class hierarchy in AST
- **Classes added** (7 new expression types):
  - `Expression` - Abstract base class with Type enum (INT, REAL, BOOL, STRING, TIMESTAMP, TIMESPAN, GUID, UNKNOWN)
  - `BinaryExpression` - Arithmetic (+, -, *, /, %), comparison (<, >, <=, >=, ==, !=), and logical (&&, ||) operators
  - `UnaryExpression` - Negation (-) and logical NOT (!) operators
  - `FieldReference` - Reference to class field by name
  - `LiteralExpression` - Integer, real, string, and boolean literals
  - `FunctionCall` - Function invocation with arguments
  - `ParenthesizedExpression` - Grouped expressions
- **Methods implemented**: GetResultType(), ToString(), Dump(), type-specific getters
- **Type inference**: BinaryExpression implements basic type widening (INT + REAL → REAL), comparison operators return BOOL
- **Files modified**:
  - `include/AST.h` - Added forward declarations and complete class hierarchy
  - `src/AST.cpp` - Added all expression class implementations (approximately 300 lines)
- **Build status**: ✅ Project builds successfully with new expression system
- **Next steps**: Add lexer tokens for arithmetic operators, implement parser grammar with precedence, integrate with invariants
- **Reasoning**: This lays the foundation for a general expression system that can be used in invariants, computed properties, and function bodies. Rather than restricting expressions to just invariants, this architecture enables future features like computed fields (`function Void duration() -> Timespan { return endTime - startTime; }`), default values, and derived attributes. The design follows standard compiler architecture with separate expression node types for each construct, making semantic analysis and code generation straightforward.

### November 1, 2025 (21:00)

- **Symbol table dump enhancement**: Added origin markers for inherited vs. local features and invariants
- **Changes made**:
  - Modified `DumpSymbolTable()` method in SemanticAnalyzer to prefix features and invariants with origin markers
  - Inherited features/invariants show `<base>` prefix
  - Locally declared features/invariants show `<self>` prefix
  - Uses set-based lookup to determine if a feature/invariant is local to the class or inherited
- **Example output**:

  ```text
  class AudioAsset inherits Asset {
    Features:
      <base> url: String [1..1]
      <self> format: String [1..1]
      <self> fileSize: Int [1..1]
    Invariants:
      <self> maxFileSize: fileSize <= 500000000
  }
  ```

- **Files modified**: `src/SemanticAnalyzer.cpp`
- **Testing**: Verified with comprehensive_test.fm and podcast.fm examples showing correct origin markers
- **Reasoning**: The symbol table dump now clearly indicates which features and invariants are inherited versus locally declared, making it much easier to understand class hierarchies and identify where members are defined. This is especially useful when debugging inheritance chains and understanding the complete interface of derived classes. The `<base>`/`<self>` markers provide immediate visual feedback about member origin without requiring developers to trace through base classes manually.

### November 1, 2025 (20:45)

- **FabricDeclaration to ClassDeclaration refactoring**: Renamed FabricDeclaration class to ClassDeclaration throughout the entire codebase
- **Scope**: Systematic rename across all files (AST.h/cpp, SemanticAnalyzer.h/cpp, model-compiler.y)
- **Changes made**:
  - AST.h: Renamed class from FabricDeclaration to ClassDeclaration, updated forward declaration, renamed Declaration::Kind::FABRIC to CLASS, renamed AsFabric() to AsClass()
  - AST.cpp: Updated all method implementations and comments
  - SemanticAnalyzer.h: Updated TypeSymbol.fabricDecl to TypeSymbol.classDecl, renamed ValidateFabricDeclaration() to ValidateClassDeclaration(), updated all method parameters
  - SemanticAnalyzer.cpp: Updated all occurrences (variable names, method calls, symbol table accesses)
  - model-compiler.y: Updated parser union (fabricDecl→classDecl), renamed fabric_declaration to class_declaration, updated all parser actions creating ClassDeclaration instances
- **Variable naming**: Changed local variables from `fabric`/`fabricDecl` to `classDecl` for consistency
- **Testing**: Build succeeded, symbol table dump shows correct functionality with inheritance
- **Reasoning**: The "Fabric" terminology was a historical artifact from when the language used the `fabric` keyword (before switching to `class` keyword on November 1, 2025 22:45). The refactoring aligns the C++ codebase with the current language syntax, removing confusion and making code more maintainable. Using `ClassDeclaration` is more intuitive and accurately reflects that these AST nodes represent class declarations in the BBFM modeling language.

### November 1, 2025 (20:30)

- **Symbol table dump enhancement**: Updated symbol table dump to show inherited features and invariants
- **Changes made**:
  - Added `GetAllInvariants()` and `GetAllInvariantsHelper()` methods to collect inherited invariants (similar to existing `GetAllFields()`)
  - Updated `DumpSymbolTable()` to use `GetAllFields()` and `GetAllInvariants()` instead of only showing directly declared members
  - Made helper methods const to allow usage from const `DumpSymbolTable()` method
- **Impact**: Symbol table dump now shows complete interface of each class including inherited features and invariants
- **Example**: `Region inherits Tag` now displays both Tag's features (name, timestamp) and invariants (validTimestamp) along with Region's own members
- **Files modified**:
  - `include/SemanticAnalyzer.h` - added method declarations
  - `src/SemanticAnalyzer.cpp` - implemented GetAllInvariants methods and updated DumpSymbolTable
- **Reasoning**: Symbol table dump is a diagnostic tool, so showing the complete interface (including inherited members) provides better visibility into what's available on each type. This is especially useful when debugging inheritance hierarchies and understanding the full contract of derived classes.

### November 1, 2025 (20:15)

- **Swift as primary code generation target**: Changed target language from C++ to Swift
- **Files modified**:
  - `src/main.cpp` - updated usage message to "Compiles .fm source files to Swift"
  - `AGENTS.md` - updated all references from C++ to Swift throughout
  - `README.md` - updated all references from C++ to Swift throughout
- **Changes made**:
  - Updated Target Outputs to Swift
  - Updated Type Mapping table (C++ types to Swift types)
  - Updated Phase 2 description to Swift class generation
  - Updated class prefix example to reference Swift instead of C++
  - Updated historical references to show Swift as target
- **Type mappings**:
  - String → String (was std::string)
  - Int → Int64 (was int64_t)
  - Real → Double (was double)
  - Bool → Bool (was bool)
  - Timestamp → Double (was double)
  - Timespan → Double (was double)
  - Guid → String (was std::string)
- **Reasoning**: Swift is a better initial target language for the BBFM project as it's widely used in podcast applications (iOS, macOS) and has cleaner syntax for generated code. Swift's strong type system, optional handling, and modern language features align well with the BBFM modeling language design. The compiler infrastructure (C++23, Flex, Bison) remains unchanged; only the code generation target is different.

### November 1, 2025 (20:00)

- **SQL/Database feature removal**: Removed all references to SQL and database generation from codebase and documentation
- **Files modified**:
  - `src/main.cpp` - removed "and SQL" from usage message
  - `AGENTS.md` - removed database schema references, SQLite type mappings, foreign key constraints
  - `README.md` - removed database schema references and SQLite type mappings
- **Changes made**:
  - Removed SQLite from Target Outputs section
  - Removed foreign key constraints from language features
  - Updated Type Mapping tables to show only Swift mappings
  - Updated Phase 2 description to only mention Swift code generation
  - Updated historical references to remove SQLite/database mentions
- **Reasoning**: Database/SQL schema generation will be added back later once the Swift code generator is working. Removing these references now simplifies the current focus on implementing Phase 2 (Swift code generation) without the distraction of unimplemented database features. This keeps the documentation accurate to what the compiler currently does and plans to do in the immediate future.

### November 1, 2025 (19:45)

- **Class prefix option**: Added new `--class-prefix` command line option for code generation
- **Features added**:
  - Command line option `--class-prefix` to specify a prefix for generated class and enum names
  - Default value is empty string (no prefix)
  - Prefix stored in Driver class for use in Phase 2 (code generation)
  - New `GetClassPrefix()` method in Driver class to retrieve the prefix
  - Status message displayed when prefix is set
- **Files modified**:
  - `src/main.cpp` - added command line option, passing prefix to Driver, and status message
  - `include/Driver.h` - added classPrefix parameter to constructor and GetClassPrefix() method
  - `src/Driver.cpp` - updated constructor to accept and store classPrefix, implemented GetClassPrefix()
- **Usage**: `model-compiler --class-prefix "BBFM" source.fm`
- **Testing**: Verified option appears in help, compiles correctly with and without prefix
- **Reasoning**: When generating code from BBFM models, users may need to add prefixes to class and enum names to avoid naming conflicts with existing code or to follow project naming conventions. For example, a BBFM model defining a `Podcast` class might generate `BBFMPodcast` in Swift to distinguish it from other Podcast classes in the project. This option prepares the infrastructure for Phase 2 (code generation) where the prefix will be applied to all generated type names. The prefix is stored in the Driver class where it can be accessed during code generation.

### November 1, 2025 (19:30)

- **Symbol table dump feature**: Added new `--dump-symtab` command line option
- **Features added**:
  - Command line option `--dump-symtab` to display symbol table after semantic analysis
  - New `DumpSymbolTable()` method in SemanticAnalyzer class
  - Comprehensive symbol table output showing:
    - Total symbol count by category (primitives, enums, classes)
    - All primitive types
    - Enumerations with their values
    - Classes with inheritance, features (fields), and invariants
    - Field cardinality and constraint modifiers
- **Files modified**:
  - `src/main.cpp` - added command line option and call to DumpSymbolTable()
  - `include/SemanticAnalyzer.h` - added DumpSymbolTable() method declaration
  - `src/SemanticAnalyzer.cpp` - implemented DumpSymbolTable() method
- **Output format**: Clean, hierarchical display with section headers and proper indentation
- **Testing**: Verified with podcast.fm and comprehensive_test.fm examples
- **Reasoning**: The symbol table is a critical compiler data structure that developers need to inspect during development and debugging. Providing a formatted dump option allows developers to verify that types are being correctly registered and analyzed. This complements the --dump-ast option by showing the semantic view of the program after Phase 1, rather than just the syntactic structure from Phase 0. The feature will be especially useful when implementing Phase 2 (code generation) to verify symbol table contents before generating code.

### November 1, 2025 (19:15)

- **Command line parsing implementation**: Implemented cxxopts-based command line parsing in main.cpp
- **Features added**:
  - `--help` / `-h`: Display usage information
  - `--version` / `-v`: Display version information (v0.1.0)
  - `--dump-ast`: Dump the Abstract Syntax Tree after parsing (optional)
  - Positional argument for input source file(s)
  - Automatic help display when no arguments provided
  - Professional error messages for parsing errors
- **Error handling**: Added exception handling for cxxopts parsing errors and general exceptions
- **User experience improvements**:
  - Clear usage message: "BBFM Model Compiler - Compiles .fm source files to Swift"
  - Helpful error messages when no input file specified
  - Standard command line option conventions
- **File modified**: src/main.cpp
- **Testing**: Verified all options work correctly (--help, --version, normal compilation)
- **Reasoning**: Using cxxopts provides a professional, user-friendly command line interface with automatic help generation and standard option parsing. The implementation follows modern C++ best practices with exception handling and clear error reporting. This makes the compiler much more polished and easier to use.

### November 1, 2025 (19:00)

- **Command line parser integration**: Added cxxopts library for command line argument parsing
- **Integration method**: Using CMake FetchContent to fetch cxxopts v3.3.1 from GitHub
- **Configuration**:
  - Disabled CXXOPTS_BUILD_EXAMPLES (don't build examples)
  - Disabled CXXOPTS_BUILD_TESTS (don't build tests)
  - Disabled CXXOPTS_ENABLE_INSTALL (don't install cxxopts)
  - Disabled CXXOPTS_ENABLE_WARNINGS (don't add cxxopts warnings)
  - Disabled CXXOPTS_USE_UNICODE_HELP (don't use unicode in help)
- **Linking**: Static library linking via `target_link_libraries(model-compiler PRIVATE cxxopts::cxxopts)`
- **Benefits**: No runtime dependencies, professional command line parsing, automatic help generation
- **CMakeLists.txt updated**: Added FetchContent setup and cxxopts configuration
- **Build verified**: Project builds successfully with cxxopts integrated
- **Reasoning**: Using FetchContent ensures cxxopts is automatically downloaded and built during the CMake configure step, eliminating the need for manual dependency management. Static linking ensures the executable has no runtime dependencies. Disabling unnecessary features keeps the build clean and fast. The cxxopts library provides a modern, type-safe C++ interface for parsing command line arguments with automatic help text generation, which is much better than manual argument parsing.

### November 1, 2025 (18:45)

- **File extension change**: Updated source file extension from `.bbfm` to `.fm`
- **Files updated**: README.md and AGENTS.md
- **Files renamed**: All example files in `examples/` directory renamed from `.bbfm` to `.fm`
- **Changes made**:
  - Updated file extension in Language & Compiler Design section (`.bbfm` → `.fm`)
  - Updated all usage examples and build commands
  - Updated project structure documentation
  - Updated all historical references in Recent Updates section
  - Renamed all 10 example files:
    - comprehensive_test.bbfm → comprehensive_test.fm
    - error_test_suite.bbfm → error_test_suite.fm
    - podcast.bbfm → podcast.fm
    - test_bad_invariant.bbfm → test_bad_invariant.fm
    - test_circular_inheritance.bbfm → test_circular_inheritance.fm
    - test_duplicate_field.bbfm → test_duplicate_field.fm
    - test_error_column.bbfm → test_error_column.fm
    - test_missing_semicolon.bbfm → test_missing_semicolon.fm
    - test_undefined_type.bbfm → test_undefined_type.fm
    - test_wrong_case.bbfm → test_wrong_case.fm
- **Testing**: Verified compiler works correctly with new `.fm` extension
- **Reasoning**: The `.fm` extension is simpler and cleaner while still being distinctive. It stands for "Feed Machine" (from BBFM - Big Bad Feed Machine), making it clear this is a modeling language for the BBFM project. The shorter extension is easier to type and more professional-looking than the longer `.bbfm` extension.

### November 1, 2025 (18:30)

- **Invariant inheritance test**: Added test case to comprehensive_test.fm demonstrating that invariants are inherited along with features
- **Test classes added**:
  - `BaseWithInvariant` - base class with features (width, height) and invariants (validWidth, validHeight)
  - `DerivedWithInvariant` - inherits features and invariants from BaseWithInvariant, adds depth feature and validDepth invariant
  - `FurtherDerivedWithInvariant` - multi-level inheritance, inherits all ancestor features and invariants, adds color feature and nonEmptyColor invariant
- **What it tests**: Verifies that derived classes inherit both features AND invariants from their base classes, not just features
- **Inheritance chain**: BaseWithInvariant → DerivedWithInvariant → FurtherDerivedWithInvariant
- **Features inherited**: width, height → depth → color
- **Invariants inherited**: validWidth, validHeight → validDepth → nonEmptyColor
- **Testing**: Verified comprehensive test file still compiles successfully through both Phase 0 and Phase 1
- **Reasoning**: While the current semantic analyzer validates invariants reference valid fields (including inherited fields), this test case explicitly documents and tests that invariants themselves are inherited in class hierarchies. This is an important language feature that should be demonstrated in the comprehensive test suite and will be relevant for code generation where inherited invariants need to be enforced in derived classes.

### November 1, 2025 (18:15)

- **Test file cleanup**: Removed redundant test files after consolidating into comprehensive_test.fm
- **Files removed** (15 redundant valid test files):
  - test_all_literals.fm
  - test_bool.fm
  - test_comprehensive.fm (old version)
  - test_duration.fm
  - test_inheritance.fm
  - test_inherits.fm
  - test_invariant.fm
  - test_literals.fm
  - test_optional.fm
  - test_question_mark.fm
  - test_shorthand.fm
  - test_simple.fm
  - test_tag.fm
  - test_timestamp_field.fm
  - test_timestamp_name.fm
- **Files retained**:
  - comprehensive_test.fm - complete test suite with all valid features
  - error_test_suite.fm - documentation of error test cases
  - podcast.fm - example file for real-world usage
  - Error test files (7 files for testing error reporting):
    - test_bad_invariant.fm
    - test_circular_inheritance.fm
    - test_duplicate_field.fm
    - test_error_column.fm
    - test_missing_semicolon.fm
    - test_undefined_type.fm
    - test_wrong_case.fm
- **Reasoning**: Removed duplicate and redundant test files since comprehensive_test.fm now covers all valid language features. Kept error test files for validating error reporting. This simplifies the test suite while maintaining complete coverage. The examples directory is now cleaner and easier to navigate with a clear separation between valid tests, error tests, and example files.

### November 1, 2025 (18:00)

- **Test suite consolidation**: Created comprehensive test file combining all valid test cases
- **Files created**:
  - `comprehensive_test.fm` - Single comprehensive test file with all language features organized by category
  - `error_test_suite.fm` - Documentation file listing all error test cases (not meant to compile)
- **Categories tested in comprehensive file**:
  - Enumerations (MediaType, Status)
  - Primitive types (all 8 primitive types)
  - Field modifiers (mandatory, optional, arrays, unique, combined modifiers)
  - Field names matching type keywords (string, int, timestamp, etc.)
  - Inheritance (single and multi-level)
  - Invariants (all comparison operators with all literal types)
  - Relationships (one-to-one, one-to-many, many-to-many)
  - Complex domain models (Podcast, Episode, Book, etc.)
  - Boolean types and feature flags
- **Error test files preserved**: Kept individual error test files for testing error reporting
  - test_missing_semicolon.bbfm
  - test_error_column.bbfm
  - test_wrong_case.bbfm
  - test_undefined_type.bbfm
  - test_circular_inheritance.bbfm
  - test_duplicate_field.bbfm
  - test_bad_invariant.bbfm
- **Testing**: Verified comprehensive test file compiles successfully through both Phase 0 and Phase 1
- **Reasoning**: Having a single comprehensive test file makes it easier to validate all language features at once and serves as a complete example of the BBFM modeling language. Individual error test files are preserved for testing error reporting. The comprehensive test is organized by category with clear section headers making it easy to understand and maintain. This approach provides both a complete regression test and excellent documentation of all language features.

### November 1, 2025 (17:30)

- **README documentation update**: Updated README.md to reflect latest project state
- **Changes made**:
  - Updated case-sensitivity description (keywords and types are now case-sensitive, not case-insensitive)
  - Fixed duplicate code block in example section
  - Updated build instructions to include build.sh script
  - Updated usage examples to show correct binary path (_build/model-compiler)
  - Updated project structure to include SemanticAnalyzer.cpp and Console.cpp files
  - Updated compilation phases to show Phase 0 and Phase 1 as implemented with checkmarks
  - Expanded current status section with detailed breakdown of implemented features
  - Added visual indicators (✅ for implemented, 🚧 for planned)
  - Documented semantic analysis features (symbol table, type validation, inheritance cycle detection, etc.)
- **Reasoning**: README must accurately reflect the current state of the project. The semantic analysis phase is fully implemented, case-sensitivity has been enforced, and new source files have been added. Keeping documentation synchronized with code changes prevents confusion and provides accurate information to users and contributors.

### November 2, 2025 (00:05)

- **Parser error output complete**: Finalized Console class integration in model-compiler.y
- **Change**: Updated remaining `std::cerr` calls in yyerror() to use Console::ReportError()
- **Details**: Source line display and caret pointer now use Console class with ostringstream for formatting
- **Result**: All compiler output now goes through Console class for consistent message handling
- **Reasoning**: Completing the Console class integration ensures all output is centralized and can be easily controlled or redirected. The error formatting remains identical but now uses the same output mechanism as the rest of the compiler.

### November 2, 2025 (00:00)

- **Include guard naming convention update**: Updated include guard format to include BBFM project prefix
- **New format**: `__BBFM_CLASS_NAME_H_INCL__` where CLASS_NAME matches the class in the file
- **Prefix requirement**: All include guards must start with `__BBFM_` to identify project namespace
- **Examples updated**:
  - `Driver` → `__BBFM_DRIVER_H_INCL__` (was `__DRIVER_H_INCL__`)
  - `SemanticAnalyzer` → `__BBFM_SEMANTIC_ANALYZER_H_INCL__` (was `__SEMANTIC_ANALYZER_H_INCL__`)
  - `AST` → `__BBFM_AST_H_INCL__` (was `__AST_H_INCL__`)
- **Reasoning**: Adding the BBFM prefix to include guards prevents potential naming conflicts with other libraries or system headers. This follows best practices for project-specific include guards and makes it immediately clear which project a header belongs to. The prefix serves as a namespace for header guards, similar to how the `bbfm` namespace protects C++ symbols.

### November 1, 2025 (23:55)

- **Console class integration**: Updated codebase to use Console class for all error and status messages
- **Files updated**: Driver.cpp, SemanticAnalyzer.cpp, main.cpp, model-compiler.y
- **Changes**:
  - Replaced `std::cerr` calls with `Console::ReportError()`
  - Replaced `std::cout` status messages with `Console::ReportStatus()`
  - Parser (model-compiler.y) now uses Console for error messages
  - AST dump output remains using `std::cout` directly (structured data output)
- **Benefits**: Centralized output control, consistent message formatting, easier to redirect or modify output behavior
- **Testing**: Verified successful compilation, normal output, and error reporting
- **Reasoning**: Using the Console class throughout the codebase provides a single point of control for all compiler messages. This makes it easier to add features like colored output, logging to files, or different output formats in the future. The separation of error (stderr) and status (stdout) streams is maintained while providing a cleaner API.

### November 1, 2025 (23:50)

- **Console utility class**: Added new Console class for standardized output
- **Static methods**: ReportError() outputs to stderr, ReportStatus() outputs to stdout
- **Files added**:
  - `include/Console.h` - Console class interface
  - `src/Console.cpp` - Console class implementation
- **CMakeLists.txt updated**: Added Console.cpp to build
- **Design**: Static-only class with deleted constructors to prevent instantiation
- **Reasoning**: Provides a centralized, consistent interface for all compiler output. Using static methods makes the API simple and convenient to call from anywhere in the codebase without requiring object instantiation. Separating error (stderr) and status (stdout) output follows Unix conventions and allows users to redirect output streams independently.

### November 1, 2025 (23:45)

- **Implementation separation**: Moved inline method implementations from AST.h to AST.cpp
- **Guideline added**: Method implementations should be in .cpp files, not inline in headers
- **Methods moved**: Moved 30+ getter and utility methods from inline in header to out-of-line in implementation file
- **Exception noted**: Tightly coupled class hierarchies (like AST nodes) may share a single header/implementation file pair
- **Benefit**: Reduces recompilation of dependencies when implementation changes; improves compilation times
- **Files updated**: AST.h (declarations only), AST.cpp (added implementations)
- **Testing**: Verified build succeeds and compiler works correctly with example files
- **Reasoning**: Separating interface from implementation is a core C++ best practice. Inline implementations in headers force recompilation of all dependent files when any implementation detail changes. Moving implementations to .cpp files means only the implementation file needs recompilation, significantly improving build times in larger projects. This change follows the guideline while recognizing that AST nodes are a cohesive unit that benefits from staying in a single header/implementation pair rather than being split across multiple files.

### November 1, 2025 (23:30)

- **Class file organization**: Added coding guideline requiring separate header and implementation files for each class
- **Guideline**: Each class must have its own .h and .cpp file with filenames matching the class name exactly
- **File locations**: Header files in `include/` directory, implementation files in `src/` directory
- **Example**: `Driver` class must be defined in `Driver.h` (in `include/`) and implemented in `Driver.cpp` (in `src/`)
- **Exception**: Template classes may have implementation in header file if needed
- **Reasoning**: Enforces clear organization and makes it easy to locate class definitions and implementations. This convention is standard in professional C++ projects and improves maintainability by establishing a predictable file structure. Having one class per file pair also reduces coupling and makes the codebase easier to navigate.

### November 1, 2025 (23:15)

- **Class scope declaration order**: Added coding guideline for consistent scope ordering in classes
- **Guideline**: Always declare scopes in the order: `public`, `protected`, `private`
- **Rationale**: Makes the public interface immediately visible when reading class definitions, improving code readability and maintainability
- **Documentation**: Added example showing proper scope ordering with clear separation between public, protected, and private sections
- **Reasoning**: Consistent scope ordering improves code readability by presenting the most important information (public API) first. This follows the principle of "most important first" and makes it easier for developers to quickly understand a class's public interface without scrolling through implementation details. This convention is also widely adopted in modern C++ codebases and style guides.

### November 1, 2025 (23:00)

- **Semantic analysis implementation**: Implemented Phase 1 of the compiler with comprehensive semantic validation
- **SemanticAnalyzer class**: Created new analyzer class with symbol table and validation logic
- **Symbol table**: Built symbol table containing primitive types, enums, and user-defined classes
- **Type validation**: Validates all type references in field declarations and base classes
- **Inheritance validation**:
  - Checks that base types exist and are classes (not enums or primitives)
  - Detects circular inheritance chains (e.g., A inherits B, B inherits A)
  - Uses cycle-safe recursion with visited set tracking
- **Field uniqueness**: Validates that field names are unique within a class including inherited fields
- **Invariant validation**: Checks that invariants reference valid fields (including inherited fields)
- **Error reporting**: Semantic errors reported with clear messages indicating the problem and location
- **Files added**:
  - `include/SemanticAnalyzer.h` - Semantic analyzer interface
  - `src/SemanticAnalyzer.cpp` - Semantic analyzer implementation
- **Files updated**:
  - `include/Driver.h` - Added Phase1() method
  - `src/Driver.cpp` - Implemented Phase1() to run semantic analysis
  - `src/main.cpp` - Updated to call Phase1 after Phase0
  - `CMakeLists.txt` - Added SemanticAnalyzer.cpp to build
- **Test files**: Created error test cases for undefined types, circular inheritance, duplicate fields, and invalid invariants
- **Cycle detection fix**: Fixed infinite recursion bug in GetAllFields() by adding cycle detection using visited set
- **Reasoning**: Semantic analysis is essential for catching errors before code generation. The implementation provides comprehensive validation of the BBFM language semantics including type safety, inheritance correctness, and constraint validation. The symbol table architecture prepares the foundation for code generation in Phase 2. Cycle detection in both inheritance checking and field collection prevents infinite recursion and provides clear error messages when cycles are detected.

### November 2, 2025 (01:00)

- **Enhanced error diagnostics**: Improved parser error reporting to show filename, line, column, and source context
- **Column tracking**: Added column number tracking in lexer using `yycolumn` variable
- **Location tracking**: Enabled Bison `%locations` directive for full position tracking
- **Source line caching**: Driver now reads and caches source file lines for error display
- **Error format**: Changed from basic "Error at line X" to compiler-style "file:line:column: error: message"
- **Visual error pointer**: Shows the actual source line with a caret (^) pointing to the error column
**Reasoning**: Professional error diagnostics are essential for usability. The enhanced format matches modern compiler conventions (GCC, Clang, Rust) making errors easier to locate and fix. Showing the source line with a visual pointer dramatically improves the user experience when debugging syntax errors.

### November 2, 2025 (00:45)

- **Case-sensitivity simplification**: Made the language fully case-sensitive for keywords and type names
- **Lexer simplification**: Replaced verbose case-insensitive patterns (e.g., `[cC][lL][aA][sS][sS]`) with simple string literals (e.g., `"class"`)
- **Rationale**:
  - Massive code reduction: ~30 lines of complex patterns reduced to ~10 clean lines
  - Better maintainability: Adding new keywords is now trivial
  - Modern convention: All modern DSLs (Rust, Swift, Kotlin, TypeScript) are case-sensitive
  - Improved tooling: Better IDE/editor support for syntax highlighting and autocomplete
  - Consistency: Eliminates confusion between case-insensitive keywords and case-sensitive identifiers
- **Breaking change**: Users must now write exact case (`class`, not `CLASS`; `String`, not `string`)
- **Files updated**: model-compiler.l (lexer), AGENTS.md
- **Testing**: Verified all existing test files work; confirmed incorrect case is properly rejected
- **Reasoning**: We already removed `%option case-insensitive` to fix parser ambiguities, so we were using verbose patterns to simulate case-insensitivity. This change completes the transition to full case-sensitivity, making the codebase cleaner and following modern language design best practices.

### November 2, 2025 (00:30)

- **Parser ambiguity fix**: Fixed issue where field names matching type keywords caused parser errors
- **Problem**: Statement like `feature timestamp: Timestamp;` failed because both the field name and type name were tokenized as `TIMESTAMP_TYPE`
- **Solution**: Removed `%option case-insensitive` from lexer and implemented explicit case-insensitive patterns for keywords only (e.g., `[tT][iI][mM][eE][sS][tT][aA][mM][pP]`)
- **Grammar enhancement**: Added `field_name` and `attribute_name` grammar rules that allow type keywords to be used as identifiers in field declarations and invariant expressions
- **Invariant literal support**: Extended invariants to support INTEGER, REAL, STRING, and BOOL literals
- **Lexer additions**:
  - `REAL_LITERAL` token for floating-point numbers (pattern: `{DIGIT}+\.{DIGIT}+([eE][+-]?{DIGIT}+)?`)
  - `STRING_LITERAL` token for quoted strings (pattern: `\"([^\"\\]|\\.)*\"`)
  - `BOOL_LITERAL` token for true/false values (case-insensitive patterns: `[tT][rR][uU][eE]`, `[fF][aA][lL][sS][eE]`)
- **Parser additions**:
  - `literal_value` grammar rule that accepts INTEGER_LITERAL, REAL_LITERAL, STRING_LITERAL, or BOOL_LITERAL
  - `field_name` rule allows type keywords (String, Int, Timestamp, etc.) as field names
  - `attribute_name` rule allows type keywords as attribute names in invariant expressions
- **Type support in invariants**: Timestamp and Timespan work with REAL_LITERAL since they map to double internally
- **Files updated**: model-compiler.l (lexer), model-compiler.y (parser)
- **Testing**: Verified with test cases including field names matching type names and invariants with all literal types
- **Reasoning**: The ambiguity between type keywords and identifiers is a common problem in DSLs. By removing global case-insensitivity and making keywords case-insensitive only through explicit patterns, we allow identifiers (field names, attribute names) to be case-sensitive and potentially match type keyword spellings. The grammar rules `field_name` and `attribute_name` explicitly allow type tokens to be used as identifiers where appropriate, solving the conflict. This provides maximum flexibility for users who want to name fields naturally (e.g., "timestamp" for a Timestamp field) while maintaining clear keyword recognition.

### November 1, 2025 (23:45)

- **Invariant feature**: Added `invariant` keyword for declaring boolean constraints on class attributes
- **Syntax**: `invariant name: expression;` where expression is a comparison of an attribute against a literal
- **Supported operators**: `<=`, `>=`, `<`, `>`, `==`, `!=`
- **Example**: `invariant validWidth: width <= 3000;`
- **AST updates**: Created new `Invariant` class to represent constraint declarations
- **FabricDeclaration updates**: Added invariants vector to store class constraints
- **Parser updates**: Added grammar rules for invariant declarations with all comparison operators
- **Lexer updates**: Added `INVARIANT` keyword and comparison operator tokens (LE, GE, LT, GT, EQ, NE)
- **Files updated**: AGENTS.md, AST.h, AST.cpp, model-compiler.l, model-compiler.y
- **Type checking**: Invariants are intended to be type-checked as boolean expressions (to be implemented in semantic analysis phase)
- **Reasoning**: Invariants provide a declarative way to specify domain constraints on data types. This allows the compiler to generate validation code and documentation from the model. The syntax is inspired by UML constraints and Design by Contract principles. Starting with simple comparison expressions keeps the initial implementation focused while establishing the foundation for more complex boolean expressions in future iterations.

### November 1, 2025 (23:30)

- **Feature keyword introduction**: Added `feature` keyword for explicit field declarations
- **Syntax change**: Changed from `String author [optional];` to `feature author: String [optional];`
- **New field declaration format**: `feature fieldName: TypeName [modifiers];`
- **Parser updates**: Modified grammar to require `feature` keyword before field name, followed by colon, then type specification
- **Lexer updates**: Added `FEATURE` token to recognized keywords
- **Files updated**: AGENTS.md, all example files, lexer (model-compiler.l), parser (model-compiler.y)
- **Self-documenting syntax**: Field declarations now explicitly use `feature` keyword making the intent immediately clear
- **Reasoning**: The `feature` keyword makes field declarations more explicit and self-documenting. The syntax `feature author: String [optional]` clearly separates the field name from its type specification, making the language more readable and easier to parse. This syntax is inspired by UML attribute notation and modern languages like Kotlin/Swift that use explicit declaration keywords. The colon separator between name and type is a familiar pattern that reduces ambiguity and improves code clarity.

### November 1, 2025 (23:00)

- **Optional modifier**: Replaced `?` shorthand syntax with `[optional]` modifier
- **Syntax change**: Changed from `String author?;` to `String author[optional];`
- **Modifier list enhancement**: `optional` can be combined with other modifiers like `[optional,unique]`
- **Consistency improvement**: All field constraints now use the same square bracket modifier syntax
- **Files updated**: AGENTS.md, README.md, examples, lexer, parser, and AST
- **Reasoning**: Using `[optional]` instead of `?` provides a more consistent syntax where all field modifiers use the same square bracket notation. This makes the language easier to parse and understand, eliminates special cases, and allows `optional` to be naturally combined with other modifiers in a uniform way. The `?` suffix was a special case that broke the pattern of bracket-based modifiers.

### November 1, 2025 (22:45)

- **Keyword change**: Replaced `fabric` keyword with `class` for type definitions
- **Inheritance syntax change**: Changed from `AudioAsset : Asset` to `class AudioAsset inherits Asset`
- **New keyword**: Added `inherits` keyword for declaring inheritance relationships
- **Syntax modernization**: The new syntax is more explicit and reads more naturally
- **Files updated**: AGENTS.md, README.md, and lexer/parser
- **Reasoning**: The `class` keyword is more familiar to developers and clearly indicates user-defined types. The `inherits` keyword makes inheritance explicit and readable, avoiding potential confusion with C++ syntax where `:` can mean multiple things. The syntax `class AudioAsset inherits Asset` is self-documenting and easier to understand at a glance than `fabric AudioAsset : Asset`.

### November 1, 2025 (22:30)

- **Fabric base type eliminated**: Removed the Fabric base type entirely from the type system
- **Unified metadata model**: Moved all Fabric fields (creationDate, modificationDate, comment) into universal metadata
- **Simplified architecture**: All types now have six universal metadata fields that are automatically present
- **Universal metadata fields** (complete list):
  - `typeId` (Guid) - unique per type
  - `id` (Guid) - unique per instance
  - `cardinality` (Int) - cardinality of instance
  - `creationDate` (Timestamp) - when instance created
  - `modificationDate` (Timestamp) - when instance modified
  - `comment` (String) - user notes
- **Inheritance simplified**: User-defined inheritance (e.g., `AudioAsset : Asset`) is now purely for domain modeling, not for inheriting system metadata
- **Documentation updates**: Updated AGENTS.md and example code to remove all Fabric references
- **Reasoning**: Eliminating the Fabric base type simplifies the type system by having a single, flat layer of universal metadata instead of a two-tier system (universal metadata + Fabric inheritance). All types get the same six metadata fields automatically, and inheritance is used only for domain-specific type hierarchies. This makes the language easier to understand and implement, with clearer semantics about what fields every type has.

### November 1, 2025 (22:15)

- **Universal metadata fields added**: Added three metadata fields that all types automatically have (separate from Fabric inheritance)
- **New fields**:
  - `typeId` (Guid) - unique identifier for the type itself, same for all instances of a given type
  - `id` (Guid) - unique identifier for each type instance, unique per instance
  - `cardinality` (Int) - cardinality of the type instance
- **Design separation**: These universal fields exist at the type system level, NOT as part of the Fabric base type
- **Field hierarchy**: All types have: universal metadata (typeId, id, cardinality) + Fabric inheritance (creationDate, modificationDate, comment) + user-defined fields
- **Documentation updates**: Updated AGENTS.md to clearly separate universal metadata from Fabric base type fields
- **Reasoning**: Separating universal metadata from Fabric provides cleaner architecture. The typeId, id, and cardinality fields are fundamental to the type system itself and apply to ALL types including Fabric. Keeping them separate from Fabric inheritance makes the distinction clear: universal metadata is about the type system mechanics, while Fabric provides domain-level instance tracking (timestamps, comments). This also allows Fabric to be treated as a regular base type that could theoretically be extended or modified without affecting core type system metadata.

### November 1, 2025 (22:00)

- **Fabric typeId removal**: Removed the static `typeId` field from the built-in Fabric base type
- **Simplified base type**: Fabric now only provides instance-level metadata: creationDate, modificationDate, and comment
- **Documentation updates**: Updated AGENTS.md and README.md to remove all references to typeId
- **Guid usage clarification**: The `Guid` type is now exclusively used for instance identifiers (e.g., `Podcast.id`), not for type identification
- **Reasoning**: Simplifies the type system by removing type-level identification. The typeId field added complexity without clear benefit for the initial use cases. Instance-level identification via Guid fields and runtime type information (RTTI) in generated code are sufficient for podcast domain modeling. This makes the Fabric base type cleaner and more focused on instance metadata tracking.

### November 1, 2025 (21:00)

- **Project branding update**: Replaced all references to "P3 (Podcast Project Programming language)" with BBFM (Big Bad Feed Machine) branding
- **File extension change**: Changed source file extension from `.p3` to `.fm`
- **Example files renamed**: Renamed all example files from `.p3` to `.fm` extension
- **Documentation updates**: Updated README.md and AGENTS.md to reflect BBFM project context
- **Project context clarification**: Emphasized that this compiler is part of the larger BBFM project which provides common infrastructure for podcast-related applications
- **Terminology updates**:
  - "P3 language" → "BBFM modeling language"
  - "P3 syntax" → "BBFM syntax"
  - "P3 Type" → "BBFM Type"
  - Example references: `podcast.p3` → `podcast.fm`
- **AST dump header**: Changed output from "P3 Program AST" to "BBFM Program AST"
- **.gitignore update**: Marked old `p3c` binary name as deprecated
- **Reasoning**: The compiler is not a standalone "P3" project but rather part of the larger Big Bad Feed Machine (BBFM) ecosystem. Using consistent BBFM branding across all documentation, file extensions, and code makes the project's purpose and context clearer. The `.fm` extension better represents the project identity and avoids confusion with other potential "P3" meanings.

### November 1, 2025 (20:30)

- **Common.h header created**: Added new header file for common macros and utilities
- **UNREFERENCED_PARAMETER macro**: Implemented macro to handle intentionally unused function parameters
- **Macro definition**: `#define UNREFERENCED_PARAMETER(param) (void)(param)`
- **Codebase updated**: Replaced all `(void)indent; // Unused parameter` with `UNREFERENCED_PARAMETER(indent)` in AST.cpp
- **Coding guidelines updated**: Added requirement to use `UNREFERENCED_PARAMETER` macro from Common.h for unused parameters
- **Reasoning**: Using a macro instead of raw `(void)param` casts makes the code more self-documenting and consistent. The macro name clearly indicates the intent to suppress warnings for intentionally unused parameters. This is a common pattern in professional codebases (e.g., Windows SDK uses a similar approach). Centralizing the definition in Common.h also makes it easy to adjust the implementation if needed for different compilers.

### November 1, 2025 (20:15)

- **Header file alignment**: Added requirement for 8-byte alignment in all header files
- **Cross-compiler compatibility**: Implemented pragma pack directives compatible with MSVC, GCC, and Clang
- **Structure**: Use `#pragma pack(push, 8)` at top of header and `#pragma pack(pop)` at bottom
- **Location**: Alignment pragmas placed after include guard opening, before includes at top; before include guard closing at bottom
- **Reasoning**: Ensures consistent memory layout across different compilers and platforms. 8-byte alignment is standard for modern 64-bit systems and prevents potential ABI issues when code is compiled with different compilers or used as a library. The push/pop pattern preserves any alignment settings from including files.

### November 1, 2025 (20:00)

- **Include guard naming convention**: Defined standard format for header file include guards
- **Format**: `__CLASS_NAME_H_INCL__` where CLASS_NAME is the class declared in the file
- **Rules**:
  - Convert class name to UPPER_SNAKE_CASE
  - Single word: `Driver` → `__DRIVER_H_INCL__`
  - Multi-word PascalCase: Insert underscore between words, e.g., `TestTools` → `__TEST_TOOLS_H_INCL__`
  - All caps acronyms: `AST` → `__AST_H_INCL__`
- **Reasoning**: Provides consistent, predictable naming pattern for include guards based on the primary class in the file. The double underscore prefix/suffix and _INCL suffix clearly identify these as include guards while avoiding conflicts with standard library macros.

### November 1, 2025 (19:45)

- **Comparison ordering rule**: Added coding guideline for constant-left comparisons
- **Explicit nullptr checks**: Mandate explicit `nullptr` comparisons instead of implicit boolean conversion
- **New rule**: Always write `if (nullptr == ptr)` instead of `if (!ptr)` or `if (ptr == nullptr)`
- **Guideline location**: Added new "Comparison and Conditional Expressions" subsection to C++ Coding Standards
- **Reasoning**: Constant-left comparisons prevent accidental assignment bugs where `=` is used instead of `==`. The compiler will error on `nullptr = ptr` but may silently allow `ptr = nullptr` in a conditional. This defensive programming practice catches errors at compile time rather than runtime. Additionally, explicit nullptr comparisons make the intent clearer and more maintainable.

### November 1, 2025 (19:30)

- **Lexer and parser file naming**: Renamed lexer and parser files to use project name prefix
- **Files renamed**:
  - `src/lexer.l` → `src/model-compiler.l`
  - `src/parser.y` → `src/model-compiler.y`
- **CMakeLists.txt updated**: Updated LEXER_SOURCE and PARSER_SOURCE paths to reference new filenames
- **Build verification**: Confirmed project builds successfully with renamed files
- **Reasoning**: Using the project name prefix (model-compiler) for lexer and parser files creates consistency with the binary name and makes the files more identifiable in the project. This follows common convention where core compiler components are named after the project itself.

### November 1, 2025 (19:20)

- **AST root node rename**: Renamed `Program` class to `AST`
- **Files updated**: AST.h, AST.cpp, Driver.h, Driver.cpp, main.cpp, parser.y
- **Updated references**:
  - `std::unique_ptr<Program>` → `std::unique_ptr<AST>`
  - `bbfm::Program` → `bbfm::AST`
  - Parser union member `program` → `ast`
  - Variable names `prog` → `ast` in parser
- **Documentation updated**: Comments and docstrings now refer to AST instead of Program
- **Reasoning**: The name `AST` is more descriptive and accurately represents that this class is the Abstract Syntax Tree root node. It avoids confusion with the concept of a "program" as executable code versus a data structure. The term AST is standard in compiler literature and makes the codebase more immediately understandable.

### November 1, 2025 (19:15)

- **Driver API refactoring**: Modified `Driver::Phase0()` to return AST as `std::unique_ptr<AST>`
- **Return value change**: `Phase0()` now returns `std::unique_ptr<AST>` instead of `bool`
  - Returns `nullptr` on failure
  - Returns AST ownership to caller on success
- **Removed methods**:
  - `Driver::GetAST()` - no longer needed since Phase0 returns the AST
  - `Driver::DumpAST()` - redundant with `AST::Dump()` method
- **Driver state simplified**: Driver no longer stores AST internally (removed `ast_` member variable)
- **main.cpp updated**: Now receives AST from `Phase0()` and calls `ast->Dump()` directly
- **Reasoning**: Returning the AST directly from Phase0 provides clearer ownership semantics and a more functional programming style. The caller receives the AST and decides what to do with it (dump, analyze, transform, etc.). This eliminates the need for getter methods and makes the Driver's lifecycle simpler. The AST class already has a `Dump()` method, so having a wrapper in Driver was redundant.

### November 1, 2025 (19:00)

- **Namespace rename**: Renamed C++ namespace from `p3` to `bbfm`
- **Files updated**: All source files (AST.h, AST.cpp, Driver.h, Driver.cpp, main.cpp, parser.y)
- **Redundant scopes removed**: Within the `bbfm` namespace, type references no longer need the `bbfm::` prefix (e.g., `std::unique_ptr<AST>` instead of `std::unique_ptr<bbfm::AST>` in Driver.h)
- **Reasoning**: The `bbfm` namespace better reflects the project's broader purpose and provides a unique identifier. Removing redundant namespace prefixes within the namespace itself improves code readability and follows standard C++ practices.

### November 1, 2025 (18:30)

- **Driver class implementation**: Created new `Driver` class to orchestrate compilation phases
- **Separation of concerns**: `main.cpp` now only handles command-line argument parsing
- **Phase 0 implementation**: Actual parsing moved to `Driver::Phase0()` method
- **Files added**:
  - `include/Driver.h` - Driver class interface
  - `src/Driver.cpp` - Driver class implementation
- **Architecture improvement**: Clear separation between CLI handling and compilation logic
- **Multi-file support**: Driver prepared for future multi-file compilation (currently single file)
- **Error handling**: Driver tracks compilation errors through `HasErrors()` method
- **Reasoning**: Separating the driver from main() creates a cleaner architecture where main.cpp is just a thin CLI wrapper. This makes the compiler easier to test, embed in other applications, and extend with additional compilation phases. The Driver class will orchestrate all future phases (semantic analysis, code generation) in a consistent way.

### November 1, 2025 (18:00)

- **File naming convention**: Renamed AST files to use uppercase naming
- **Files renamed**:
  - `include/ast.h` → `include/AST.h`
  - `src/ast.cpp` → `src/AST.cpp`
- **Updated references**: Updated all include statements in `main.cpp`, `AST.cpp`, and `parser.y`
- **CMake update**: Updated `CMakeLists.txt` to reference `src/AST.cpp`
- **Project rename**: Renamed CMake project from `p3_compiler` to `model_compiler` and binary from `p3c` to `model-compiler`
- **Reasoning**: Uppercase file names (AST.h, AST.cpp) better reflect that these files define types and classes, following common C++ convention where header files for major components use uppercase. The project rename reflects the broader BBFM (Big Bad Feed Machine) scope, positioning the compiler as infrastructure for podcast-related applications.

### November 1, 2025 (17:00)

- **C++ Coding Guidelines Implementation**: Applied all coding guidelines to existing source code
- **Method Naming**: Renamed all methods to Upper PascalCase:
  - Getters: `getType()` → `GetType()`, `getName()` → `GetName()`, etc.
  - Boolean checks: `isPrimitive()` → `IsPrimitive()`, `isStatic()` → `IsStatic()`, etc.
  - Helpers: `dump()` → `Dump()`, `printIndent()` → `PrintIndent()`, etc.
  - Static methods: `typeToString()` → `TypeToString()`
- **Doxygen Documentation**: Added comprehensive Doxygen-style comments to all public APIs in AST.h:
  - Used `///` for Doxygen comments
  - Added `\brief` descriptions for all classes and methods
  - Added `\param` for all parameters
  - Added `\return` for return values
- **Parameter Handling**: Marked unused `indent` parameters with `(void)indent;` to suppress warnings
- **Const Correctness**: Made all function parameters const where appropriate
- **Testing**: Verified all changes compile cleanly and produce identical output
- **Reasoning**: Implementing coding standards early in the project ensures consistency as the codebase grows. Using Upper PascalCase for methods aligns with the convention and makes the API more professional. Doxygen comments improve code documentation and enable automatic API documentation generation.

### November 1, 2025 (16:30)

- **C++ Coding Standards Consolidation**: Integrated manually added C++ coding guidelines into the Coding Standards & Conventions section
- **Documentation Organization**: Organized coding standards into clear subsections:
  - General Principles (RAII, smart pointers, const-correctness)
  - Functions and Methods (const parameters, const methods)
  - Classes and Destructors (virtual destructors, Rule of Five/Zero)
  - Naming Conventions (PascalCase for types/functions, camelCase for variables)
  - Comments and Documentation (C++ line comments, Doxygen style)
  - Documentation Tools (Graphviz DOT, UML guidelines)
  - Documentation Accuracy (verification against implementation)
- **Reasoning**: Consolidating coding standards into a comprehensive, well-organized section makes them easier to follow and reference. Clear subsections help AI agents and human developers quickly find relevant guidelines. The standards now cover all aspects of C++ development in the project from naming to documentation.

### November 1, 2025 (16:00)

- **AST Implementation**: Implemented complete Abstract Syntax Tree for the BBFM modeling language
- **AST Structure**: Created comprehensive node hierarchy with `Program`, `Declaration`, `EnumDeclaration`, `FabricDeclaration`, `Field`, `TypeSpec`, `Modifier` classes
- **AST Features**:
  - Modern C++23 with smart pointers (`std::unique_ptr`) for memory management
  - Support for all BBFM language constructs (enums, fabric types, inheritance, fields, modifiers)
  - `PrimitiveTypeSpec` and `UserDefinedTypeSpec` for type representation
  - `CardinalityModifier` and `UniqueModifier` for field constraints
  - Clean `dump()` method for AST visualization and debugging
- **Parser Integration**: Updated Bison parser to construct AST during parsing using void pointer union for C/C++ interop
- **File Organization**:
  - `include/AST.h` - AST node class declarations
  - `src/AST.cpp` - AST implementation with dump methods
  - Updated `CMakeLists.txt` to include AST source
- **Testing**: Successfully tested with all example files; AST correctly represents parsed programs
- **Reasoning**: AST is essential foundation for semantic analysis and code generation phases. Using modern C++ with smart pointers ensures memory safety and clean resource management. The separation of interface (AST.h) and implementation (AST.cpp) follows best practices for maintainable C++ projects. Void pointer union in Bison parser solves C/C++ linkage issues while maintaining type safety through static_cast in grammar actions.

### November 1, 2025 (14:30)

- **Optional field shorthand**: Implemented `?` syntax for optional fields
- **Question mark operator**: `String name?;` is now equivalent to `String name[0..1];`
- **Lexer enhancement**: Added `QUESTION` token to recognize `?` character
- **Parser enhancement**: Added grammar rule for optional field shorthand syntax
- **Example updates**: Updated examples to demonstrate the new `?` syntax
- **Reasoning**: Further reduces verbosity for optional fields, which are very common in data modeling. The `?` syntax is intuitive (borrowed from languages like TypeScript, Swift, Kotlin) and makes optional fields immediately recognizable. Complements the `[1]` default to create a very clean, readable syntax for the most common field types.

### November 1, 2025

- **Shorthand modifier syntax**: Implemented optional modifier syntax where `[1]` is the default
- **Parser enhancement**: Modified parser to accept fields without explicit modifiers (e.g., `String name;` instead of `String name[1];`)
- **Backward compatibility**: Explicit modifiers still work; both syntaxes are valid
- **Example updates**: Updated example files to demonstrate cleaner shorthand syntax
- **Reasoning**: Reduces verbosity for the most common case (mandatory single values); makes the modeling language cleaner and more readable while maintaining full expressiveness when needed. Since `[1]` is the most common modifier, making it the default eliminates repetitive syntax without losing clarity.

### October 14, 2025

- **C++ conversion**: Converted entire project from C to C++23
- **Language change**: Project now uses C++23 instead of C23
- **Source files**: Renamed `main.c` to `main.cpp`; Flex/Bison generate `.cpp` files
- **C++ features adopted**:
  - `iostream` for I/O (`std::cout`, `std::cerr` instead of `printf`, `fprintf`)
  - C++ headers (`<cstdio>`, `<cstdlib>`, `<cstring>` instead of C headers)
  - Proper extern "C" linkage for Flex/Bison generated code
- **Build system updates**:
  - CMakeLists.txt updated to use `CXX` project type
  - CMAKE_CXX_STANDARD set to 23
  - Flex/Bison configured to generate C++ compatible code
- **Linkage strategy**:
  - YY_DECL macro defines yylex with extern "C" linkage
  - Parser functions wrapped with extern "C" for proper C/C++ interoperability
  - %code requires in parser.y ensures correct linkage declarations
- **Reasoning**: C++23 provides better type safety, modern features (modules, ranges, concepts), and improved standard library while maintaining compatibility with Flex/Bison toolchain. This positions the project for future enhancements using modern C++ features for semantic analysis and code generation phases.

### October 4, 2025 (20:00)

- **UUID type removal**: Removed `UUID_TYPE` token from lexer and parser
- **Guid type usage**: The `Guid` type serves all identifier needs for instance IDs (like `Podcast.id`)
- **Reasoning**: Users never write `UUID` in their BBFM code. The `Guid` type is a universal identifier type for domain object instances.

### October 4, 2025 (19:45)

- **Build script**: Created `build.sh` for automated builds using CMake with Ninja generator
- **Reasoning**: Simplifies build process; provides consistent build commands; easier for testing iterations

### October 4, 2025 (19:40)

- **Guid type rename**: Renamed `Id` type to `Guid` to avoid confusion with lowercase field names like `id`
- **Reasoning**: Case-insensitive language needs clear distinction; `Guid` (GUID = Globally Unique Identifier) is semantically correct and avoids conflicts

### October 4, 2025 (19:35)

- **Case-insensitive language**: Language is case-insensitive for keywords and identifiers
- **Reasoning**: Simplifies usage; users can write `FABRIC`, `fabric`, or `Fabric` - all equivalent; reduces errors from capitalization mistakes

### October 4, 2025 (19:30)

- **Lexer and parser implementation**: Implemented BBFM lexer and parser supporting full syntax
- **Features working**: Enums, fabric types, inheritance (`:` syntax), field modifiers (`[1]`, `[0..1]`, `[0..*]`, `[unique]`), all primitive types
- **Reasoning**: Foundation complete for semantic analysis and code generation phases

### October 4, 2025 (19:20)

- **Keyword decision**: Use `fabric` keyword for type definitions instead of `type`
- **Reasoning**: Reinforces the concept that all types are "fabric" objects inheriting from the implicit Fabric base; avoids confusion with having `type Fabric` in source code; makes the language feel cohesive

### October 4, 2025 (19:15)

- **User-defined inheritance**: Types can inherit from other user types using `: BaseType` syntax
- **Inheritance chains**: Support for multi-level inheritance (e.g., AudioAsset : Asset : Fabric)
- **Reasoning**: Enables code reuse and proper modeling of domain hierarchies; inherited fields not duplicated

### October 4, 2025 (19:10)

- **Fabric as implicit base type**: Fabric is not declared in source code but built into the language; all user types automatically inherit from it
- **No field duplication**: Inherited Fabric fields (creationDate, modificationDate, comment) are not redeclared in user types
- **Reasoning**: Cleaner syntax; users don't have to think about base type boilerplate; ensures all types have consistent metadata

### October 4, 2025 (19:05)

- **Design philosophy**: UML-inspired syntax for familiarity, but deliberately simplified
- **Excluded features**: No composition/aggregation distinction, visibility modifiers, abstract types, interfaces, or stereotypes
- **Reasoning**: Keep the language focused on data modeling without unnecessary complexity; easier to learn and use

### October 4, 2025 (19:00)

- **Field modifier syntax**: Introduced unified modifier syntax using square brackets `[]`
- **Cardinality modifiers**: `[1]` mandatory, `[0..1]` optional, `[0..*]` optional array, `[1..*]` required array
- **Constraint modifiers**: `[unique]` for unique fields; modifiers can be combined (e.g., `[1,unique]`)
- **Reasoning**: Provides consistent, explicit syntax for all field constraints; eliminates ambiguity and makes cardinality/constraints immediately visible

### October 4, 2025 (18:45)

- **Type naming conventions**: All types use PascalCase; primitive types renamed to `String`, `Int`, `Real`
- **Timespan type addition**: Added `Timespan` type for durations (maps to double/seconds)
- **Reasoning**: Consistent naming improves readability and makes the language feel more cohesive; PascalCase distinguishes types from values

### October 4, 2025 (18:35)

- **Primitive type naming**: Added `Timestamp` type for temporal data (stored as double/seconds since epoch)
- **Reasoning**: More semantic and clear than using generic `double` for timestamp fields

### October 4, 2025 (18:30)

- **Base type decision**: Introduced `Fabric` base type with common metadata fields
- **Metadata fields**: All types inherit creationDate (Timestamp), modificationDate (Timestamp), and comment (String)
- **Reasoning**: Provides consistent metadata tracking across all domain objects with instance-level timestamps and user comments

### October 4, 2025 (18:15)

- **Project goals defined**: DSL for podcast object model definitions, targeting Swift code generation
- **Language features specified**: User-defined types, enums, dynamic arrays, all relationship types, required/optional fields, unique constraints
- **Syntax decision**: C++-like syntax (to be refined iteratively)
- **Reasoning**: Clear project scope enables focused language design; Swift chosen as initial target with extensibility for future languages

### October 4, 2025 (18:00)

- **Update protocol clarification**: Made update protocol more explicit and proactive
- **Reasoning**: AI agents need clear triggers for when to update AGENTS.md automatically without being asked. Added explicit list of when to update and emphasized the word "PROACTIVELY"

### October 4, 2025 (17:30)

- **Build system selection**: Use Ninja instead of Make
- **Reasoning**: Ninja provides faster parallel builds and cleaner output, better for iterative compiler development

### October 4, 2025 (17:00)

- **Project structure**: Created initial compiler project with Flex/Bison/CMake
- **Reasoning**: Established standard compiler architecture with clear separation of lexer, parser, and main driver

### October 4, 2025 (16:00)

- **Initial setup**: Created AGENTS.md with foundational operating instructions
- **Reasoning**: Establish clear guidelines for AI agent behavior, update protocols, and commit procedures to ensure consistent and controlled collaboration

