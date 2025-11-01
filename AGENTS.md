# AI Agent Operating Instructions

**Last updated:** November 1, 2025 (18:00)

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

This is a Domain-Specific Language (DSL) compiler for defining podcast object models and their relationships. The compiler is written in C++23 using Flex (lexical analyzer) and Bison (parser generator), built with CMake and Ninja.

### Project Goals

The compiler enables users to define data types and relationships for podcast domains (podcasts, episodes, audio assets, transcripts, etc.) using a simple, expressive language. The compiler then generates:

1. Source code in target programming languages
2. SQL database schema creation scripts

### Target Outputs (Initial)

- **Source Code**: C++ (with plans to support additional languages in the future)
- **Database Schema**: SQLite (with plans to support additional SQL dialects in the future)

## Coding Standards & Conventions

### Build System

- **Build Tool**: CMake (version 3.20+)
- **Generator**: Ninja (preferred over Make for faster parallel builds)
- **Build Directory**: `_build/` (not `build/`)
- **C++ Standard**: C++23

### C++ Coding Standards

**General Principles:**

- Follow modern C++ best practices (C++23 standard)
- Use RAII principles for resource management
- Prefer smart pointers (`std::unique_ptr`, `std::shared_ptr`) over raw pointers
- Use const-correctness throughout the codebase
- This style provides extra safety and consistency throughout the codebase

**Functions and Methods:**

- All function input parameters should be const (e.g., `void SetTitle(const std::string& title)`)
- All functions that return class data without modification should be const (e.g., `std::string GetTitle() const`)
- Pass by const reference for complex types, by value for primitives
- Use trailing return types when it improves clarity

**Classes and Destructors:**

- All destructors should be virtual (even when deleted)
- All abstract/interface classes should have a protected virtual destructor
- Use the Rule of Zero when possible (let compiler generate special members)
- When implementing special members, follow the Rule of Five

**Naming Conventions:**

- Types (classes, structs, enums, typedefs): Upper PascalCase (e.g., `Episode`, `SharedObject`, `MediaType`)
- Functions/methods: Upper PascalCase (e.g., `GetTitle`, `SetDuration`, `ParseInput`)
- Variables and function parameters: camelCase (e.g., `bufferSize`, `episodeCount`, `userName`)
- Member variables: camelCase with underscore postfix (e.g., `dataSize_`, `title_`, `description_`)
- Constants: UPPER_SNAKE_CASE (e.g., `MAX_EPISODE_LENGTH`, `DEFAULT_TIMEOUT`)
- Remove redundant prefixes from class names (e.g., use `Model` instead of `P3Model`)

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
p3-compiler/
├── CMakeLists.txt          # CMake configuration
├── src/                    # Source files
│   ├── lexer.l            # Flex lexer specification
│   ├── parser.y           # Bison parser specification
│   ├── AST.cpp            # AST implementation
│   └── main.cpp           # Main entry point (C++)
├── include/               # Header files
│   └── AST.h              # AST node definitions
├── examples/              # Test programs
└── _build/                # Build artifacts (gitignored)
```

### Language & Compiler Design

- **DSL Name**: P3 (Podcast Project Programming language)
- **Syntax Style**: C++-like (will be refined iteratively)
- **Design Philosophy**: UML-inspired but simplified - focuses on data modeling without UML's full complexity
- **Lexer**: Flex 2.6+
- **Parser**: Bison 3.8+
- **Keywords**: `fabric` (define types), `enum` (define enumerations)
- **Naming Conventions**:
  - All type names use PascalCase
  - User-defined types: PascalCase (e.g., `Podcast`, `Episode`)
  - Primitive types: PascalCase (e.g., `String`, `Int`, `Real`)
  - Language is case-insensitive for keywords and type names
- **Field Modifiers**:
  - Modifiers are specified in square brackets `[]` after the field name
  - **Default modifier**: `[1]` (mandatory single value) - applied when no modifiers are specified
  - Shorthand syntax: `String name;` is equivalent to `String name[1];`
  - **Optional shorthand**: `String name?;` is equivalent to `String name[0..1];`
  - Cardinality: `[1]` (mandatory), `[0..1]` (optional), `[0..*]` (optional array), `[1..*]` (required array with at least one element)
  - Constraints: `[unique]` for unique fields
  - Modifiers can be combined: `[1,unique]` for mandatory unique field

### Language Features

**Type System:**

- User-defined types (for podcast domain objects)
- Base type: `Fabric` - all types automatically inherit from this (built-in, not declared in source)
  - `typeId` (static Guid) - unique identifier for the type itself
  - `creationDate` (Timestamp) - when instance was created
  - `modificationDate` (Timestamp) - when instance was last modified
  - `comment` (String) - user comment/notes
- Inheritance: All user types inherit from Fabric; inherited fields are not duplicated in generated code
- User-defined inheritance: Types can inherit from other user types using `: BaseType` syntax
- Primitive types (String, Int, Real, Timestamp, Timespan, Date, Guid)
  - `Guid` type for globally unique identifiers (maps to UUID/string in implementation)
  - Used for both instance identifiers (e.g., `Podcast.id`) and type identifiers (e.g., Fabric's static `typeId`)
  - `Timestamp` type for points in time (maps to double/seconds since epoch)
  - `Timespan` type for durations (maps to double/seconds)
  - `Int` for integers
  - `Real` for floating-point numbers
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
- Foreign key constraints (for database generation)

**Compilation Phases:**

1. Lexical Analysis (tokenization) ✅ Implemented
2. Syntax Analysis (parsing) ✅ Implemented
3. **AST Construction** ✅ Implemented
4. Semantic Analysis (type checking, validation) - To be implemented
5. Code Generation:
   - C++ class generation - To be implemented
   - SQLite schema generation - To be implemented

**Current Status:** The lexer, parser, and AST construction are complete. The compiler successfully parses P3 syntax and builds a complete Abstract Syntax Tree representing enums, fabric types, inheritance, field modifiers, and all primitive types. The AST uses modern C++ with smart pointers and provides a clean dump() method for visualization.

### Example P3 Syntax

```cpp
// Fabric base type is implicit - not declared in source code
// All types automatically inherit:
//   - static Guid typeId (unique per type)
//   - Timestamp creationDate (when instance created)
//   - Timestamp modificationDate (when instance modified)
//   - String comment (user notes)

enum MediaType {
    AUDIO,
    VIDEO
}

fabric Asset {
    // Inherits from implicit Fabric base
    Guid id;        // Shorthand for [1]
    String url;     // Shorthand for [1]
}

fabric AudioAsset : Asset {
    // Inherits from Asset (which inherits from Fabric)
    String format;
    Int fileSize;
}

fabric VideoAsset : Asset {
    // Inherits from Asset (which inherits from Fabric)
    Int width;
    Int height;
    Timespan duration;
}

fabric Podcast {
    // Inherits Fabric fields automatically (not shown in source)
    Guid id;
    String title;
    String description;
    String author?;              // optional field using ? shorthand
    String rssUrl[1,unique];     // mandatory + unique (explicit modifiers)
    Episode episodes[0..*];      // one-to-many (may be empty)
}

fabric Episode {
    Guid id;
    String title;
    Date publishedAt;
    Timespan duration;
    MediaType mediaType;
    AudioAsset audio;            // one-to-one relationship
    Transcript transcript?;      // optional one-to-one using ? shorthand
}

fabric Transcript {
    Guid id;
    String text;
    String language;
}
```

### Type Mapping

| P3 Type | C++ | SQLite |
|---------|-----|--------|
| `String` | `std::string` | `TEXT` |
| `Int` | `int64_t` | `INTEGER` |
| `Real` | `double` | `REAL` |
| `Timestamp` | `double` | `REAL` |
| `Timespan` | `double` | `REAL` |
| `Guid` | `std::string` | `TEXT` |

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
_build/model-compiler examples/podcast.p3
```

---

## Recent Updates & Decisions

### November 1, 2025 (18:00)

- **File naming convention**: Renamed AST files to use uppercase naming
- **Files renamed**:
  - `include/ast.h` → `include/AST.h`
  - `src/ast.cpp` → `src/AST.cpp`
- **Updated references**: Updated all include statements in `main.cpp`, `AST.cpp`, and `parser.y`
- **CMake update**: Updated `CMakeLists.txt` to reference `src/AST.cpp`
- **Project rename**: Renamed CMake project from `p3_compiler` to `model_compiler` and binary from `p3c` to `model-compiler`
- **Reasoning**: Uppercase file names (AST.h, AST.cpp) better reflect that these files define types and classes, following common C++ convention where header files for major components use uppercase. The project rename reflects a broader scope beyond just P3 language, positioning the compiler as a general model compiler framework.

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

- **AST Implementation**: Implemented complete Abstract Syntax Tree for P3 language
- **AST Structure**: Created comprehensive node hierarchy with `Program`, `Declaration`, `EnumDeclaration`, `FabricDeclaration`, `Field`, `TypeSpec`, `Modifier` classes
- **AST Features**:
  - Modern C++23 with smart pointers (`std::unique_ptr`) for memory management
  - Support for all P3 language constructs (enums, fabric types, inheritance, fields, modifiers)
  - `PrimitiveTypeSpec` and `UserDefinedTypeSpec` for type representation
  - `CardinalityModifier` and `UniqueModifier` for field constraints
  - Clean `dump()` method for AST visualization and debugging
- **Parser Integration**: Updated Bison parser to construct AST during parsing using void pointer union for C/C++ interop
- **File Organization**:
  - `include/AST.h` - AST node class declarations
  - `src/AST.cpp` - AST implementation with dump methods
  - Updated `CMakeLists.txt` to include AST source
- **Testing**: Successfully tested with all example P3 files; AST correctly represents parsed programs
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
- **Example updates**: Updated `podcast.p3` example to demonstrate cleaner shorthand syntax
- **Reasoning**: Reduces verbosity for the most common case (mandatory single values); makes P3 code cleaner and more readable while maintaining full expressiveness when needed. Since `[1]` is the most common modifier, making it the default eliminates repetitive syntax without losing clarity.

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
- **Fabric typeId clarification**: The Fabric base type's static `typeId` field uses the `Guid` type (not a separate `UUID` type)
- **Reasoning**: Users never write `UUID` in their P3 code. The `Guid` type serves all identifier needs - both for instance IDs (like `Podcast.id`) and type IDs (like Fabric's static `typeId`). This simplifies the language with a single universal identifier type.

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

- **Lexer and parser implementation**: Implemented P3 lexer and parser supporting full syntax
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
- **No field duplication**: Inherited Fabric fields (typeId, creationDate, modificationDate, comment) are not redeclared in user types
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
- **Metadata fields**: All types inherit typeId (Guid), creationDate (Timestamp), modificationDate (Timestamp), and comment (String)
- **Reasoning**: Provides consistent metadata tracking across all domain objects; typeId is static (per-type), while creation/modification dates are per-instance

### October 4, 2025 (18:15)

- **Project goals defined**: DSL for podcast object model definitions, targeting C++ code generation and SQLite schema
- **Language features specified**: User-defined types, enums, dynamic arrays, all relationship types, required/optional fields, unique constraints, foreign keys
- **Syntax decision**: C++-like syntax (to be refined iteratively)
- **Reasoning**: Clear project scope enables focused language design; C++ and SQLite chosen as initial targets with extensibility for future languages/dialects

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

