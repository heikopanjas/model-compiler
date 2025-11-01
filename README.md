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
- **Case-Insensitive**: Keywords and type names are case-insensitive

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

```
Podcast
├── typeId (Guid)                 // universal metadata
├── id (Guid)                     // universal metadata
├── cardinality (Int)             // universal metadata
├── creationDate (Timestamp)      // universal metadata
├── modificationDate (Timestamp)  // universal metadata
├── comment (String)              // universal metadata
├── title (String)                // defined in Podcast
└── description (String)          // defined in Podcast
```

User-defined inheritance works as expected. For example:

```bbfm
class Asset {
    String url;
}

class AudioAsset inherits Asset {
    String format;
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

Field cardinality and constraints are specified using square bracket notation:

- `[1]` - Mandatory single value (default if no modifier specified)
- `[0..1]` - Optional single value
- `[1..*]` - Array with at least one element
- `[0..*]` - Array that may be empty
- `[unique]` - Unique constraint
- Modifiers can be combined: `[1,unique]`

**Shorthand Syntax:**

- **Default cardinality**: Fields without modifiers default to `[1]` (mandatory)
  - `String title;` is equivalent to `String title[1];`
- **Optional shorthand**: Use `?` for optional fields
  - `String author?;` is equivalent to `String author[0..1];`

These shortcuts make the most common field types cleaner and more readable while maintaining full expressiveness when needed.

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
    String url;
}

// Define a derived type with inheritance
class AudioAsset inherits Asset {
    String format;
    Int fileSize;
}

// Define a type with relationships
class Podcast {
    String title;
    String description;
    String author?;              // shorthand for [0..1] (optional)
    String rssUrl[1,unique];     // mandatory + unique
    Episode episodes[0..*];      // one-to-many relationship
}

class Episode {
    String title;
    Date publishedAt;
    Timespan duration;
    MediaType mediaType;
    AudioAsset audio;            // one-to-one relationship
    Transcript transcript?;      // shorthand for [0..1] (optional)
}

class Transcript {
    String text;
    String language;
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
./model-compiler <source_file.bbfm>
```

Example:

```bash
./model-compiler ../examples/podcast.bbfm
```

## Project Structure

```text
model-compiler/
├── CMakeLists.txt          # CMake build configuration
├── README.md               # This file
├── AGENTS.md               # AI agent operating instructions
├── src/
│   ├── model-compiler.l   # Flex lexer specification
│   ├── model-compiler.y   # Bison parser specification
│   ├── Driver.cpp         # Compiler driver implementation
│   ├── AST.cpp            # AST implementation
│   └── main.cpp           # Main entry point (C++)
├── include/               # Header files
│   ├── Common.h           # Common macros and utilities
│   ├── Driver.h           # Compiler driver interface
│   └── AST.h              # AST node definitions
├── examples/              # Example programs
│   └── podcast.bbfm       # Podcast domain model example
└── _build/                # Build artifacts (gitignored)
```

## Compilation Phases

The compiler implements a multi-phase compilation process:

1. **Lexical Analysis** - Tokenizes the input source code
2. **Syntax Analysis** - Parses tokens into an abstract syntax tree
3. **AST Construction** - Builds complete Abstract Syntax Tree
4. **Semantic Analysis** - Type checking and validation (planned)
5. **Code Generation** - Generates target code (planned):
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

**Implemented:**

- Lexical analysis with case-insensitive keywords
- Syntax analysis supporting full grammar
- AST construction with modern C++23
- Enum declarations
- Class type declarations with inheritance
- Field modifiers (cardinality and constraints)
- All primitive types
- Optional field shorthand syntax (?)

**Planned:**

- Semantic analysis (type checking, inheritance validation)
- C++ code generation
- SQLite schema generation
- Additional target languages and SQL dialects

## Language Specification

### Keywords

- `class` - Define a new type
- `enum` - Define an enumeration
- `inherits` - Specify inheritance relationship
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

## Contributing

This is a personal project for experimenting with domain-specific language design and compiler implementation. See AGENTS.md for development guidelines and project decisions.

## License

TBD

## Author

Heiko
