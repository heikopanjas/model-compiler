#include "AST.h"
#include "Common.h"
#include <iostream>
#include <string>

namespace bbfm {
// ============================================================================
// Helper Functions
// ============================================================================

void ASTNode::PrintIndent(const int indent) const
{
    for (int i = 0; i < indent; ++i)
    {
        std::cout << "  ";
    }
}

// ============================================================================
// PrimitiveTypeSpec Implementation
// ============================================================================

PrimitiveType PrimitiveTypeSpec::GetType() const
{
    return type_;
}

bool PrimitiveTypeSpec::IsPrimitive() const
{
    return true;
}

bool PrimitiveTypeSpec::IsUserDefined() const
{
    return false;
}

const char* PrimitiveTypeSpec::TypeToString(const PrimitiveType type)
{
    switch (type)
    {
        case PrimitiveType::STRING:
            return "String";
        case PrimitiveType::INT:
            return "Int";
        case PrimitiveType::REAL:
            return "Real";
        case PrimitiveType::BOOL:
            return "Bool";
        case PrimitiveType::TIMESTAMP:
            return "Timestamp";
        case PrimitiveType::TIMESPAN:
            return "Timespan";
        case PrimitiveType::DATE:
            return "Date";
        case PrimitiveType::GUID:
            return "Guid";
        default:
            return "Unknown";
    }
}

void PrimitiveTypeSpec::Dump(const int indent) const
{
    UNREFERENCED_PARAMETER(indent);
    std::cout << TypeToString(type_);
}

// ============================================================================
// UserDefinedTypeSpec Implementation
// ============================================================================

const std::string& UserDefinedTypeSpec::GetTypeName() const
{
    return typeName_;
}

bool UserDefinedTypeSpec::IsPrimitive() const
{
    return false;
}

bool UserDefinedTypeSpec::IsUserDefined() const
{
    return true;
}

void UserDefinedTypeSpec::Dump(const int indent) const
{
    UNREFERENCED_PARAMETER(indent);
    std::cout << typeName_;
}

// ============================================================================
// Modifier Implementation
// ============================================================================

ModifierType Modifier::GetType() const
{
    return type_;
}

// ============================================================================
// CardinalityModifier Implementation
// ============================================================================

int CardinalityModifier::GetMin() const
{
    return minCardinality_;
}

int CardinalityModifier::GetMax() const
{
    return maxCardinality_;
}

bool CardinalityModifier::IsUnbounded() const
{
    return -1 == maxCardinality_;
}

bool CardinalityModifier::IsOptional() const
{
    return 0 == minCardinality_;
}

bool CardinalityModifier::IsMandatory() const
{
    return minCardinality_ > 0;
}

bool CardinalityModifier::IsArray() const
{
    return -1 == maxCardinality_ || maxCardinality_ > 1;
}

void CardinalityModifier::Dump(const int indent) const
{
    UNREFERENCED_PARAMETER(indent);
    std::cout << "[" << minCardinality_;
    if (maxCardinality_ == -1)
    {
        std::cout << "..*";
    }
    else if (maxCardinality_ != minCardinality_)
    {
        std::cout << ".." << maxCardinality_;
    }
    std::cout << "]";
}

// ============================================================================
// UniqueModifier Implementation
// ============================================================================

void UniqueModifier::Dump(const int indent) const
{
    UNREFERENCED_PARAMETER(indent);
    std::cout << "[unique]";
}

// ============================================================================
// Field Implementation
// ============================================================================

const TypeSpec* Field::GetType() const
{
    return type_.get();
}

const std::string& Field::GetName() const
{
    return name_;
}

const std::vector<std::unique_ptr<Modifier>>& Field::GetModifiers() const
{
    return modifiers_;
}

bool Field::IsStatic() const
{
    return isStatic_;
}

const CardinalityModifier* Field::GetCardinalityModifier() const
{
    for (const auto& mod : modifiers_)
    {
        if (mod->GetType() == ModifierType::CARDINALITY)
        {
            return static_cast<const CardinalityModifier*>(mod.get());
        }
    }
    return nullptr;
}

bool Field::HasUniqueConstraint() const
{
    for (const auto& mod : modifiers_)
    {
        if (mod->GetType() == ModifierType::UNIQUE)
        {
            return true;
        }
    }
    return false;
}

void Field::Dump(const int indent) const
{
    PrintIndent(indent);
    if (isStatic_)
    {
        std::cout << "static ";
    }
    std::cout << "feature " << name_ << ": ";
    type_->Dump(0);

    // Print modifiers
    for (const auto& mod : modifiers_)
    {
        std::cout << " ";
        mod->Dump(0);
    }

    std::cout << ";\n";
}

// ============================================================================
// Invariant Implementation
// ============================================================================

const std::string& Invariant::GetName() const
{
    return name_;
}

const std::string& Invariant::GetExpression() const
{
    return expression_;
}

void Invariant::Dump(const int indent) const
{
    PrintIndent(indent);
    std::cout << "invariant " << name_ << ": " << expression_ << ";\n";
}

// ============================================================================
// EnumDeclaration Implementation
// ============================================================================

const std::string& EnumDeclaration::GetName() const
{
    return name_;
}

const std::vector<std::string>& EnumDeclaration::GetValues() const
{
    return values_;
}

void EnumDeclaration::Dump(const int indent) const
{
    PrintIndent(indent);
    std::cout << "enum " << name_ << " {\n";

    for (size_t i = 0; i < values_.size(); ++i)
    {
        PrintIndent(indent + 1);
        std::cout << values_[i];
        if (i < values_.size() - 1)
        {
            std::cout << ",";
        }
        std::cout << "\n";
    }

    PrintIndent(indent);
    std::cout << "}\n";
}

// ============================================================================
// ClassDeclaration Implementation
// ============================================================================

const std::string& ClassDeclaration::GetName() const
{
    return name_;
}

const std::string& ClassDeclaration::GetBaseType() const
{
    return baseType_;
}

bool ClassDeclaration::HasExplicitBase() const
{
    return false == baseType_.empty();
}

const std::vector<std::unique_ptr<Field>>& ClassDeclaration::GetFields() const
{
    return fields_;
}

const std::vector<std::unique_ptr<Invariant>>& ClassDeclaration::GetInvariants() const
{
    return invariants_;
}

void ClassDeclaration::Dump(const int indent) const
{
    PrintIndent(indent);
    std::cout << "class " << name_;

    if (false == baseType_.empty())
    {
        std::cout << " inherits " << baseType_;
    }

    std::cout << " {\n";

    for (const auto& field : fields_)
    {
        field->Dump(indent + 1);
    }

    for (const auto& invariant : invariants_)
    {
        invariant->Dump(indent + 1);
    }

    PrintIndent(indent);
    std::cout << "}\n";
}

// ============================================================================
// Declaration Implementation
// ============================================================================

Declaration::Kind Declaration::GetKind() const
{
    return kind_;
}

const EnumDeclaration* Declaration::AsEnum() const
{
    return Kind::ENUM == kind_ ? static_cast<const EnumDeclaration*>(declaration_.get()) : nullptr;
}

const ClassDeclaration* Declaration::AsClass() const
{
    return Kind::CLASS == kind_ ? static_cast<const ClassDeclaration*>(declaration_.get()) : nullptr;
}

void Declaration::Dump(const int indent) const
{
    declaration_->Dump(indent);
}

// ============================================================================
// AST Implementation
// ============================================================================

const std::vector<std::unique_ptr<Declaration>>& AST::GetDeclarations() const
{
    return declarations_;
}

void AST::Dump(const int indent) const
{
    PrintIndent(indent);
    std::cout << "=== BBFM Program AST ===\n\n";

    for (const auto& decl : declarations_)
    {
        decl->Dump(indent);
        std::cout << "\n";
    }

    PrintIndent(indent);
    std::cout << "=== End of AST ===\n";
}
} // namespace bbfm
