# Model Compiler

A Domain-Specific Language (DSL) compiler for defining podcast object models and their relationships. Part of the Big Bad Feed Machine (BBFM) project, this compiler provides common infrastructure for podcast-related applications. The modeling language provides a simple, UML-inspired syntax for modeling podcast domain objects, which the compiler translates into source code.

## Overview

The BBFM modeling language enables you to define data types, relationships, and constraints for podcast domains using an expressive, type-safe syntax. The compiler generates:

- **Swift classes** with proper inheritance hierarchies

Future targets include additional programming languages.

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

These fields are automatically available on all types. **You never declare them in your class definitions.** When you create a type like:

```bbfm
class Podcast {
    feature title: String;
    feature description: String;
}
```

The generated code will include the universal metadata fields, so the actual structure is:

```bbfm
class Podcast {
    feature typeId: Guid;                 // universal metadata (automatic)
    feature id: Guid;                     // universal metadata (automatic)
    feature cardinality: Int;             // universal metadata (automatic)
    feature creationDate: Timestamp;      // universal metadata (automatic)
    feature modificationDate: Timestamp;  // universal metadata (automatic)
    feature comment: String;              // universal metadata (automatic)
    feature title: String;                // user-defined
    feature description: String;          // user-defined
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

Invariants are boolean constraints on class attributes that express domain rules and validation requirements using a powerful expression system:

- Declared using the `invariant` keyword
- Syntax: `invariant name: expression;`
- **Expression support**:
  - **Arithmetic operators**: `+`, `-`, `*`, `/`, `%`
  - **Comparison operators**: `<`, `>`, `<=`, `>=`, `==`, `!=`
  - **Logical operators**: `&&` (AND), `||` (OR), `!` (NOT)
  - **Parentheses**: For grouping and precedence control
  - **Literals**: Integer, real, string, boolean values
  - **Field references**: Access to class attributes
  - **Unary operators**: `-` (negation), `!` (logical NOT)

**Examples:**

```bbfm
class Rectangle {
    feature width: Int;
    feature height: Int;
    feature maxArea: Int;

    // Simple comparison
    invariant positiveWidth: width > 0;

    // Arithmetic expression
    invariant validArea: width * height <= maxArea;

    // Complex expression with parentheses
    invariant aspectRatio: (width + height) * 2 <= 1000;

    // Logical operators
    invariant validDimensions: width >= 10 && height >= 10;
}

class Temperature {
    feature celsius: Real;
    feature fahrenheit: Real;

    // Arithmetic with real numbers
    invariant conversion: fahrenheit == celsius * 1.8 + 32.0;

    // Negative numbers
    invariant aboveAbsoluteZero: celsius >= -273.15;
}

class AudioAsset inherits Asset {
    feature fileSize: Int;

    invariant maxFileSize: fileSize <= 500000000;  // Max 500MB
}
```

Invariants are validated during semantic analysis to ensure:

- All referenced fields exist (including inherited fields)
- Type compatibility of expressions
- Proper syntax and operator usage

The expression system enables declarative constraints that will generate validation code in target languages.

### Computed Features

Computed features are auto-calculated fields whose values are derived from other fields using expressions. They provide a declarative way to define derived attributes without writing imperative code.

- **Syntax**: `feature fieldName: TypeName = expression;`
- **Expression support**: Full expression system (arithmetic, comparison, logical, field references, member access)
- **Member access**: Access fields of nested objects using dot notation (`object.field`)
- **Type safety**: Expression result type must match declared field type
- **Constraints**: Computed features must have cardinality `[1]` (cannot be arrays or optional)

**Examples:**

```bbfm
class Rectangle {
    feature width: Int;
    feature height: Int;

    // Simple arithmetic
    feature area: Int = width * height;

    // Complex expression
    feature perimeter: Int = (width + height) * 2;
}

class Point {
    feature x: Int;
    feature y: Int;

    invariant validX: x >= 0;
    invariant validY: y >= 0;
}

class Shape {
    feature width: Int;
    feature height: Int;

    invariant positiveWidth: width > 0;
    invariant positiveHeight: height > 0;
}

class Rectangle inherits Shape {
    feature topLeft: Point;
    feature bottomRight: Point;

    // Member access expressions
    feature computedWidth: Int = bottomRight.x - topLeft.x;
    feature computedHeight: Int = bottomRight.y - topLeft.y;

    // Computed features can reference other computed features
    feature computedArea: Int = computedWidth * computedHeight;

    // Invariants can reference computed features
    invariant areaMatchesFields: computedArea == width * height;
}
```

**Type Checking:**

The compiler performs comprehensive type checking for computed features:

- **Type inference**: Recursively determines expression result types
- **Type compatibility**: Validates expression type matches declared field type
- **Type promotion**: Allows safe widening conversions (Int → Real), rejects narrowing (Real → Int)
- **Member access types**: Tracks types through `object.field` chains

```bbfm
class Example {
    feature intValue: Int;
    feature realValue: Real;

    // ✅ Valid: Int → Real promotion (widening)
    feature computed1: Real = intValue * 2;

    // ❌ Error: Real → Int would lose precision (narrowing)
    // feature computed2: Int = realValue * 2.0;

    // ✅ Valid: Exact type match
    feature computed3: Int = intValue + 5;
}
```

The semantic analyzer ensures all field references exist, member access chains are valid, and type conversions are safe before code generation.

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
    feature wordCount: Int;

    // Computed feature example
    feature isLongTranscript: Bool = wordCount > 10000;
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
# Basic compilation
./_build/model-compiler <source_file.fm>

# Dump AST (syntax tree) for debugging
./_build/model-compiler --dump-syntax-tree <source_file.fm>

# Dump symbol table after semantic analysis
./_build/model-compiler --dump-symbol-table <source_file.fm>

# Show help
./_build/model-compiler --help
```

Examples:

```bash
# Compile a file
./_build/model-compiler examples/podcast.fm

# View syntax tree
./_build/model-compiler --dump-syntax-tree examples/podcast.fm

# View symbol table
./_build/model-compiler --dump-symbol-table examples/podcast.fm
```

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
   - Full expression grammar with operator precedence
   - Builds expression AST nodes for invariants
2. **Phase 1: Semantic Analysis** ✅ - Type checking and validation
   - Symbol table construction
   - Type validation (primitives, enums, user-defined types)
   - Inheritance validation (cycle detection, base type verification)
   - Field uniqueness validation (including inherited fields)
   - Invariant validation (expression AST traversal, field reference checking)
   - Expression type inference and validation
3. **Phase 2: Code Generation** 🚧 - Planned:
   - Swift class definitions with inheritance
   - Invariant validation code generation

## Type Mappings

| BBFM Type | Swift |
|---------|-------|
| String | String |
| Int | Int64 |
| Real | Double |
| Bool | Bool |
| Timestamp | Double |
| Timespan | Double |
| Guid | String |

## Current Status

**✅ Implemented:**

- **Phase 0 (Lexical Analysis & Parsing)**:
  - Lexical analysis with case-sensitive keywords and types
  - Syntax analysis supporting full grammar
  - AST construction with modern C++23 and smart pointers
  - Expression grammar with operator precedence (arithmetic, comparison, logical)
  - Enhanced error diagnostics with file:line:column format
  - Visual error pointers showing source context

- **Language Features**:
  - Enum declarations
  - Class type declarations with inheritance (`inherits` keyword)
  - Field declarations with `feature` keyword
  - Field modifiers (cardinality and constraints)
  - **Computed features** with initializer expressions:
    - Syntax: `feature name: Type = expression;`
    - Member access expressions: `object.field`
    - Type inference and validation
    - Type promotion rules (Int → Real safe, Real → Int error)
  - **Expression system** with full operator support:
    - Arithmetic: `+`, `-`, `*`, `/`, `%`
    - Comparison: `<`, `>`, `<=`, `>=`, `==`, `!=`
    - Logical: `&&`, `||`, `!`
    - Parentheses for grouping
    - Literals: integer, real, string, boolean
    - Field references and complex nested expressions
    - Member access: `object.field` for nested properties
  - Invariant constraints using expression AST
  - All primitive types (String, Int, Real, Bool, Timestamp, Timespan, Date, Guid)
  - Optional modifier syntax and shorthand syntax

- **Phase 1 (Semantic Analysis)**:
  - Symbol table construction
  - Type validation for all type references
  - Inheritance validation with cycle detection
  - Field uniqueness validation including inherited fields
  - Invariant validation using expression AST traversal
  - Expression field reference validation
  - **Computed features validation**:
    - Field reference validation (including inherited fields)
    - Member access validation (object.field chains)
    - Type inference for expressions
    - Type compatibility checking with promotion rules
    - Cardinality validation (must be `[1]`)
  - Comprehensive error reporting

**🚧 Planned:**

- **Phase 2 (Code Generation)**:
  - Swift code generation (class definitions, inheritance hierarchies)
  - Invariant validation code generation
  - Additional target languages

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
