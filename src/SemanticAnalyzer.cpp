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
        else if (Declaration::Kind::CLASS == decl->GetKind())
        {
            const ClassDeclaration* classDecl = decl->AsClass();
            const std::string&      name      = classDecl->GetName();

            // Check for duplicate type names
            if (TypeExists(name))
            {
                ReportError("Type '" + name + "' is already declared");
                success = false;
                continue;
            }

            // Add to symbol table
            symbolTable_.insert({name, TypeSymbol(classDecl)});
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
        if (Declaration::Kind::CLASS == decl->GetKind())
        {
            const ClassDeclaration* classDecl = decl->AsClass();

            if (!ValidateClassDeclaration(classDecl))
            {
                success = false;
            }
        }
    }

    // Second pass: Check for inheritance cycles
    // This must be done after all types are validated to handle forward references
    for (const auto& decl : ast_->GetDeclarations())
    {
        if (Declaration::Kind::CLASS == decl->GetKind())
        {
            const ClassDeclaration* classDecl = decl->AsClass();

            if (classDecl->HasExplicitBase())
            {
                std::set<std::string> visited;
                visited.insert(classDecl->GetName());
                if (HasInheritanceCycle(classDecl->GetBaseType(), visited))
                {
                    ReportError("Circular inheritance detected in class '" + classDecl->GetName() + "'");
                    success = false;
                }
            }
        }
    }

    return success;
}

bool SemanticAnalyzer::ValidateClassDeclaration(const ClassDeclaration* classDecl)
{
    bool success = true;

    // Validate base type if specified
    if (classDecl->HasExplicitBase())
    {
        const std::string& baseType = classDecl->GetBaseType();

        if (!TypeExists(baseType))
        {
            ReportError("Class '" + classDecl->GetName() + "' inherits from undefined type '" + baseType + "'");
            success = false;
        }
        else
        {
            const TypeSymbol* baseSym = LookupType(baseType);
            if (TypeSymbol::Kind::CLASS != baseSym->kind)
            {
                ReportError("Class '" + classDecl->GetName() + "' cannot inherit from non-class type '" + baseType + "'");
                success = false;
            }
            // Note: Inheritance cycle detection happens in a second pass
            // after all types are validated, to avoid issues with forward references
        }
    }

    // Validate field types
    for (const auto& field : classDecl->GetFields())
    {
        const TypeSpec* typeSpec = field->GetType();

        if (typeSpec->IsUserDefined())
        {
            const UserDefinedTypeSpec* userType = static_cast<const UserDefinedTypeSpec*>(typeSpec);
            const std::string&         typeName = userType->GetTypeName();

            if (!TypeExists(typeName))
            {
                ReportError("Field '" + field->GetName() + "' in class '" + classDecl->GetName() + "' has undefined type '" + typeName + "'");
                success = false;
            }
        }
    }

    // Validate field uniqueness
    if (!ValidateFieldUniqueness(classDecl))
    {
        success = false;
    }

    // Validate invariants
    if (!ValidateInvariants(classDecl))
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

    if (nullptr == typeSym->classDecl)
    {
        // This shouldn't happen if symbol table is built correctly
        return false;
    }

    const ClassDeclaration* classDecl = typeSym->classDecl;
    if (!classDecl->HasExplicitBase())
    {
        return false;
    }

    // Add current class to visited set before recursing
    visited.insert(className);

    // Recursively check base type
    return HasInheritanceCycle(classDecl->GetBaseType(), visited);
}

void SemanticAnalyzer::GetAllFields(const ClassDeclaration* classDecl, std::vector<const Field*>& allFields) const
{
    // Use a set to track visited classes and prevent infinite recursion on cycles
    std::set<std::string> visited;
    GetAllFieldsHelper(classDecl, allFields, visited);
}

void SemanticAnalyzer::GetAllFieldsHelper(const ClassDeclaration* classDecl, std::vector<const Field*>& allFields, std::set<std::string>& visited) const
{
    if (nullptr == classDecl)
    {
        return;
    }

    // If we've already visited this class, stop (cycle detected)
    if (0 != visited.count(classDecl->GetName()))
    {
        return;
    }

    visited.insert(classDecl->GetName());

    // First, get fields from base class if any
    if (classDecl->HasExplicitBase())
    {
        const TypeSymbol* baseSym = LookupType(classDecl->GetBaseType());
        if (nullptr != baseSym && TypeSymbol::Kind::CLASS == baseSym->kind)
        {
            GetAllFieldsHelper(baseSym->classDecl, allFields, visited);
        }
    }

    // Then add this class's fields
    for (const auto& field : classDecl->GetFields())
    {
        allFields.push_back(field.get());
    }
}

void SemanticAnalyzer::GetAllInvariants(const ClassDeclaration* classDecl, std::vector<const Invariant*>& allInvariants) const
{
    // Use a set to track visited classes and prevent infinite recursion on cycles
    std::set<std::string> visited;
    GetAllInvariantsHelper(classDecl, allInvariants, visited);
}

void SemanticAnalyzer::GetAllInvariantsHelper(
    const ClassDeclaration* classDecl, std::vector<const Invariant*>& allInvariants, std::set<std::string>& visited) const
{
    if (nullptr == classDecl)
    {
        return;
    }

    // If we've already visited this class, stop (cycle detected)
    if (0 != visited.count(classDecl->GetName()))
    {
        return;
    }

    visited.insert(classDecl->GetName());

    // First, get invariants from base class if any
    if (classDecl->HasExplicitBase())
    {
        const TypeSymbol* baseSym = LookupType(classDecl->GetBaseType());
        if (nullptr != baseSym && TypeSymbol::Kind::CLASS == baseSym->kind)
        {
            GetAllInvariantsHelper(baseSym->classDecl, allInvariants, visited);
        }
    }

    // Then add this class's invariants
    for (const auto& invariant : classDecl->GetInvariants())
    {
        allInvariants.push_back(invariant.get());
    }
}

bool SemanticAnalyzer::ValidateFieldUniqueness(const ClassDeclaration* classDecl)
{
    std::vector<const Field*> allFields;
    GetAllFields(classDecl, allFields);

    std::set<std::string> fieldNames;
    bool                  success = true;

    for (const Field* field : allFields)
    {
        const std::string& name = field->GetName();
        if (0 != fieldNames.count(name))
        {
            ReportError("Duplicate field '" + name + "' in class '" + classDecl->GetName() + "' (possibly inherited)");
            success = false;
        }
        fieldNames.insert(name);
    }

    return success;
}

bool SemanticAnalyzer::ValidateInvariants(const ClassDeclaration* classDecl)
{
    bool success = true;

    // Get all fields (including inherited) for validation
    std::vector<const Field*> allFields;
    GetAllFields(classDecl, allFields);

    std::set<std::string> fieldNames;
    for (const Field* field : allFields)
    {
        fieldNames.insert(field->GetName());
    }

    // Validate each invariant
    for (const auto& invariant : classDecl->GetInvariants())
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
            ReportError("Invalid invariant expression '" + expr + "' in class '" + classDecl->GetName() + "'");
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
            ReportError("Invariant '" + invariant->GetName() + "' in class '" + classDecl->GetName() + "' references undefined field '" + fieldName + "'");
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
                const std::string& baseType = entry.second.classDecl->GetBaseType();
                if (false == baseType.empty())
                {
                    std::cout << " inherits " << baseType;
                }
                std::cout << " {\n";

                // Show fields (including inherited)
                std::vector<const Field*> allFields;
                GetAllFields(entry.second.classDecl, allFields);

                // Also get just the local fields for comparison
                const auto&            localFields = entry.second.classDecl->GetFields();
                std::set<const Field*> localFieldSet;
                for (const auto& field : localFields)
                {
                    localFieldSet.insert(field.get());
                }

                if (false == allFields.empty())
                {
                    std::cout << "    Features:\n";
                    for (const auto* field : allFields)
                    {
                        // Determine if this is a local or inherited field
                        bool isLocal = (0 != localFieldSet.count(field));
                        std::cout << "      " << (isLocal ? "<self>" : "<base>") << " ";
                        std::cout << field->GetName() << ": ";

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

                // Show invariants (including inherited)
                std::vector<const Invariant*> allInvariants;
                GetAllInvariants(entry.second.classDecl, allInvariants);

                // Also get just the local invariants for comparison
                const auto&                localInvariants = entry.second.classDecl->GetInvariants();
                std::set<const Invariant*> localInvariantSet;
                for (const auto& invariant : localInvariants)
                {
                    localInvariantSet.insert(invariant.get());
                }

                if (false == allInvariants.empty())
                {
                    std::cout << "    Invariants:\n";
                    for (const auto* invariant : allInvariants)
                    {
                        // Determine if this is a local or inherited invariant
                        bool isLocal = (0 != localInvariantSet.count(invariant));
                        std::cout << "      " << (isLocal ? "<self>" : "<base>") << " ";
                        std::cout << invariant->GetName() << ": " << invariant->GetExpression() << "\n";
                    }
                }

                std::cout << "  }\n\n";
            }
        }
    }

    std::cout << "========================================\n";
}
} // namespace bbfm
