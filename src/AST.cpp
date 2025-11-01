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

void UserDefinedTypeSpec::Dump(const int indent) const
{
    UNREFERENCED_PARAMETER(indent);
    std::cout << typeName_;
}

// ============================================================================
// CardinalityModifier Implementation
// ============================================================================

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
    type_->Dump(0);
    std::cout << " " << name_;

    // Print modifiers
    for (const auto& mod : modifiers_)
    {
        std::cout << " ";
        mod->Dump(0);
    }

    std::cout << ";\n";
}

// ============================================================================
// EnumDeclaration Implementation
// ============================================================================

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
// FabricDeclaration Implementation
// ============================================================================

void FabricDeclaration::Dump(const int indent) const
{
    PrintIndent(indent);
    std::cout << "fabric " << name_;

    if (false == baseType_.empty())
    {
        std::cout << " : " << baseType_;
    }

    std::cout << " {\n";

    for (const auto& field : fields_)
    {
        field->Dump(indent + 1);
    }

    PrintIndent(indent);
    std::cout << "}\n";
}

// ============================================================================
// Declaration Implementation
// ============================================================================

void Declaration::Dump(const int indent) const
{
    declaration_->Dump(indent);
}

// ============================================================================
// AST Implementation
// ============================================================================

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
