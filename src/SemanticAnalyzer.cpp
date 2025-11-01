#include "SemanticAnalyzer.h"
#include "Common.h"
#include "Console.h"
#include <iostream>

namespace bbfm {
SemanticAnalyzer::SemanticAnalyzer(const AST* ast) : ast_(ast), hasErrors_(false) {}

bool SemanticAnalyzer::Analyze()
{
    // Register built-in primitive types
    RegisterPrimitiveTypes();

    // Build symbol table from declarations
    if (!BuildSymbolTable())
    {
        return false;
    }

    // Validate type references
    if (!ValidateTypeReferences())
    {
        return false;
    }

    return !hasErrors_;
}

void SemanticAnalyzer::RegisterPrimitiveTypes()
{
    symbolTable_.insert({"String", TypeSymbol("String")});
    symbolTable_.insert({"Int", TypeSymbol("Int")});
    symbolTable_.insert({"Real", TypeSymbol("Real")});
    symbolTable_.insert({"Bool", TypeSymbol("Bool")});
    symbolTable_.insert({"Timestamp", TypeSymbol("Timestamp")});
    symbolTable_.insert({"Timespan", TypeSymbol("Timespan")});
    symbolTable_.insert({"Date", TypeSymbol("Date")});
    symbolTable_.insert({"Guid", TypeSymbol("Guid")});
}

bool SemanticAnalyzer::BuildSymbolTable()
{
    bool success = true;

    for (const auto& decl : ast_->GetDeclarations())
    {
        if (Declaration::Kind::ENUM == decl->GetKind())
        {
            const EnumDeclaration* enumDecl = decl->AsEnum();
            const std::string&     name     = enumDecl->GetName();

            // Check for duplicate type names
            if (TypeExists(name))
            {
                ReportError("Type '" + name + "' is already declared");
                success = false;
                continue;
            }

            // Add to symbol table
            symbolTable_.insert({name, TypeSymbol(enumDecl)});
        }
        else if (Declaration::Kind::FABRIC == decl->GetKind())
        {
            const FabricDeclaration* fabricDecl = decl->AsFabric();
            const std::string&       name       = fabricDecl->GetName();

            // Check for duplicate type names
            if (TypeExists(name))
            {
                ReportError("Type '" + name + "' is already declared");
                success = false;
                continue;
            }

            // Add to symbol table
            symbolTable_.insert({name, TypeSymbol(fabricDecl)});
        }
    }

    return success;
}

bool SemanticAnalyzer::ValidateTypeReferences()
{
    bool success = true;

    // First pass: Validate type references
    for (const auto& decl : ast_->GetDeclarations())
    {
        if (Declaration::Kind::FABRIC == decl->GetKind())
        {
            const FabricDeclaration* fabricDecl = decl->AsFabric();

            if (!ValidateFabricDeclaration(fabricDecl))
            {
                success = false;
            }
        }
    }

    // Second pass: Check for inheritance cycles
    // This must be done after all types are validated to handle forward references
    for (const auto& decl : ast_->GetDeclarations())
    {
        if (Declaration::Kind::FABRIC == decl->GetKind())
        {
            const FabricDeclaration* fabricDecl = decl->AsFabric();

            if (fabricDecl->HasExplicitBase())
            {
                std::set<std::string> visited;
                visited.insert(fabricDecl->GetName());
                if (HasInheritanceCycle(fabricDecl->GetBaseType(), visited))
                {
                    ReportError("Circular inheritance detected in class '" + fabricDecl->GetName() + "'");
                    success = false;
                }
            }
        }
    }

    return success;
}

bool SemanticAnalyzer::ValidateFabricDeclaration(const FabricDeclaration* fabric)
{
    bool success = true;

    // Validate base type if specified
    if (fabric->HasExplicitBase())
    {
        const std::string& baseType = fabric->GetBaseType();

        if (!TypeExists(baseType))
        {
            ReportError("Class '" + fabric->GetName() + "' inherits from undefined type '" + baseType + "'");
            success = false;
        }
        else
        {
            const TypeSymbol* baseSym = LookupType(baseType);
            if (TypeSymbol::Kind::CLASS != baseSym->kind)
            {
                ReportError("Class '" + fabric->GetName() + "' cannot inherit from non-class type '" + baseType + "'");
                success = false;
            }
            // Note: Inheritance cycle detection happens in a second pass
            // after all types are validated, to avoid issues with forward references
        }
    }

    // Validate field types
    for (const auto& field : fabric->GetFields())
    {
        const TypeSpec* typeSpec = field->GetType();

        if (typeSpec->IsUserDefined())
        {
            const UserDefinedTypeSpec* userType = static_cast<const UserDefinedTypeSpec*>(typeSpec);
            const std::string&         typeName = userType->GetTypeName();

            if (!TypeExists(typeName))
            {
                ReportError("Field '" + field->GetName() + "' in class '" + fabric->GetName() + "' has undefined type '" + typeName + "'");
                success = false;
            }
        }
    }

    // Validate field uniqueness
    if (!ValidateFieldUniqueness(fabric))
    {
        success = false;
    }

    // Validate invariants
    if (!ValidateInvariants(fabric))
    {
        success = false;
    }

    return success;
}

bool SemanticAnalyzer::HasInheritanceCycle(const std::string& className, std::set<std::string>& visited)
{
    // If we've visited this class before, we have a cycle
    if (0 != visited.count(className))
    {
        return true;
    }

    const TypeSymbol* typeSym = LookupType(className);
    if (nullptr == typeSym || TypeSymbol::Kind::CLASS != typeSym->kind)
    {
        return false;
    }

    if (nullptr == typeSym->fabricDecl)
    {
        // This shouldn't happen if symbol table is built correctly
        return false;
    }

    const FabricDeclaration* fabric = typeSym->fabricDecl;
    if (!fabric->HasExplicitBase())
    {
        return false;
    }

    // Add current class to visited set before recursing
    visited.insert(className);

    // Recursively check base type
    return HasInheritanceCycle(fabric->GetBaseType(), visited);
}

void SemanticAnalyzer::GetAllFields(const FabricDeclaration* fabric, std::vector<const Field*>& allFields)
{
    // Use a set to track visited classes and prevent infinite recursion on cycles
    std::set<std::string> visited;
    GetAllFieldsHelper(fabric, allFields, visited);
}

void SemanticAnalyzer::GetAllFieldsHelper(const FabricDeclaration* fabric, std::vector<const Field*>& allFields, std::set<std::string>& visited)
{
    if (nullptr == fabric)
    {
        return;
    }

    // If we've already visited this class, stop (cycle detected)
    if (0 != visited.count(fabric->GetName()))
    {
        return;
    }

    visited.insert(fabric->GetName());

    // First, get fields from base class if any
    if (fabric->HasExplicitBase())
    {
        const TypeSymbol* baseSym = LookupType(fabric->GetBaseType());
        if (nullptr != baseSym && TypeSymbol::Kind::CLASS == baseSym->kind)
        {
            GetAllFieldsHelper(baseSym->fabricDecl, allFields, visited);
        }
    }

    // Then add this class's fields
    for (const auto& field : fabric->GetFields())
    {
        allFields.push_back(field.get());
    }
}

bool SemanticAnalyzer::ValidateFieldUniqueness(const FabricDeclaration* fabric)
{
    std::vector<const Field*> allFields;
    GetAllFields(fabric, allFields);

    std::set<std::string> fieldNames;
    bool                  success = true;

    for (const Field* field : allFields)
    {
        const std::string& name = field->GetName();
        if (0 != fieldNames.count(name))
        {
            ReportError("Duplicate field '" + name + "' in class '" + fabric->GetName() + "' (possibly inherited)");
            success = false;
        }
        fieldNames.insert(name);
    }

    return success;
}

bool SemanticAnalyzer::ValidateInvariants(const FabricDeclaration* fabric)
{
    bool success = true;

    // Get all fields (including inherited) for validation
    std::vector<const Field*> allFields;
    GetAllFields(fabric, allFields);

    std::set<std::string> fieldNames;
    for (const Field* field : allFields)
    {
        fieldNames.insert(field->GetName());
    }

    // Validate each invariant
    for (const auto& invariant : fabric->GetInvariants())
    {
        const std::string& expr = invariant->GetExpression();

        // Parse the expression to extract the field name
        // Expression format: "fieldName operator literal"
        // Find the operator position
        size_t opPos = expr.find("<=");
        if (std::string::npos == opPos)
        {
            opPos = expr.find(">=");
        }
        if (std::string::npos == opPos)
        {
            opPos = expr.find("==");
        }
        if (std::string::npos == opPos)
        {
            opPos = expr.find("!=");
        }
        if (std::string::npos == opPos)
        {
            opPos = expr.find('<');
        }
        if (std::string::npos == opPos)
        {
            opPos = expr.find('>');
        }

        if (std::string::npos == opPos)
        {
            ReportError("Invalid invariant expression '" + expr + "' in class '" + fabric->GetName() + "'");
            success = false;
            continue;
        }

        // Extract field name (trim whitespace)
        std::string fieldName = expr.substr(0, opPos);
        // Trim leading/trailing whitespace
        size_t      start     = fieldName.find_first_not_of(" \t");
        size_t      end       = fieldName.find_last_not_of(" \t");
        if (std::string::npos != start && std::string::npos != end)
        {
            fieldName = fieldName.substr(start, end - start + 1);
        }

        // Check if field exists
        if (0 == fieldNames.count(fieldName))
        {
            ReportError("Invariant '" + invariant->GetName() + "' in class '" + fabric->GetName() + "' references undefined field '" + fieldName + "'");
            success = false;
        }
    }

    return success;
}

bool SemanticAnalyzer::TypeExists(const std::string& typeName) const
{
    return 0 != symbolTable_.count(typeName);
}

const TypeSymbol* SemanticAnalyzer::LookupType(const std::string& typeName) const
{
    auto it = symbolTable_.find(typeName);
    if (symbolTable_.end() == it)
    {
        return nullptr;
    }
    return &it->second;
}

void SemanticAnalyzer::ReportError(const std::string& message)
{
    Console::ReportError("Semantic error: " + message);
    hasErrors_ = true;
}

bool SemanticAnalyzer::HasErrors() const
{
    return hasErrors_;
}

const std::map<std::string, TypeSymbol>& SemanticAnalyzer::GetSymbolTable() const
{
    return symbolTable_;
}

void SemanticAnalyzer::DumpSymbolTable() const
{
    std::cout << "========================================\n";
    std::cout << "Symbol Table\n";
    std::cout << "========================================\n\n";

    // Count types by kind
    int primitiveCount = 0;
    int enumCount      = 0;
    int classCount     = 0;

    for (const auto& entry : symbolTable_)
    {
        if (TypeSymbol::Kind::PRIMITIVE == entry.second.kind)
        {
            primitiveCount++;
        }
        else if (TypeSymbol::Kind::ENUM == entry.second.kind)
        {
            enumCount++;
        }
        else if (TypeSymbol::Kind::CLASS == entry.second.kind)
        {
            classCount++;
        }
    }

    std::cout << "Total Symbols: " << symbolTable_.size() << "\n";
    std::cout << "  Primitive Types: " << primitiveCount << "\n";
    std::cout << "  Enumerations: " << enumCount << "\n";
    std::cout << "  Classes: " << classCount << "\n";
    std::cout << "\n";

    // Dump primitive types
    if (primitiveCount > 0)
    {
        std::cout << "Primitive Types:\n";
        std::cout << "----------------\n";
        for (const auto& entry : symbolTable_)
        {
            if (TypeSymbol::Kind::PRIMITIVE == entry.second.kind)
            {
                std::cout << "  " << entry.second.name << "\n";
            }
        }
        std::cout << "\n";
    }

    // Dump enums
    if (enumCount > 0)
    {
        std::cout << "Enumerations:\n";
        std::cout << "-------------\n";
        for (const auto& entry : symbolTable_)
        {
            if (TypeSymbol::Kind::ENUM == entry.second.kind)
            {
                std::cout << "  enum " << entry.second.name << " {\n";
                const auto& values = entry.second.enumDecl->GetValues();
                for (size_t i = 0; i < values.size(); ++i)
                {
                    std::cout << "    " << values[i];
                    if (i < values.size() - 1)
                    {
                        std::cout << ",";
                    }
                    std::cout << "\n";
                }
                std::cout << "  }\n\n";
            }
        }
    }

    // Dump classes
    if (classCount > 0)
    {
        std::cout << "Classes:\n";
        std::cout << "--------\n";
        for (const auto& entry : symbolTable_)
        {
            if (TypeSymbol::Kind::CLASS == entry.second.kind)
            {
                std::cout << "  class " << entry.second.name;

                // Show inheritance
                const std::string& baseType = entry.second.fabricDecl->GetBaseType();
                if (false == baseType.empty())
                {
                    std::cout << " inherits " << baseType;
                }
                std::cout << " {\n";

                // Show fields
                const auto& fields = entry.second.fabricDecl->GetFields();
                if (false == fields.empty())
                {
                    std::cout << "    Features:\n";
                    for (const auto& field : fields)
                    {
                        std::cout << "      " << field->GetName() << ": ";

                        // Get type name based on TypeSpec type
                        const TypeSpec* typeSpec = field->GetType();
                        if (typeSpec->IsPrimitive())
                        {
                            const PrimitiveTypeSpec* primType = dynamic_cast<const PrimitiveTypeSpec*>(typeSpec);
                            std::cout << PrimitiveTypeSpec::TypeToString(primType->GetType());
                        }
                        else
                        {
                            const UserDefinedTypeSpec* userType = dynamic_cast<const UserDefinedTypeSpec*>(typeSpec);
                            std::cout << userType->GetTypeName();
                        }

                        // Show modifiers
                        const auto& modifiers = field->GetModifiers();
                        if (false == modifiers.empty())
                        {
                            std::cout << " [";
                            for (size_t i = 0; i < modifiers.size(); ++i)
                            {
                                if (const CardinalityModifier* cardMod = dynamic_cast<const CardinalityModifier*>(modifiers[i].get()))
                                {
                                    std::cout << cardMod->GetMin() << ".." << cardMod->GetMax();
                                }
                                else if (dynamic_cast<const UniqueModifier*>(modifiers[i].get()))
                                {
                                    std::cout << "unique";
                                }

                                if (i < modifiers.size() - 1)
                                {
                                    std::cout << ", ";
                                }
                            }
                            std::cout << "]";
                        }
                        std::cout << "\n";
                    }
                }

                // Show invariants
                const auto& invariants = entry.second.fabricDecl->GetInvariants();
                if (false == invariants.empty())
                {
                    std::cout << "    Invariants:\n";
                    for (const auto& invariant : invariants)
                    {
                        std::cout << "      " << invariant->GetName() << ": " << invariant->GetExpression() << "\n";
                    }
                }

                std::cout << "  }\n\n";
            }
        }
    }

    std::cout << "========================================\n";
}
} // namespace bbfm
