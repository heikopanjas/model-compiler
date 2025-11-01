#ifndef __BBFM_AST_H_INCL__
#define __BBFM_AST_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

#include <memory>
#include <string>
#include <vector>

namespace bbfm {
// Forward declarations
class ASTNode;
class AST;
class Declaration;
class EnumDeclaration;
class ClassDeclaration;
class Field;
class Invariant;
class TypeSpec;
class Modifier;
class Expression;
class BinaryExpression;
class UnaryExpression;
class FieldReference;
class MemberAccessExpression;
class LiteralExpression;
class FunctionCall;
class ParenthesizedExpression;

// ============================================================================
// Base AST Node
// ============================================================================

/// \brief Base class for all AST nodes
class ASTNode
{
public:
    virtual ~ASTNode() = default;

    /// \brief Dump the AST node to stdout for debugging
    /// \param indent Indentation level for pretty printing
    virtual void Dump(int indent = 0) const = 0;

protected:
    /// \brief Print indentation for pretty printing
    /// \param indent Number of indentation levels
    void PrintIndent(int indent) const;
};

// ============================================================================
// Type Specification
// ============================================================================

/// \brief Enumeration of primitive types in BBFM language
enum class PrimitiveType
{
    STRING,
    INT,
    REAL,
    BOOL,
    TIMESTAMP,
    TIMESPAN,
    DATE,
    GUID
};

/// \brief Base class for type specifications
class TypeSpec : public ASTNode
{
public:
    virtual ~TypeSpec() = default;

    /// \brief Check if this is a primitive type
    /// \return True if primitive type, false otherwise
    virtual bool IsPrimitive() const = 0;

    /// \brief Check if this is a user-defined type
    /// \return True if user-defined type, false otherwise
    virtual bool IsUserDefined() const = 0;
};

/// \brief Type specification for primitive types
class PrimitiveTypeSpec : public TypeSpec
{
public:
    /// \brief Construct a primitive type specification
    /// \param type The primitive type
    explicit PrimitiveTypeSpec(const PrimitiveType type) : type_(type) {}

    /// \brief Get the primitive type
    /// \return The primitive type
    PrimitiveType GetType() const;

    bool IsPrimitive() const override;

    bool IsUserDefined() const override;

    void Dump(int indent = 0) const override;

    /// \brief Convert primitive type to string representation
    /// \param type The primitive type to convert
    /// \return String representation of the type
    static const char* TypeToString(const PrimitiveType type);

private:
    PrimitiveType type_;
};

/// \brief Type specification for user-defined types
class UserDefinedTypeSpec : public TypeSpec
{
public:
    /// \brief Construct a user-defined type specification
    /// \param name The name of the user-defined type
    explicit UserDefinedTypeSpec(const std::string& name) : typeName_(name) {}

    /// \brief Get the type name
    /// \return The name of the user-defined type
    const std::string& GetTypeName() const;

    bool IsPrimitive() const override;

    bool IsUserDefined() const override;

    void Dump(int indent = 0) const override;

private:
    std::string typeName_;
};

// ============================================================================
// Modifiers (Cardinality and Constraints)
// ============================================================================

/// \brief Type of field modifier
enum class ModifierType
{
    CARDINALITY, // [1], [0..1], [1..*], [0..*]
    UNIQUE       // [unique]
};

/// \brief Base class for field modifiers
class Modifier : public ASTNode
{
public:
    /// \brief Construct a modifier
    /// \param type The type of modifier
    explicit Modifier(const ModifierType type) : type_(type) {}

    virtual ~Modifier() = default;

    /// \brief Get the modifier type
    /// \return The modifier type
    ModifierType GetType() const;

private:
    ModifierType type_;
};

/// \brief Cardinality modifier for fields (e.g., [1], [0..1], [0..*])
class CardinalityModifier : public Modifier
{
public:
    /// \brief Construct a cardinality modifier
    /// \param min Minimum cardinality
    /// \param max Maximum cardinality (-1 for unbounded)
    CardinalityModifier(const int min, const int max) : Modifier(ModifierType::CARDINALITY), minCardinality_(min), maxCardinality_(max) {}

    /// \brief Get minimum cardinality
    /// \return Minimum cardinality value
    int GetMin() const;

    /// \brief Get maximum cardinality
    /// \return Maximum cardinality value (-1 for unbounded)
    int GetMax() const;

    /// \brief Check if cardinality is unbounded
    /// \return True if unbounded (max is *)
    bool IsUnbounded() const;

    /// \brief Check if field is optional
    /// \return True if minimum cardinality is 0
    bool IsOptional() const;

    /// \brief Check if field is mandatory
    /// \return True if minimum cardinality is greater than 0
    bool IsMandatory() const;

    /// \brief Check if field is an array
    /// \return True if maximum cardinality is unbounded or greater than 1
    bool IsArray() const;

    void Dump(int indent = 0) const override;

private:
    int minCardinality_;
    int maxCardinality_; // -1 for unbounded (*)
};

/// \brief Unique constraint modifier for fields
class UniqueModifier : public Modifier
{
public:
    /// \brief Construct a unique modifier
    UniqueModifier() : Modifier(ModifierType::UNIQUE) {}

    void Dump(int indent = 0) const override;
};

// ============================================================================
// Expression System
// ============================================================================

/// \brief Base class for all expressions in BBFM language
class Expression : public ASTNode
{
public:
    /// \brief Expression result types
    enum class Type
    {
        INT,
        REAL,
        BOOL,
        STRING,
        TIMESTAMP,
        TIMESPAN,
        DATE,
        GUID,
        VOID,
        UNKNOWN
    };

    virtual ~Expression() = default;

    /// \brief Get the result type of this expression
    /// \return The expression result type
    virtual Type GetResultType() const = 0;

    /// \brief Convert expression to string representation
    /// \return String representation of the expression
    virtual std::string ToString() const = 0;
};

/// \brief Binary expression (arithmetic, comparison, logical operations)
class BinaryExpression : public Expression
{
public:
    /// \brief Binary operators
    enum class Op
    {
        // Arithmetic
        ADD, // +
        SUB, // -
        MUL, // *
        DIV, // /
        MOD, // %
             // Comparison
        LT,  // <
        GT,  // >
        LE,  // <=
        GE,  // >=
        EQ,  // ==
        NE,  // !=
            // Logical (future)
        AND, // &&
        OR   // ||
    };

    /// \brief Construct a binary expression
    /// \param left Left operand
    /// \param op Binary operator
    /// \param right Right operand
    BinaryExpression(std::unique_ptr<Expression> left, const Op op, std::unique_ptr<Expression> right);

    Type        GetResultType() const override;
    std::string ToString() const override;
    void        Dump(int indent = 0) const override;

    /// \brief Get the left operand
    /// \return Pointer to left expression
    const Expression* GetLeft() const;

    /// \brief Get the right operand
    /// \return Pointer to right expression
    const Expression* GetRight() const;

    /// \brief Get the operator
    /// \return The binary operator
    Op GetOperator() const;

    /// \brief Convert operator to string
    /// \param op The operator to convert
    /// \return String representation of operator
    static const char* OpToString(const Op op);

private:
    std::unique_ptr<Expression> left_;
    std::unique_ptr<Expression> right_;
    Op                          op_;
};

/// \brief Unary expression (negation, logical not)
class UnaryExpression : public Expression
{
public:
    /// \brief Unary operators
    enum class Op
    {
        NEG, // - (numeric negation)
        NOT  // ! (logical not)
    };

    /// \brief Construct a unary expression
    /// \param op Unary operator
    /// \param operand The operand
    UnaryExpression(const Op op, std::unique_ptr<Expression> operand);

    Type        GetResultType() const override;
    std::string ToString() const override;
    void        Dump(int indent = 0) const override;

    /// \brief Get the operand
    /// \return Pointer to operand expression
    const Expression* GetOperand() const;

    /// \brief Get the operator
    /// \return The unary operator
    Op GetOperator() const;

    /// \brief Convert operator to string
    /// \param op The operator to convert
    /// \return String representation of operator
    static const char* OpToString(const Op op);

private:
    Op                          op_;
    std::unique_ptr<Expression> operand_;
};

/// \brief Field reference expression
class FieldReference : public Expression
{
public:
    /// \brief Construct a field reference
    /// \param fieldName The name of the field
    explicit FieldReference(const std::string& fieldName);

    Type        GetResultType() const override;
    std::string ToString() const override;
    void        Dump(int indent = 0) const override;

    /// \brief Get the field name
    /// \return The field name
    const std::string& GetFieldName() const;

private:
    std::string fieldName_;
};

/// \brief Member access expression (object.field)
class MemberAccessExpression : public Expression
{
public:
    /// \brief Construct a member access expression
    /// \param object The object expression
    /// \param memberName The name of the member being accessed
    MemberAccessExpression(std::unique_ptr<Expression> object, const std::string& memberName);

    Type        GetResultType() const override;
    std::string ToString() const override;
    void        Dump(int indent = 0) const override;

    /// \brief Get the object expression
    /// \return The object expression
    const Expression* GetObject() const;

    /// \brief Get the member name
    /// \return The member name
    const std::string& GetMemberName() const;

private:
    std::unique_ptr<Expression> object_;
    std::string                 memberName_;
};

/// \brief Literal value expression
class LiteralExpression : public Expression
{
public:
    /// \brief Construct an integer literal
    /// \param value The integer value
    explicit LiteralExpression(const int64_t value);

    /// \brief Construct a real (floating-point) literal
    /// \param value The real value
    explicit LiteralExpression(const double value);

    /// \brief Construct a string literal
    /// \param value The string value
    explicit LiteralExpression(const std::string& value);

    /// \brief Construct a boolean literal
    /// \param value The boolean value
    explicit LiteralExpression(const bool value);

    Type        GetResultType() const override;
    std::string ToString() const override;
    void        Dump(int indent = 0) const override;

    /// \brief Get the integer value (if type is INT)
    /// \return The integer value
    int64_t GetIntValue() const;

    /// \brief Get the real value (if type is REAL, TIMESTAMP, or TIMESPAN)
    /// \return The real value
    double GetRealValue() const;

    /// \brief Get the string value (if type is STRING or GUID)
    /// \return The string value
    const std::string& GetStringValue() const;

    /// \brief Get the boolean value (if type is BOOL)
    /// \return The boolean value
    bool GetBoolValue() const;

private:
    Type        type_;
    int64_t     intValue_;
    double      realValue_;
    std::string stringValue_;
    bool        boolValue_;
};

/// \brief Function call expression
class FunctionCall : public Expression
{
public:
    /// \brief Construct a function call
    /// \param functionName The name of the function
    /// \param arguments The function arguments
    FunctionCall(const std::string& functionName, std::vector<std::unique_ptr<Expression>> arguments);

    Type        GetResultType() const override;
    std::string ToString() const override;
    void        Dump(int indent = 0) const override;

    /// \brief Get the function name
    /// \return The function name
    const std::string& GetFunctionName() const;

    /// \brief Get the arguments
    /// \return Vector of argument expressions
    const std::vector<std::unique_ptr<Expression>>& GetArguments() const;

private:
    std::string                              functionName_;
    std::vector<std::unique_ptr<Expression>> arguments_;
};

/// \brief Parenthesized expression (for grouping)
class ParenthesizedExpression : public Expression
{
public:
    /// \brief Construct a parenthesized expression
    /// \param expr The inner expression
    explicit ParenthesizedExpression(std::unique_ptr<Expression> expr);

    Type        GetResultType() const override;
    std::string ToString() const override;
    void        Dump(int indent = 0) const override;

    /// \brief Get the inner expression
    /// \return Pointer to the inner expression
    const Expression* GetExpression() const;

private:
    std::unique_ptr<Expression> expr_;
};

// ============================================================================
// Invariant Declaration
// ============================================================================

/// \brief Represents an invariant constraint in a fabric type
class Invariant : public ASTNode
{
public:
    /// \brief Construct an invariant
    /// \param name The invariant's name
    /// \param expression The boolean expression for the invariant
    Invariant(const std::string& name, std::unique_ptr<Expression> expression);

    /// \brief Get the invariant's name
    /// \return The invariant name
    const std::string& GetName() const;

    /// \brief Get the invariant's expression
    /// \return The boolean expression
    const Expression* GetExpression() const;

    void Dump(int indent = 0) const override;

private:
    std::string                 name_;
    std::unique_ptr<Expression> expression_;
};

// ============================================================================
// Field Declaration
// ============================================================================

/// \brief Represents a field in a fabric type
class Field : public ASTNode
{
public:
    /// \brief Construct a field
    /// \param type The field's type specification
    /// \param name The field's name
    /// \param modifiers Vector of field modifiers
    /// \param isStatic Whether the field is static
    /// \param initializer Optional initializer expression for computed features
    Field(
        std::unique_ptr<TypeSpec> type, const std::string& name, std::vector<std::unique_ptr<Modifier>> modifiers, const bool isStatic = false,
        std::unique_ptr<Expression> initializer = nullptr) :
        type_(std::move(type)), name_(name), modifiers_(std::move(modifiers)), isStatic_(isStatic), initializer_(std::move(initializer))
    {
    }

    /// \brief Get the field's type specification
    /// \return Pointer to the type specification
    const TypeSpec* GetType() const;

    /// \brief Get the field's name
    /// \return The field name
    const std::string& GetName() const;

    /// \brief Get the field's modifiers
    /// \return Vector of modifiers
    const std::vector<std::unique_ptr<Modifier>>& GetModifiers() const;

    /// \brief Check if field is static
    /// \return True if field is static
    bool IsStatic() const;

    /// \brief Check if field is computed (has an initializer expression)
    /// \return True if field has an initializer
    bool IsComputed() const;

    /// \brief Get the initializer expression
    /// \return Pointer to the initializer expression or nullptr
    const Expression* GetInitializer() const;

    /// \brief Get the cardinality modifier if present
    /// \return Pointer to cardinality modifier or nullptr
    const CardinalityModifier* GetCardinalityModifier() const;

    /// \brief Check if field has unique constraint
    /// \return True if field has unique modifier
    bool HasUniqueConstraint() const;

    void Dump(int indent = 0) const override;

private:
    std::unique_ptr<TypeSpec>              type_;
    std::string                            name_;
    std::vector<std::unique_ptr<Modifier>> modifiers_;
    bool                                   isStatic_;
    std::unique_ptr<Expression>            initializer_;
};

// ============================================================================
// Enum Declaration
// ============================================================================

/// \brief Represents an enum declaration
class EnumDeclaration : public ASTNode
{
public:
    /// \brief Construct an enum declaration
    /// \param name The enum name
    /// \param values Vector of enum value names
    EnumDeclaration(const std::string& name, std::vector<std::string> values) : name_(name), values_(std::move(values)) {}

    /// \brief Get the enum name
    /// \return The enum name
    const std::string& GetName() const;

    /// \brief Get the enum values
    /// \return Vector of enum value names
    const std::vector<std::string>& GetValues() const;

    void Dump(int indent = 0) const override;

private:
    std::string              name_;
    std::vector<std::string> values_;
};

// ============================================================================
// Class Declaration
// ============================================================================

/// \brief Represents a class (user-defined type) declaration
class ClassDeclaration : public ASTNode
{
public:
    /// \brief Construct a class declaration
    /// \param name The class type name
    /// \param baseType The base type name (empty if no explicit base)
    /// \param fields Vector of field declarations
    /// \param invariants Vector of invariant declarations
    ClassDeclaration(
        const std::string& name, const std::string& baseType, std::vector<std::unique_ptr<Field>> fields,
        std::vector<std::unique_ptr<Invariant>> invariants = {}) :
        name_(name), baseType_(baseType), fields_(std::move(fields)), invariants_(std::move(invariants))
    {
    }

    /// \brief Get the class type name
    /// \return The class type name
    const std::string& GetName() const;

    /// \brief Get the base type name
    /// \return The base type name (empty if no explicit base)
    const std::string& GetBaseType() const;

    /// \brief Check if class has explicit base type
    /// \return True if class explicitly inherits from another type
    bool HasExplicitBase() const;

    /// \brief Get the class's fields
    /// \return Vector of field declarations
    const std::vector<std::unique_ptr<Field>>& GetFields() const;

    /// \brief Get the class's invariants
    /// \return Vector of invariant declarations
    const std::vector<std::unique_ptr<Invariant>>& GetInvariants() const;

    void Dump(int indent = 0) const override;

private:
    std::string                             name_;
    std::string                             baseType_; // Empty string if no explicit base
    std::vector<std::unique_ptr<Field>>     fields_;
    std::vector<std::unique_ptr<Invariant>> invariants_;
};

// ============================================================================
// Declaration (Union of Enum and Class)
// ============================================================================

/// \brief Represents a top-level declaration (enum or class)
class Declaration : public ASTNode
{
public:
    /// \brief Kind of declaration
    enum class Kind
    {
        ENUM,
        CLASS
    };

    /// \brief Construct a declaration from an enum
    /// \param enumDecl Unique pointer to enum declaration
    Declaration(std::unique_ptr<EnumDeclaration> enumDecl) : kind_(Kind::ENUM), declaration_(std::move(enumDecl)) {}

    /// \brief Construct a declaration from a class
    /// \param classDecl Unique pointer to class declaration
    Declaration(std::unique_ptr<ClassDeclaration> classDecl) : kind_(Kind::CLASS), declaration_(std::move(classDecl)) {}

    /// \brief Get the kind of declaration
    /// \return The declaration kind (ENUM or CLASS)
    Kind GetKind() const;

    /// \brief Cast to enum declaration if applicable
    /// \return Pointer to enum declaration or nullptr
    const EnumDeclaration* AsEnum() const;

    /// \brief Cast to class declaration if applicable
    /// \return Pointer to class declaration or nullptr
    const ClassDeclaration* AsClass() const;

    void Dump(int indent = 0) const override;

private:
    Kind                     kind_;
    std::unique_ptr<ASTNode> declaration_;
};

// ============================================================================
// AST (Root Node)
// ============================================================================

/// \brief Represents a complete P3 program (root AST node)
class AST : public ASTNode
{
public:
    /// \brief Construct an AST from declarations
    /// \param declarations Vector of top-level declarations
    explicit AST(std::vector<std::unique_ptr<Declaration>> declarations) : declarations_(std::move(declarations)) {}

    /// \brief Get all declarations in the AST
    /// \return Vector of declarations
    const std::vector<std::unique_ptr<Declaration>>& GetDeclarations() const;

    void Dump(int indent = 0) const override;

private:
    std::vector<std::unique_ptr<Declaration>> declarations_;
};
} // namespace bbfm

// Restore previous alignment
#pragma pack(pop)

#endif // __BBFM_AST_H_INCL__
