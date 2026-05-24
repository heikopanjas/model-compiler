# BBFM Model Compiler

A Domain-Specific Language (DSL) compiler for defining podcast object models and their relationships. Part of the Big Bad Feed Machine (BBFM) project, this compiler provides common infrastructure for podcast-related applications. The modeling language provides a simple, UML-inspired syntax for modeling podcast domain objects, which the compiler translates into source code.

**License:** MIT  
**Language:** C++23 (minimum C++17)  
**Version:** v0.1.0 — Phases 0–2 complete (C++ code generation; Rust experimental)

## Overview

The BBFM modeling language enables you to define data types, relationships, and constraints for podcast domains using an expressive, type-safe syntax. The compiler generates:

- **C++ header files** with class hierarchies, field wrappers, invariants, and computed features
- **Rust source files** (experimental) via `--lang rust`

Additional target languages are planned. See [The FM Language](#the-fm-language) for the full language reference.

## The FM Language

The **FM language** (`.fm` files) is the BBFM modeling DSL. It provides a UML-inspired, declarative syntax for describing domain object models: types, fields, relationships, constraints, and derived values. The compiler validates FM source and generates target-language code.

### Design Philosophy

The language is inspired by UML class diagrams but deliberately simplified. It focuses on **data modeling** without visibility modifiers, abstract types, interfaces, or stereotypes. Keywords are lowercase and case-sensitive; user-defined type names use PascalCase and are case-sensitive.

### Source File Structure

An FM source file consists of an optional namespace declaration followed by zero or more top-level declarations:

```bbfm
[ namespace NamespaceName ; ]
{ enum_declaration | class_declaration }*
```

Each file is compiled independently (multi-file compilation is not yet supported). All referenced types must be defined in the same file.

### Lexical Elements

**Comments**

```bbfm
// Single-line comment

/*
   Multi-line comment
*/
```

**Identifiers**

- Start with a letter, followed by letters, digits, or `_`
- Used for type names, field names, enum values, and namespace names
- Field names may match primitive type names (e.g. `feature string: String;`) — the parser disambiguates by context

**Literals**

| Literal | Examples |
|---------|----------|
| Integer | `0`, `42`, `1000` |
| Real | `3.14`, `1.8`, `-273.15`, `1.5e10` |
| String | `"hello"`, `"ISO 8601 date"` |
| Boolean | `true`, `false` |

**Keywords** (reserved, lowercase)

`class`, `enum`, `inherits`, `feature`, `invariant`, `alias`, `namespace`, `optional`, `unique`

**Primitive types** (PascalCase)

`String`, `Int`, `Real`, `Bool`, `Timestamp`, `Timespan`, `Date`, `Guid`

| Type | Description |
|------|-------------|
| `String` | Text strings |
| `Int` | Integer numbers |
| `Real` | Floating-point numbers |
| `Bool` | Boolean values |
| `Timestamp` | Point in time (seconds since epoch) |
| `Timespan` | Duration (seconds) |
| `Date` | Calendar date |
| `Guid` | Globally unique identifier |

### Namespaces

Namespaces organize generated code and prevent naming conflicts.

```bbfm
namespace models;

class Podcast {
    feature title: String;
}
```

| Rule | Description |
|------|-------------|
| Placement | Must appear before all other declarations |
| Count | At most one namespace declaration per file |
| Form | Flat names only (no `a.b.c` nesting in source) |
| CLI nesting | Combine with `--target-namespace myapp` for nested output namespaces |

Generated C++ with `--target-namespace myapp`:

```cpp
namespace myapp {
namespace models {
    class Podcast { /* ... */ };
}
}
```

### Enumerations

Enums define categorical values referenced by field types:

```bbfm
enum MediaType {
    AUDIO,
    VIDEO
}

enum Status {
    DRAFT,
    PUBLISHED,
    ARCHIVED
}
```

Enum value names are identifiers separated by commas.

### Classes and Inheritance

Classes define structured types with fields, computed features, aliases, and invariants:

```bbfm
class Asset {
    feature url: String;
    feature title: String;
}

class AudioAsset inherits Asset {
    feature format: String;
    feature fileSize: Int;

    invariant maxFileSize: fileSize <= 500000000;
}
```

| Rule | Description |
|------|-------------|
| Inheritance | Single inheritance only (`inherits BaseType`) |
| Base type | Must reference a previously declared class |
| Cycles | Circular inheritance is rejected |
| Body | Fields, computed features, aliases, and invariants appear inside `{ }` |

### Features (Fields)

Fields are declared with the `feature` keyword:

```bbfm
feature fieldName: TypeName [ modifiers ] [ = expression ] ;
```

| Form | Meaning |
|------|---------|
| `feature name: Type;` | Mandatory single-valued field (default `[1]`) |
| `feature name: Type [modifiers];` | Field with explicit cardinality or constraints |
| `feature name: Type = expr;` | Computed feature (derived, read-only) |

Type references may be primitive types, enum names, or user-defined class names.

#### Cardinality and Constraints

Modifiers appear in square brackets. Multiple modifiers are comma-separated.

| Modifier | Meaning |
|----------|---------|
| *(none)* | Equivalent to `[1]` — exactly one value, required |
| `[1]` | Mandatory single value |
| `[0..1]` or `[optional]` | Zero or one value |
| `[1..*]` | Array with at least one element |
| `[0..*]` | Array that may be empty |
| `[unique]` | Unique constraint (combine with cardinality) |

```bbfm
feature title: String;                      // [1] by default
feature author: String [optional];          // [0..1]
feature rssUrl: String [1,unique];          // mandatory + unique
feature episodes: Episode [0..*];           // zero or more
feature tags: Tag [1..*];                   // one or more
feature optionalUnique: String [optional,unique];
```

#### Modeling Relationships

FM models associations through typed fields and cardinality — there is no separate relationship keyword:

| Pattern | FM syntax | Generated shape |
|---------|-----------|-----------------|
| One-to-one | `feature child: Child;` | Single field |
| One-to-one optional | `feature child: Child [optional];` | Optional field |
| One-to-many | `feature items: Item [0..*];` or `[1..*]` | Collection |
| Many-to-many | Two inverse `[0..*]` fields, one on each class | Two collections |

### Computed Features

Computed features derive their value from an expression. They are read-only and evaluated on access in generated code.

```bbfm
class Rectangle {
    feature width: Int;
    feature height: Int;

    feature area: Int = width * height;
    feature perimeter: Int = (width + height) * 2;
}
```

Member access traverses nested objects:

```bbfm
class Rectangle inherits Shape {
    feature topLeft: Point;
    feature bottomRight: Point;

    feature computedWidth: Int = bottomRight.x - topLeft.x;
    feature computedHeight: Int = bottomRight.y - topLeft.y;
    feature computedArea: Int = computedWidth * computedHeight;

    invariant areaMatchesFields: computedArea == width * height;
}
```

| Rule | Description |
|------|-------------|
| Cardinality | Must be `[1]` — not optional, not an array |
| Type | Expression result must match declared type (with promotion rules below) |
| Promotion | `Int → Real` widening allowed; `Real → Int` rejected |
| References | Must reference defined fields (including inherited) |
| Optional fields | Cannot be referenced in computed expressions |
| Chaining | May reference other computed features in the same class |

```bbfm
class Example {
    feature intValue: Int;
    feature realValue: Real;

    feature asReal: Real = intValue * 2;        // OK: Int → Real
    feature asInt: Int = intValue + 5;          // OK: exact match
    // feature bad: Int = realValue * 2.0;      // Error: Real → Int
}
```

### Alias Fields

Aliases provide an alternate name for an existing field, typically to give semantic meaning to inherited members:

```bbfm
class Event {
    feature timestamp: Timestamp;

    invariant validTimestamp: timestamp >= 0;
}

class ScheduledEvent inherits Event {
    feature title: String;

    alias startTime = timestamp;
}
```

| Rule | Description |
|------|-------------|
| Syntax | `alias aliasName = targetFieldName;` |
| Target | Must be a simple field name in scope (including inherited) |
| Semantics | Read-write forwarding; writes validate target field invariants |
| Restrictions | Cannot target computed features or other aliases |

### Invariants

Invariants declare boolean constraints on class state. Generated code validates them when relevant fields are written.

```bbfm
invariant name: expression ;
```

```bbfm
class Rectangle {
    feature width: Int;
    feature height: Int;
    feature maxArea: Int;

    invariant positiveWidth: width > 0;
    invariant validArea: width * height <= maxArea;
    invariant validDimensions: width >= 10 && height >= 10;
}

class Temperature {
    feature celsius: Real;
    feature fahrenheit: Real;

    invariant conversion: fahrenheit == celsius * 1.8 + 32.0;
    invariant aboveAbsoluteZero: celsius >= -273.15;
}
```

During semantic analysis the compiler verifies that all referenced fields exist (including inherited fields). In generated C++ output, invariant checkers follow the `Require_<field>_<invariant>` naming pattern.

### Expression Language

Expressions are used in invariants and computed features.

**Operators** (precedence from lowest to highest):

| Precedence | Operators | Description |
|------------|-----------|-------------|
| 1 | `||` | Logical OR |
| 2 | `&&` | Logical AND |
| 3 | `==` `!=` | Equality |
| 4 | `<` `>` `<=` `>=` | Comparison |
| 5 | `+` `-` | Addition, subtraction |
| 6 | `*` `/` `%` | Multiplication, division, modulo |
| 7 | `!` `-` | Logical NOT, unary negation |
| 8 | `.` | Member access |

**Operands**

- **Literals** — integer, real, string, boolean
- **Field references** — bare names refer to fields visible on the current class (including inherited)
- **Member access** — `object.field` chains for nested class fields
- **Parentheses** — `( expression )` for grouping

**Type inference**

- Arithmetic on `Int` operands yields `Int`; mixed with `Real`, `Timestamp`, or `Timespan` yields `Real`
- Comparisons and logical operators yield `Bool`
- String `+` concatenates when both operands are `String`

### Universal Metadata Fields

Every class instance automatically carries six metadata fields. These are part of the type system and **must never be declared** in FM source. Generated C++ provides them through inheritance from `bbfm::runtime::Fabric`.

| Field | Type | Purpose |
|-------|------|---------|
| `typeId` | Guid | Identifier shared by all instances of a type |
| `id` | Guid | Identifier unique to each instance |
| `cardinality` | Int | Relationship cardinality metadata |
| `creationDate` | Timestamp | Instance creation time |
| `modificationDate` | Timestamp | Last modification time |
| `comment` | String | User notes |

```bbfm
class Podcast {
    feature title: String;
    feature description: String;
}
```

The effective instance shape includes universal metadata plus user-defined fields. Subclasses inherit base-class fields; universal metadata is always present on every type.

```bbfm
class Asset {
    feature url: String;
}

class AudioAsset inherits Asset {
    feature format: String;
}
```

An `AudioAsset` instance conceptually contains: universal metadata (6 fields) + `Asset` fields (`url`) + `AudioAsset` fields (`format`).

### Semantic Validation

The compiler performs multi-phase validation on every FM file:

| Check | Description |
|-------|-------------|
| Syntax | Grammar, punctuation, keyword casing |
| Type references | All field types and base types must be defined |
| Inheritance | No cycles; base must be a class |
| Field uniqueness | No duplicate names within a class (including inherited) |
| Invariants | Referenced fields must exist |
| Computed features | Valid references, member access, type compatibility, cardinality `[1]`, no optional references |
| Aliases | Valid targets; no alias chaining |

Errors report `file:line:column` with source context and a caret marker.

## Complete Example

The following podcast domain model combines enums, inheritance, field modifiers, relationships, computed features, invariants, and aliases in a single file. See also `examples/podcast.fm` and `examples/comprehensive_test.fm`.

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
    feature wordCount: Int;

    // Computed feature example
    feature isLongTranscript: Bool = wordCount > 10000;
}

class Tag {
    feature timestamp: Timestamp;
    
    invariant validTimestamp: timestamp >= 0;
}

class NamedTag inherits Tag {
    feature name: String;
    
    // Alias provides semantic name for inherited field
    alias createdAt = timestamp;
}
```

## Prerequisites

- CMake 3.20 or higher
- C++23-compatible compiler (GCC 11+ or Clang 14+, minimum C++17 support required)
- Flex 2.6+ (lexical analyzer generator)
- Bison 3.8+ (parser generator)
- Ninja (build system, recommended)

**Note:** The [cxxopts](https://github.com/jarro2783/cxxopts) CLI library is fetched automatically by CMake via `FetchContent`; no separate install is required.

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
# macOS / Linux
./build.sh

# Windows (PowerShell)
./build.ps1
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
# Validation only (no code generation, checks syntax and semantics)
./_build/model-compiler <source_file.fm>

# Generate C++ header
./_build/model-compiler --lang c++ <source_file.fm>

# Generate Rust source (experimental; specify .rs output explicitly)
./_build/model-compiler --lang rust <source_file.fm> -o output.rs

# Generate with custom output file
./_build/model-compiler --lang c++ <source_file.fm> -o output.h

# Generate with namespace and class prefix
./_build/model-compiler --lang c++ --target-namespace myapp --target-class-prefix FM <source_file.fm>

# Validation with AST dump (no code generation)
./_build/model-compiler --dump-syntax-tree <source_file.fm>

# Validation with symbol table dump (no code generation)
./_build/model-compiler --dump-symbol-table <source_file.fm>

# Show version or help
./_build/model-compiler --version
./_build/model-compiler --help
```

When `--lang` is omitted, the compiler runs validation only (Phases 0 and 1). When `--lang` is specified, it also runs Phase 2 code generation. The default output extension is `.h`; use `-o` to set the path explicitly (required for Rust output). Only a single input file is supported per invocation.

Supported `--lang` values: `c++`, `rust` (experimental).

Examples:

```bash
# Validate file (syntax and semantic checking only)
./_build/model-compiler examples/podcast.fm

# Generate C++ header (creates examples/podcast.h)
./_build/model-compiler --lang c++ examples/podcast.fm

# Generate with custom output path
./_build/model-compiler --lang c++ examples/podcast.fm -o generated/podcast.h

# Generate with namespace
./_build/model-compiler --lang c++ --target-namespace myapp examples/podcast.fm

# Generate Rust (experimental)
./_build/model-compiler --lang rust examples/test_computed_simple.fm -o examples/test_computed_simple.rs

# View syntax tree (validation only, no code generation)
./_build/model-compiler --dump-syntax-tree examples/podcast.fm

# View symbol table (validation only, no code generation)
./_build/model-compiler --dump-symbol-table examples/podcast.fm
```

### Example Files

The `examples/` directory contains a comprehensive test suite demonstrating all language features:

**Main Examples:**

- `podcast.fm` - Complete podcast domain model with classes, enums, inheritance, invariants, and computed features
- `comprehensive_test.fm` - Tests all language features in a single file

**Feature-Specific Tests:**

- `test_computed_simple.fm` - Basic computed features
- `test_computed_member_access.fm` - Computed features with member access (object.field)
- `test_computed_inheritance.fm` - Computed features in inheritance hierarchies
- `test_expressions.fm` - Full expression system (arithmetic, logical, comparison)
- `test_namespace.fm` - Source-file namespace declarations
- `test_type_promotion_ok.fm` - Valid type promotions (Int → Real)

**Error Validation Tests:**

- `error_test_suite.fm` - Index of error scenarios (cases are block-commented; file validates as empty input)
- `test_circular_inheritance.fm` - Circular inheritance detection
- `test_duplicate_field.fm` - Duplicate field detection
- `test_undefined_type.fm` - Undefined type reference detection
- `test_type_error_int_real.fm` - Type mismatch errors (Real → Int)
- `test_type_error_member_access.fm` - Invalid member access in expressions
- `test_computed_error_*.fm` - Computed feature validation errors
- `test_bad_invariant.fm` - Invalid invariant expressions
- `test_missing_semicolon.fm` - Syntax error reporting
- `test_error_column.fm` - Error column position accuracy
- `test_wrong_case.fm` - Case sensitivity validation

### Symbol Table Dump

The `--dump-symbol-table` option displays the symbol table after semantic analysis, providing detailed information about all types, fields, invariants, and computed features in the program.

**Output Format:**

The symbol table dump shows:

- **Primitive types**: All built-in types (String, Int, Real, Bool, Timestamp, Timespan, Date, Guid)
- **Enumerations**: Enum names with their values
- **Classes**: User-defined types with inheritance, features, invariants, and computed features

**Field Origin Notation:**

The dump uses qualified name syntax to indicate where features and invariants are declared:

- `Self::fieldName` - Field declared in the current class
- `Base::fieldName` - Field inherited from a base class

**Example Output:**

```text
Symbol Table:
  Total symbols: 11 (8 primitives, 1 enums, 2 classes)

Primitive Types:
  String
  Int
  Real
  Bool
  Timestamp
  Timespan
  Date
  Guid

Enumerations:
  enum MediaType { AUDIO, VIDEO }

Classes:
  class Asset {
    Features:
      Self::url: String [1..1]
  }

  class AudioAsset inherits Asset {
    Features:
      Base::url: String [1..1]
      Self::format: String [1..1]
      Self::fileSize: Int [1..1]
    Invariants:
      Self::maxFileSize: (fileSize <= 500000000)
  }

  class Shape {
    Features:
      Self::width: Int [1..1]
      Self::height: Int [1..1]
      Self::area: Int [1..1] = Self::width * Self::height
    Invariants:
      Self::positiveWidth: (width > 0)
      Self::positiveHeight: (height > 0)
  }

  class ColoredShape inherits Shape {
    Features:
      Base::width: Int [1..1]
      Base::height: Int [1..1]
      Base::area: Int [1..1] = Base::width * Base::height
      Self::depth: Int [1..1]
      Self::color: String [1..1]
      Self::volume: Int [1..1] = Base::width * Base::height * Self::depth
      Self::doubleArea: Int [1..1] = Base::area * 2
    Invariants:
      Base::positiveWidth: (width > 0)
      Base::positiveHeight: (height > 0)
      Self::positiveDepth: (depth > 0)
  }
```

**Computed Features with Expressions:**

Computed features show their complete expression with field origin annotations. For example:

- `Self::area: Int [1..1] = Self::width * Self::height` - Uses fields from current class
- `Self::volume: Int [1..1] = Base::width * Base::height * Self::depth` - Mix of inherited and local fields
- `Self::doubleArea: Int [1..1] = Base::area * 2` - References inherited computed feature

This visualization helps developers understand:

- Which fields are locally declared vs. inherited
- How computed features derive their values
- What constraints (invariants) apply to each class
- The complete interface of each type including inherited members

## Testing

The project uses CTest. After building, run the test suite from the build directory:

```bash
cmake --build _build
ctest --test-dir _build --output-on-failure
```

Tests include:

- **`.fm` fixture tests** — positive examples must compile successfully; negative examples must fail with the expected error
- **`guid-test`** — runtime unit tests for `bbfm::runtime::Guid` (`tests/runtime/GuidTest.cpp`)

## Project Structure

```text
model-compiler/
├── CMakeLists.txt              # CMake build configuration
├── build.sh                    # Build script (macOS/Linux)
├── build.ps1                   # Build script (Windows)
├── README.md                   # This file
├── AGENTS.md                   # AI agent operating instructions
├── src/                        # Compiler source
│   ├── model-compiler.l        # Flex lexer specification
│   ├── model-compiler.y        # Bison parser specification
│   ├── main.cpp                # CLI entry point
│   ├── Driver.cpp              # Compilation phase orchestration
│   ├── AST.cpp                 # AST implementation
│   ├── SemanticAnalyzer.cpp    # Semantic analysis
│   ├── CodeGenerator.cpp       # Code generator base class
│   ├── CppCodeGenerator.cpp    # C++ code generation
│   ├── RustCodeGenerator.cpp   # Rust code generation (experimental)
│   └── Console.cpp             # Error reporting and output formatting
├── include/                    # Public headers
│   ├── AST.h
│   ├── CodeGenerator.h
│   ├── Common.h
│   ├── Console.h
│   ├── Contracts.h             # Require/RequireReturn precondition macros
│   ├── CppCodeGenerator.h
│   ├── Driver.h
│   ├── RustCodeGenerator.h
│   ├── SemanticAnalyzer.h
│   └── runtime/                # Generated-code runtime library
│       ├── Fabric.h            # Universal metadata base class
│       ├── Guid.h / Guid.cpp
│       ├── String.h
│       ├── Date.h
│       └── BoundedValue.h      # Field wrapper types
├── examples/                   # Example and fixture `.fm` files
├── tests/runtime/              # Runtime unit tests
└── _build/                     # Build artifacts (gitignored)
```

## Compilation Phases

The compiler implements a multi-phase compilation process:

1. **Phase 0: Lexical Analysis & Parsing** ✅ - Tokenizes input and parses into AST
   - Full expression grammar with operator precedence
   - Builds expression AST nodes for invariants
2. **Phase 1: Semantic Analysis** ✅ - Type checking and validation
   - Symbol table construction
   - Type validation (primitives, enums, user-defined types)
   - Inheritance validation (cycle detection, base type verification)
   - Field uniqueness validation (including inherited fields)
   - Invariant validation (expression AST traversal, field reference checking)
   - Expression type inference and validation
3. **Phase 2: Code Generation** ✅ — Generates target-language source when `--lang` is specified:
   - **C++**: Header files with `Fabric` inheritance, field wrappers, getters, computed features, and invariant checkers
   - **Rust** (experimental): Source files with structs, enums, and partial validation support

## Type Mappings

Current C++ mappings for BBFM primitive types:

| BBFM Type | C++ |
|-----------|-----|
| String | `bbfm::runtime::String` |
| Int | `int64_t` |
| Real | `double` |
| Bool | `bool` |
| Timestamp | `double` |
| Timespan | `double` |
| Date | `bbfm::runtime::Date` |
| Guid | `bbfm::runtime::Guid` |

Universal metadata fields (`typeId`, `id`, `cardinality`, `creationDate`, `modificationDate`, `comment`) are provided by inheriting from `bbfm::runtime::Fabric`.

## Status

**Completed:**

- Phase 0: Lexical analysis and parsing with expression grammar and diagnostics
- Phase 1: Semantic analysis with symbol tables, inheritance checks, invariants, computed features, and aliases
- Phase 2: C++ code generation with field wrappers, invariant checkers, namespaces, and prefixes

**Experimental:**

- `--lang rust` — Rust code generation is wired in but incomplete; output may be missing features

**Planned:**

- Additional target languages
- Implementation file (`.cpp`) generation
- Constructor implementations
- Serialization support

## Development

### Code Organization

The compiler is organized into several key components:

- **Lexer** (`model-compiler.l`) - Tokenizes input using Flex
- **Parser** (`model-compiler.y`) - Parses tokens into AST using Bison
- **AST** (`AST.h/cpp`) - Abstract Syntax Tree node definitions
- **SemanticAnalyzer** (`SemanticAnalyzer.h/cpp`) - Type checking and validation
- **CodeGenerator** (`CodeGenerator.h/cpp`) - Abstract code generation interface
- **CppCodeGenerator** (`CppCodeGenerator.h/cpp`) - C++ header generation
- **RustCodeGenerator** (`RustCodeGenerator.h/cpp`) - Experimental Rust generation
- **Driver** (`Driver.h/cpp`) - Orchestrates compilation phases
- **Console** (`Console.h/cpp`) - Error reporting and output formatting
- **Runtime** (`include/runtime/`) - Types used by generated C++ code (`Fabric`, `Guid`, field wrappers)

### Coding Standards

This project follows strict C++ coding standards documented in `AGENTS.md`:

- C++23 standard (minimum C++17 compatibility)
- All code in `bbfm` namespace
- Include guards format: `__BBFM_CLASS_NAME_H_INCL__`
- Smart pointers for resource management
- Const correctness throughout
- Doxygen documentation for all public APIs
- 8-byte alignment pragmas in headers

### Contributing

For detailed development guidelines, coding conventions, and architectural decisions, see `AGENTS.md`.

## License

MIT License - see LICENSE file for details.
