#ifndef AST_H
#define AST_H

#include <memory>
#include <string>
#include <vector>

namespace p3 {
// Forward declarations
class ASTNode;
class Program;
class Declaration;
class EnumDeclaration;
class FabricDeclaration;
class Field;
class TypeSpec;
class Modifier;

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

/// \brief Enumeration of primitive types in P3 language
enum class PrimitiveType
{
    STRING,
    INT,
    REAL,
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
private:
    PrimitiveType type_;

public:
    /// \brief Construct a primitive type specification
    /// \param type The primitive type
    explicit PrimitiveTypeSpec(const PrimitiveType type) : type_(type) {}

    /// \brief Get the primitive type
    /// \return The primitive type
    PrimitiveType GetType() const
    {
        return type_;
    }

    bool IsPrimitive() const override
    {
        return true;
    }

    bool IsUserDefined() const override
    {
        return false;
    }

    void Dump(int indent = 0) const override;

    /// \brief Convert primitive type to string representation
    /// \param type The primitive type to convert
    /// \return String representation of the type
    static const char* TypeToString(const PrimitiveType type);
};

/// \brief Type specification for user-defined types
class UserDefinedTypeSpec : public TypeSpec
{
private:
    std::string typeName_;

public:
    /// \brief Construct a user-defined type specification
    /// \param name The name of the user-defined type
    explicit UserDefinedTypeSpec(const std::string& name) : typeName_(name) {}

    /// \brief Get the type name
    /// \return The name of the user-defined type
    const std::string& GetTypeName() const
    {
        return typeName_;
    }

    bool IsPrimitive() const override
    {
        return false;
    }

    bool IsUserDefined() const override
    {
        return true;
    }

    void Dump(int indent = 0) const override;
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
private:
    ModifierType type_;

public:
    /// \brief Construct a modifier
    /// \param type The type of modifier
    explicit Modifier(const ModifierType type) : type_(type) {}

    virtual ~Modifier() = default;

    /// \brief Get the modifier type
    /// \return The modifier type
    ModifierType GetType() const
    {
        return type_;
    }
};

/// \brief Cardinality modifier for fields (e.g., [1], [0..1], [0..*])
class CardinalityModifier : public Modifier
{
private:
    int minCardinality_;
    int maxCardinality_; // -1 for unbounded (*)

public:
    /// \brief Construct a cardinality modifier
    /// \param min Minimum cardinality
    /// \param max Maximum cardinality (-1 for unbounded)
    CardinalityModifier(const int min, const int max) : Modifier(ModifierType::CARDINALITY), minCardinality_(min), maxCardinality_(max) {}

    /// \brief Get minimum cardinality
    /// \return Minimum cardinality value
    int GetMin() const
    {
        return minCardinality_;
    }

    /// \brief Get maximum cardinality
    /// \return Maximum cardinality value (-1 for unbounded)
    int GetMax() const
    {
        return maxCardinality_;
    }

    /// \brief Check if cardinality is unbounded
    /// \return True if unbounded (max is *)
    bool IsUnbounded() const
    {
        return maxCardinality_ == -1;
    }

    /// \brief Check if field is optional
    /// \return True if minimum cardinality is 0
    bool IsOptional() const
    {
        return minCardinality_ == 0;
    }

    /// \brief Check if field is mandatory
    /// \return True if minimum cardinality is greater than 0
    bool IsMandatory() const
    {
        return minCardinality_ > 0;
    }

    /// \brief Check if field is an array
    /// \return True if maximum cardinality is unbounded or greater than 1
    bool IsArray() const
    {
        return maxCardinality_ == -1 || maxCardinality_ > 1;
    }

    void Dump(int indent = 0) const override;
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
// Field Declaration
// ============================================================================

/// \brief Represents a field in a fabric type
class Field : public ASTNode
{
private:
    std::unique_ptr<TypeSpec>              type_;
    std::string                            name_;
    std::vector<std::unique_ptr<Modifier>> modifiers_;
    bool                                   isStatic_;

public:
    /// \brief Construct a field
    /// \param type The field's type specification
    /// \param name The field's name
    /// \param modifiers Vector of field modifiers
    /// \param isStatic Whether the field is static
    Field(std::unique_ptr<TypeSpec> type, const std::string& name, std::vector<std::unique_ptr<Modifier>> modifiers, const bool isStatic = false) :
        type_(std::move(type)), name_(name), modifiers_(std::move(modifiers)), isStatic_(isStatic)
    {
    }

    /// \brief Get the field's type specification
    /// \return Pointer to the type specification
    const TypeSpec* GetType() const
    {
        return type_.get();
    }

    /// \brief Get the field's name
    /// \return The field name
    const std::string& GetName() const
    {
        return name_;
    }

    /// \brief Get the field's modifiers
    /// \return Vector of modifiers
    const std::vector<std::unique_ptr<Modifier>>& GetModifiers() const
    {
        return modifiers_;
    }

    /// \brief Check if field is static
    /// \return True if field is static
    bool IsStatic() const
    {
        return isStatic_;
    }

    /// \brief Get the cardinality modifier if present
    /// \return Pointer to cardinality modifier or nullptr
    const CardinalityModifier* GetCardinalityModifier() const;

    /// \brief Check if field has unique constraint
    /// \return True if field has unique modifier
    bool HasUniqueConstraint() const;

    void Dump(int indent = 0) const override;
};

// ============================================================================
// Enum Declaration
// ============================================================================

/// \brief Represents an enum declaration
class EnumDeclaration : public ASTNode
{
private:
    std::string              name_;
    std::vector<std::string> values_;

public:
    /// \brief Construct an enum declaration
    /// \param name The enum name
    /// \param values Vector of enum value names
    EnumDeclaration(const std::string& name, std::vector<std::string> values) : name_(name), values_(std::move(values)) {}

    /// \brief Get the enum name
    /// \return The enum name
    const std::string& GetName() const
    {
        return name_;
    }

    /// \brief Get the enum values
    /// \return Vector of enum value names
    const std::vector<std::string>& GetValues() const
    {
        return values_;
    }

    void Dump(int indent = 0) const override;
};

// ============================================================================
// Fabric (Type) Declaration
// ============================================================================

/// \brief Represents a fabric (user-defined type) declaration
class FabricDeclaration : public ASTNode
{
private:
    std::string                         name_;
    std::string                         baseType_; // Empty string if no explicit base (inherits from implicit Fabric)
    std::vector<std::unique_ptr<Field>> fields_;

public:
    /// \brief Construct a fabric declaration
    /// \param name The fabric type name
    /// \param baseType The base type name (empty if inherits from implicit Fabric)
    /// \param fields Vector of field declarations
    FabricDeclaration(const std::string& name, const std::string& baseType, std::vector<std::unique_ptr<Field>> fields) :
        name_(name), baseType_(baseType), fields_(std::move(fields))
    {
    }

    /// \brief Get the fabric type name
    /// \return The fabric type name
    const std::string& GetName() const
    {
        return name_;
    }

    /// \brief Get the base type name
    /// \return The base type name (empty if implicit Fabric)
    const std::string& GetBaseType() const
    {
        return baseType_;
    }

    /// \brief Check if fabric has explicit base type
    /// \return True if fabric explicitly inherits from another type
    bool HasExplicitBase() const
    {
        return !baseType_.empty();
    }

    /// \brief Get the fabric's fields
    /// \return Vector of field declarations
    const std::vector<std::unique_ptr<Field>>& GetFields() const
    {
        return fields_;
    }

    void Dump(int indent = 0) const override;
};

// ============================================================================
// Declaration (Union of Enum and Fabric)
// ============================================================================

/// \brief Represents a top-level declaration (enum or fabric)
class Declaration : public ASTNode
{
public:
    /// \brief Kind of declaration
    enum class Kind
    {
        ENUM,
        FABRIC
    };

private:
    Kind                     kind_;
    std::unique_ptr<ASTNode> declaration_;

public:
    /// \brief Construct a declaration from an enum
    /// \param enumDecl Unique pointer to enum declaration
    Declaration(std::unique_ptr<EnumDeclaration> enumDecl) : kind_(Kind::ENUM), declaration_(std::move(enumDecl)) {}

    /// \brief Construct a declaration from a fabric
    /// \param fabricDecl Unique pointer to fabric declaration
    Declaration(std::unique_ptr<FabricDeclaration> fabricDecl) : kind_(Kind::FABRIC), declaration_(std::move(fabricDecl)) {}

    /// \brief Get the kind of declaration
    /// \return The declaration kind (ENUM or FABRIC)
    Kind GetKind() const
    {
        return kind_;
    }

    /// \brief Cast to enum declaration if applicable
    /// \return Pointer to enum declaration or nullptr
    const EnumDeclaration* AsEnum() const
    {
        return kind_ == Kind::ENUM ? static_cast<const EnumDeclaration*>(declaration_.get()) : nullptr;
    }

    /// \brief Cast to fabric declaration if applicable
    /// \return Pointer to fabric declaration or nullptr
    const FabricDeclaration* AsFabric() const
    {
        return kind_ == Kind::FABRIC ? static_cast<const FabricDeclaration*>(declaration_.get()) : nullptr;
    }

    void Dump(int indent = 0) const override;
};

// ============================================================================
// Program (Root Node)
// ============================================================================

/// \brief Represents a complete P3 program (root AST node)
class Program : public ASTNode
{
private:
    std::vector<std::unique_ptr<Declaration>> declarations_;

public:
    /// \brief Construct a program from declarations
    /// \param declarations Vector of top-level declarations
    explicit Program(std::vector<std::unique_ptr<Declaration>> declarations) : declarations_(std::move(declarations)) {}

    /// \brief Get all declarations in the program
    /// \return Vector of declarations
    const std::vector<std::unique_ptr<Declaration>>& GetDeclarations() const
    {
        return declarations_;
    }

    void Dump(int indent = 0) const override;
};
} // namespace p3

#endif // AST_H
