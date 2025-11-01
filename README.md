# Model Compiler

A Domain-Specific Language (DSL) compiler for defining podcast object models and their relationships. Part of the Big Bad Feed Machine (BBFM) project, this compiler provides common infrastructure for podcast-related applications. The modeling language provides a simple, UML-inspired syntax for modeling podcast domain objects, which the compiler translates into source code and database schemas.

## Overview

The BBFM modeling language enables you to define data types, relationships, and constraints for podcast domains using an expressive, type-safe syntax. The compiler generates:

- **C++ classes** with proper inheritance hierarchies
- **SQLite database schemas** with foreign key relationships

Future targets include additional programming languages and SQL dialects.

## Language Features

### Type System

- **Universal Metadata**: All types automatically have these metadata fields (never declared by users):
  - `typeId` (Guid) - Unique identifier for the type itself (same for all instances of a type)
  - `id` (Guid) - Unique identifier for the instance (unique per instance)
  - `cardinality` (Int) - Cardinality of the type instance
  - `creationDate` (Timestamp) - When instance was created
  - `modificationDate` (Timestamp) - When instance was last modified
  - `comment` (String) - User comment/notes
- **User-Defined Types**: Define custom types using the `class` keyword
- **Inheritance**: Support for single inheritance with the `inherits` keyword
- **Primitive Types**: String, Int, Real, Bool, Timestamp, Timespan, Date, Guid
- **Enumerations**: Define enums for categorical values
- **Case-Sensitive**: Keywords are lowercase and case-sensitive; type names are PascalCase and case-sensitive

### Universal Metadata Fields

Every type in the BBFM modeling language automatically has six universal metadata fields. These are part of the type system itself and are never declared in your source code:

- **`typeId`** (Guid) - A unique identifier for the type itself. All instances of the same type share the same `typeId`. For example, all `Podcast` instances have the same `typeId`, which is different from the `typeId` of `Episode` instances.
- **`id`** (Guid) - A unique identifier for each instance. Every object has its own unique `id` that distinguishes it from all other instances, even those of the same type.
- **`cardinality`** (Int) - The cardinality of the type instance. This tracks relationship cardinality information for the instance.
- **`creationDate`** (Timestamp) - The date and time when an instance was created. Automatically set when an object is instantiated.
- **`modificationDate`** (Timestamp) - The date and time when an instance was last modified. Updated whenever the object changes.
- **`comment`** (String) - A user-provided comment or note field for storing arbitrary text associated with the instance.

These fields are automatically available on all types. You never declare them in your class definitions. When you create a type like:

```bbfm
class Podcast {
    String title;
    String description;
}
```

The generated code will include the universal metadata fields, so the actual structure is:

```bbfm
class Podcast {
    feature typeId: Guid;                 // universal metadata
    feature id: Guid;                     // universal metadata
    feature cardinality: Int;             // universal metadata
    feature creationDate: Timestamp;      // universal metadata
    feature modificationDate: Timestamp;  // universal metadata
    feature comment: String;              // universal metadata
    feature title: String;                // defined in Podcast
    feature description: String;          // defined in Podcast
}
```

User-defined inheritance works as expected. For example:

```bbfm
class Asset {
    feature url: String;
}

class AudioAsset inherits Asset {
    feature format: String;
}
```

Results in:
- All types have universal metadata: `typeId`, `id`, `cardinality`, `creationDate`, `modificationDate`, `comment`
- `AudioAsset` inherits from `Asset` (gets the `url` field)

So an `AudioAsset` instance has: universal metadata (6 fields) + Asset fields (url) + AudioAsset fields (format).

### Relationships

- One-to-one relationships
- One-to-many relationships (using arrays)
- Many-to-many relationships

### Field Modifiers

Fields are declared using the `feature` keyword followed by a colon and the type specification. Field cardinality and constraints are specified using square bracket notation:

- `[1]` - Mandatory single value (default if no modifier specified)
- `[0..1]` - Optional single value
- `[optional]` - Optional single value (equivalent to `[0..1]`)
- `[1..*]` - Array with at least one element
- `[0..*]` - Array that may be empty
- `[unique]` - Unique constraint
- Modifiers can be combined: `[optional,unique]`, `[1,unique]`

**Field Declaration Syntax:**

```bbfm
feature fieldName: TypeName [modifiers];
```

**Examples:**

```bbfm
feature title: String;                    // mandatory (default [1])
feature author: String [optional];        // optional field
feature rssUrl: String [1,unique];        // mandatory + unique
feature episodes: Episode [0..*];         // array (may be empty)
```

**Shorthand Syntax:**

- **Default cardinality**: Fields without modifiers default to `[1]` (mandatory)
  - `feature title: String;` is equivalent to `feature title: String [1];`

This approach makes the most common field types cleaner and more readable while maintaining full expressiveness when needed.

### Invariants

Invariants are boolean constraints on class attributes that express domain rules and validation requirements:

- Declared using the `invariant` keyword
- Syntax: `invariant name: expression;`
- Support comparison operators: `<=`, `>=`, `<`, `>`, `==`, `!=`
- Currently support simple expressions (attribute operator literal)

**Examples:**

```bbfm
class Image {
    feature width: Int;
    feature height: Int;

    invariant validWidth: width <= 3840;     // Max 4K width
    invariant validHeight: height <= 2160;   // Max 4K height
    invariant minWidth: width >= 100;        // Minimum width
}

class AudioAsset inherits Asset {
    feature fileSize: Int;

    invariant maxFileSize: fileSize <= 500000000;  // Max 500MB
}
```

Invariants will be used during semantic analysis for type checking and can generate validation code in target languages.

### Design Philosophy

The BBFM modeling language is inspired by UML class diagrams but deliberately simplified. It focuses on data modeling without the complexity of visibility modifiers, abstract types, interfaces, or stereotypes. The goal is an expressive yet approachable language for domain modeling.

## Example

```bbfm
// Define an enumeration
enum MediaType {
    AUDIO,
    VIDEO
}

// Define a base type
class Asset {
    feature url: String;
}

// Define a derived type with inheritance
class AudioAsset inherits Asset {
    feature format: String;
    feature fileSize: Int;

    invariant maxFileSize: fileSize <= 500000000;  // Max 500MB
}

// Define a type with relationships
class Podcast {
    feature title: String;
    feature description: String;
    feature author: String [optional];     // optional field
    feature rssUrl: String [1,unique];     // mandatory + unique
    feature episodes: Episode [0..*];      // one-to-many relationship
}

class Episode {
    feature title: String;
    feature publishedAt: Date;
    feature duration: Timespan;
    feature mediaType: MediaType;
    feature audio: AudioAsset;                  // one-to-one relationship
    feature transcript: Transcript [optional];  // optional field
}

class Transcript {
    feature text: String;
    feature language: String;
}
```

## Prerequisites

- CMake 3.20 or higher
- C++23-compatible compiler (GCC or Clang)
- Flex 2.6+ (lexical analyzer generator)
- Bison 3.8+ (parser generator)
- Ninja (build system)

### Installing Prerequisites

**macOS:**

```bash
brew install cmake flex bison ninja
```

**Ubuntu/Debian:**

```bash
sudo apt-get install cmake flex bison gcc ninja-build
```

## Building

**Using the build script (recommended):**

```bash
./build.sh
```

**Manual build with CMake:**

```bash
# Create build directory
mkdir _build
cd _build

# Configure with CMake and Ninja
cmake -G Ninja ..

# Build
ninja

# Clean
ninja clean
```

## Usage

```bash
./_build/model-compiler <source_file.fm>
```

Example:

```bash
./_build/model-compiler examples/podcast.fm
```

## Project Structure

```text
model-compiler/
├── CMakeLists.txt          # CMake build configuration
├── build.sh                # Build script
├── README.md               # This file
├── AGENTS.md               # AI agent operating instructions
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
├── examples/              # Example programs
│   └── podcast.fm       # Podcast domain model example
└── _build/                # Build artifacts (gitignored)
```

## Compilation Phases

The compiler implements a multi-phase compilation process:

1. **Phase 0: Lexical Analysis & Parsing** ✅ - Tokenizes input and parses into AST
2. **Phase 1: Semantic Analysis** ✅ - Type checking and validation
   - Symbol table construction
   - Type validation (primitives, enums, user-defined types)
   - Inheritance validation (cycle detection, base type verification)
   - Field uniqueness validation (including inherited fields)
   - Invariant validation (field reference checking)
3. **Phase 2: Code Generation** 🚧 - Planned:
   - C++ class definitions with inheritance
   - SQLite database schema with foreign keys

## Type Mappings

| BBFM Type | C++ | SQLite |
|---------|-----|--------|
| String | std::string | TEXT |
| Int | int64_t | INTEGER |
| Real | double | REAL |
| Bool | bool | INTEGER |
| Timestamp | double | REAL |
| Timespan | double | REAL |
| Guid | std::string | TEXT |

## Current Status

**✅ Implemented:**

- **Phase 0 (Lexical Analysis & Parsing)**:
  - Lexical analysis with case-sensitive keywords and types
  - Syntax analysis supporting full grammar
  - AST construction with modern C++23 and smart pointers
  - Enhanced error diagnostics with file:line:column format
  - Visual error pointers showing source context

- **Language Features**:
  - Enum declarations
  - Class type declarations with inheritance (`inherits` keyword)
  - Field declarations with `feature` keyword
  - Field modifiers (cardinality and constraints)
  - Invariant constraints with comparison operators and literals
  - All primitive types (String, Int, Real, Bool, Timestamp, Timespan, Date, Guid)
  - Optional modifier syntax and shorthand syntax

- **Phase 1 (Semantic Analysis)**:
  - Symbol table construction
  - Type validation for all type references
  - Inheritance validation with cycle detection
  - Field uniqueness validation including inherited fields
  - Invariant validation (field reference checking)
  - Comprehensive error reporting

**🚧 Planned:**

- **Phase 2 (Code Generation)**:
  - C++ code generation (class definitions, inheritance hierarchies)
  - SQLite schema generation (tables, foreign keys, constraints)
  - Additional target languages
  - Additional SQL dialects

## Language Specification

### Keywords

- `class` - Define a new type
- `enum` - Define an enumeration
- `inherits` - Specify inheritance relationship
- `feature` - Declare a class field/attribute
- `invariant` - Declare a boolean constraint
- `optional` - Optional field modifier (equivalent to `[0..1]`)
- `unique` - Unique constraint modifier

### Primitive Types

- `String` - Text strings
- `Int` - Integer numbers
- `Real` - Floating-point numbers
- `Bool` - Boolean values (true/false)
- `Timestamp` - Points in time (seconds since epoch)
- `Timespan` - Durations (in seconds)
- `Date` - Calendar dates
- `Guid` - Globally unique identifier (for both type and instance identification)

### Comments

```bbfm
// Single-line comment

/*
   Multi-line comment
*/
```
