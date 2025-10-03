# AI Agent Operating Instructions

**Last updated:** October 4, 2025 (19:45)

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

---

## Project Overview

This is a Domain-Specific Language (DSL) compiler for defining podcast object models and their relationships. The compiler is written in C using Flex (lexical analyzer) and Bison (parser generator), built with CMake and Ninja.

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
- **C Standard**: C11

### Project Structure

```text
p3-compiler/
├── CMakeLists.txt          # CMake configuration
├── src/                    # Source files
│   ├── lexer.l            # Flex lexer specification
│   ├── parser.y           # Bison parser specification
│   └── main.c             # Main entry point
├── include/               # Header files
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
  - Cardinality: `[1]` (mandatory), `[0..1]` (optional), `[0..*]` (optional array), `[1..*]` (required array with at least one element)
  - Constraints: `[unique]` for unique fields
  - Modifiers can be combined: `[1,unique]` for mandatory unique field

### Language Features

**Type System:**

- User-defined types (for podcast domain objects)
- Base type: `Fabric` - all types automatically inherit from this (built-in, not declared in source)
  - `typeId` (static UUID) - unique identifier for the type itself
  - `creationDate` (Timestamp) - when instance was created
  - `modificationDate` (Timestamp) - when instance was last modified
  - `comment` (String) - user comment/notes
- Inheritance: All user types inherit from Fabric; inherited fields are not duplicated in generated code
- User-defined inheritance: Types can inherit from other user types using `: BaseType` syntax
- Primitive types (String, Int, Real, Timestamp, Timespan, Date, UUID, Guid, etc.)
  - `Guid` type for instance identifiers (maps to UUID in implementation)
  - `Timestamp` type for points in time (maps to double/seconds since epoch)
  - `Timespan` type for durations (maps to double/seconds)
  - `Int` for integers
  - `Real` for floating-point numbers
  - `String` for text
  - `UUID` for type identifiers (static, per-type)
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
3. Semantic Analysis (type checking, validation) - To be implemented
4. Code Generation:
   - C++ class generation - To be implemented
   - SQLite schema generation - To be implemented

**Current Status:** The lexer and parser successfully parse P3 syntax including enums, fabric types, inheritance, field modifiers, and all primitive types.

### Example P3 Syntax

```cpp
// Fabric base type is implicit - not declared in source code
// All types automatically inherit: typeId, creationDate, modificationDate, comment

enum MediaType {
    AUDIO,
    VIDEO
}

fabric Asset {
    // Inherits from implicit Fabric base
    Guid id[1];
    String url[1];
}

fabric AudioAsset : Asset {
    // Inherits from Asset (which inherits from Fabric)
    String format[1];
    Int fileSize[1];
}

fabric VideoAsset : Asset {
    // Inherits from Asset (which inherits from Fabric)
    Int width[1];
    Int height[1];
    Timespan duration[1];
}

fabric Podcast {
    // Inherits Fabric fields automatically (not shown in source)
    Guid id[1];
    String title[1];
    String description[1];
    String author[0..1];         // optional field
    String rssUrl[1,unique];     // mandatory + unique
    Episode episodes[0..*];      // one-to-many (may be empty)
}

fabric Episode {
    Guid id[1];
    String title[1];
    Date publishedAt[1];
    Timespan duration[1];
    MediaType mediaType[1];
    AudioAsset audio[1];         // one-to-one relationship
    Transcript transcript[0..1]; // optional one-to-one
}

fabric Transcript {
    Guid id[1];
    String text[1];
    String language[1];
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
| `UUID` | `std::string` | `TEXT` |
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
_build/p3c examples/podcast.p3
```

---

## Recent Updates & Decisions

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

### October 4, 2025 (18:40)

- **Id type addition**: Added `Id` primitive type for instance identifiers (separate from static `UUID` type for type identification)
- **Reasoning**: Semantic distinction between instance IDs and type IDs; `Id` is used for object instances, `UUID` for type metadata

### October 4, 2025 (18:35)

- **Primitive type naming**: Added `Timestamp` type for temporal data (stored as double/seconds since epoch)
- **Reasoning**: More semantic and clear than using generic `double` for timestamp fields

### October 4, 2025 (18:30)

- **Base type decision**: Introduced `Fabric` base type with common metadata fields
- **Metadata fields**: All types inherit typeId (UUID), creationDate (double timestamp), modificationDate (double timestamp), and comment (string)
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

