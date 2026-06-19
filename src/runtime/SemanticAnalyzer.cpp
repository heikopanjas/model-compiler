#include "runtime/SemanticAnalyzer.h"
#include "Common.h"
#include "runtime/Array.h"
#include "runtime/Dictionary.h"
#include "runtime/String.h"
#include "Console.h"
#include "Contracts.h"
#include <iostream>

namespace runtime {
SemanticAnalyzer::SemanticAnalyzer(const AST* ast, const Array<String>& namespaces) : ast_(ast), namespaces_(namespaces), hasErrors_(false) {}

String SemanticAnalyzer::FormatNamespacePrefix() const
{
    if (namespaces_.GetCount() == 0)
    {
        return "";
    }

    String prefix;
    for (size_t i = 0; i < namespaces_.GetCount(); ++i)
    {
        prefix = prefix + namespaces_.GetValueAt(i) + String("::");
    }

    return prefix;
}

bool SemanticAnalyzer::Analyze()
{
    // Register built-in primitive types
    RegisterPrimitiveTypes();

    // Build symbol table from declarations
    if (BuildSymbolTable() == false)
    {
        return false;
    }

    // Validate type references
    if (ValidateTypeReferences() == false)
    {
        return false;
    }

    return hasErrors_ == false;
}

void SemanticAnalyzer::RegisterPrimitiveTypes()
{
    symbolTable_.SetValue(String("String"), TypeSymbol(String("String")));
    symbolTable_.SetValue(String("Int"), TypeSymbol(String("Int")));
    symbolTable_.SetValue(String("Real"), TypeSymbol(String("Real")));
    symbolTable_.SetValue(String("Bool"), TypeSymbol(String("Bool")));
    symbolTable_.SetValue(String("Timestamp"), TypeSymbol(String("Timestamp")));
    symbolTable_.SetValue(String("Timespan"), TypeSymbol(String("Timespan")));
    symbolTable_.SetValue(String("Date"), TypeSymbol(String("Date")));
    symbolTable_.SetValue(String("Guid"), TypeSymbol(String("Guid")));
}

bool SemanticAnalyzer::BuildSymbolTable()
{
    bool success = true;

    for (size_t declIndex = 0; declIndex < ast_->GetDeclarations().GetCount(); ++declIndex)
    {
        const Declaration* decl = ast_->GetDeclarations().At(declIndex).get();
        if (Declaration::Kind::ENUM == decl->GetKind())
        {
            const EnumDeclaration* enumDecl = decl->AsEnum();
            const String&     name     = enumDecl->GetName();

            // Check for duplicate type names
            if (TypeExists(name) == true)
            {
                ReportError("Type '" + name + "' is already declared");
                success = false;
                continue;
            }

            // Add to symbol table
            symbolTable_.SetValue(name, TypeSymbol(enumDecl));
        }
        else if (Declaration::Kind::CLASS == decl->GetKind())
        {
            const ClassDeclaration* classDecl = decl->AsClass();
            const String&      name      = classDecl->GetName();

            // Check for duplicate type names
            if (TypeExists(name) == true)
            {
                ReportError("Type '" + name + "' is already declared");
                success = false;
                continue;
            }

            // Add to symbol table
            symbolTable_.SetValue(name, TypeSymbol(classDecl));
        }
    }

    return success;
}

bool SemanticAnalyzer::ValidateTypeReferences()
{
    bool success = true;

    // First pass: Validate type references
    for (size_t declIndex = 0; declIndex < ast_->GetDeclarations().GetCount(); ++declIndex)
    {
        const Declaration* decl = ast_->GetDeclarations().At(declIndex).get();
        if (Declaration::Kind::CLASS == decl->GetKind())
        {
            const ClassDeclaration* classDecl = decl->AsClass();

            if (ValidateClassDeclaration(classDecl) == false)
            {
                success = false;
            }
        }
    }

    // Second pass: Check for inheritance cycles
    // This must be done after all types are validated to handle forward references
    for (size_t declIndex = 0; declIndex < ast_->GetDeclarations().GetCount(); ++declIndex)
    {
        const Declaration* decl = ast_->GetDeclarations().At(declIndex).get();
        if (Declaration::Kind::CLASS == decl->GetKind())
        {
            const ClassDeclaration* classDecl = decl->AsClass();

            if (classDecl->HasExplicitBase() == true)
            {
                std::set<String> visited;
                visited.insert(classDecl->GetName());
                if (HasInheritanceCycle(classDecl->GetBaseType(), visited) == true)
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
    RequireReturn(nullptr != classDecl, false);

    bool success = true;

    // Validate base type if specified
    if (classDecl->HasExplicitBase() == true)
    {
        const String& baseType = classDecl->GetBaseType();

        if (TypeExists(baseType) == false)
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
    for (size_t fieldIndex = 0; fieldIndex < classDecl->GetFields().GetCount(); ++fieldIndex)
    {
        const Field* field = classDecl->GetFields().At(fieldIndex).get();
        // Skip alias fields - they don't have explicit types
        if (field->IsAlias() == true)
        {
            continue;
        }

        const TypeSpec* typeSpec = field->GetType();

        if (nullptr != typeSpec && typeSpec->IsUserDefined())
        {
            const UserDefinedTypeSpec* userType = static_cast<const UserDefinedTypeSpec*>(typeSpec);
            const String&         typeName = userType->GetTypeName();

            if (TypeExists(typeName) == false)
            {
                ReportError("Field '" + field->GetName() + "' in class '" + classDecl->GetName() + "' has undefined type '" + typeName + "'");
                success = false;
            }
        }
    }

    // Validate field uniqueness
    if (ValidateFieldUniqueness(classDecl) == false)
    {
        success = false;
    }

    // Validate invariants
    if (ValidateInvariants(classDecl) == false)
    {
        success = false;
    }

    // Validate computed features
    if (ValidateComputedFeatures(classDecl) == false)
    {
        success = false;
    }

    return success;
}

bool SemanticAnalyzer::HasInheritanceCycle(const String& className, std::set<String>& visited)
{
    RequireReturn(className.IsEmpty() == false, false);

    // If we've visited this class before, we have a cycle
    if (visited.count(className) != 0)
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
    if (classDecl->HasExplicitBase() == false)
    {
        return false;
    }

    // Add current class to visited set before recursing
    visited.insert(className);

    // Recursively check base type
    return HasInheritanceCycle(classDecl->GetBaseType(), visited);
}

void SemanticAnalyzer::GetAllFields(const ClassDeclaration* classDecl, Array<const Field*>& allFields) const
{
    // Use a set to track visited classes and prevent infinite recursion on cycles
    std::set<String> visited;
    GetAllFieldsHelper(classDecl, allFields, visited);
}

void SemanticAnalyzer::GetAllFieldsHelper(const ClassDeclaration* classDecl, Array<const Field*>& allFields, std::set<String>& visited) const
{
    Require(nullptr != classDecl);

    // If we've already visited this class, stop (cycle detected)
    if (visited.count(classDecl->GetName()) != 0)
    {
        return;
    }

    visited.insert(classDecl->GetName());

    // First, get fields from base class if any
    if (classDecl->HasExplicitBase() == true)
    {
        const TypeSymbol* baseSym = LookupType(classDecl->GetBaseType());
        if (nullptr != baseSym && TypeSymbol::Kind::CLASS == baseSym->kind)
        {
            GetAllFieldsHelper(baseSym->classDecl, allFields, visited);
        }
    }

    // Then add this class's fields
    for (size_t fieldIndex = 0; fieldIndex < classDecl->GetFields().GetCount(); ++fieldIndex)
    {
        allFields.AddValue(classDecl->GetFields().At(fieldIndex).get());
    }
}

void SemanticAnalyzer::GetAllInvariants(const ClassDeclaration* classDecl, Array<const Invariant*>& allInvariants) const
{
    // Use a set to track visited classes and prevent infinite recursion on cycles
    std::set<String> visited;
    GetAllInvariantsHelper(classDecl, allInvariants, visited);
}

void SemanticAnalyzer::GetAllInvariantsHelper(
    const ClassDeclaration* classDecl, Array<const Invariant*>& allInvariants, std::set<String>& visited) const
{
    Require(nullptr != classDecl);

    // If we've already visited this class, stop (cycle detected)
    if (visited.count(classDecl->GetName()) != 0)
    {
        return;
    }

    visited.insert(classDecl->GetName());

    // First, get invariants from base class if any
    if (classDecl->HasExplicitBase() == true)
    {
        const TypeSymbol* baseSym = LookupType(classDecl->GetBaseType());
        if (nullptr != baseSym && TypeSymbol::Kind::CLASS == baseSym->kind)
        {
            GetAllInvariantsHelper(baseSym->classDecl, allInvariants, visited);
        }
    }

    // Then add this class's invariants
    for (size_t invIndex = 0; invIndex < classDecl->GetInvariants().GetCount(); ++invIndex)
    {
        allInvariants.AddValue(classDecl->GetInvariants().At(invIndex).get());
    }
}

bool SemanticAnalyzer::ValidateFieldUniqueness(const ClassDeclaration* classDecl)
{
    RequireReturn(nullptr != classDecl, false);

    Array<const Field*> allFields;
    GetAllFields(classDecl, allFields);

    std::set<String> fieldNames;
    bool                  success = true;

    for (size_t fieldIndex = 0; fieldIndex < allFields.GetCount(); ++fieldIndex)
    {
        const Field* field = allFields.GetValueAt(fieldIndex);
        const String& name = field->GetName();
        if (fieldNames.count(name) != 0)
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
    RequireReturn(nullptr != classDecl, false);

    bool success = true;

    // Get all fields (including inherited) for validation
    Array<const Field*> allFields;
    GetAllFields(classDecl, allFields);

    std::set<String> fieldNames;
    for (size_t fieldIndex = 0; fieldIndex < allFields.GetCount(); ++fieldIndex)
    {
        fieldNames.insert(allFields.GetValueAt(fieldIndex)->GetName());
    }

    // Validate each invariant
    for (size_t invIndex = 0; invIndex < classDecl->GetInvariants().GetCount(); ++invIndex)
    {
        const Invariant* invariant = classDecl->GetInvariants().At(invIndex).get();
        const Expression* expr = invariant->GetExpression();
        if (nullptr == expr)
        {
            ReportError("Invariant '" + invariant->GetName() + "' in class '" + classDecl->GetName() + "' has no expression");
            success = false;
            continue;
        }

        // Collect all field references in the expression
        std::set<String> referencedFields;
        CollectFieldReferences(expr, referencedFields);

        // Validate that all referenced fields exist
        for (const String& fieldName : referencedFields)
        {
            if (fieldNames.count(fieldName) == 0)
            {
                ReportError("Invariant '" + invariant->GetName() + "' in class '" + classDecl->GetName() + "' references undefined field '" + fieldName + "'");
                success = false;
            }
        }
    }

    return success;
}

void SemanticAnalyzer::CollectFieldReferences(const Expression* expr, std::set<String>& fields) const
{
    Require(nullptr != expr);

    // Check if this is a field reference
    const FieldReference* fieldRef = dynamic_cast<const FieldReference*>(expr);
    if (nullptr != fieldRef)
    {
        fields.insert(fieldRef->GetFieldName());
        return;
    }

    // Check if this is a member access expression
    const MemberAccessExpression* memberAccess = dynamic_cast<const MemberAccessExpression*>(expr);
    if (nullptr != memberAccess)
    {
        // Recursively collect from the object expression
        CollectFieldReferences(memberAccess->GetObject(), fields);
        return;
    }

    // Check if this is a binary expression
    const BinaryExpression* binExpr = dynamic_cast<const BinaryExpression*>(expr);
    if (nullptr != binExpr)
    {
        CollectFieldReferences(binExpr->GetLeft(), fields);
        CollectFieldReferences(binExpr->GetRight(), fields);
        return;
    }

    // Check if this is a unary expression
    const UnaryExpression* unaryExpr = dynamic_cast<const UnaryExpression*>(expr);
    if (nullptr != unaryExpr)
    {
        CollectFieldReferences(unaryExpr->GetOperand(), fields);
        return;
    }

    // Check if this is a parenthesized expression
    const ParenthesizedExpression* parenExpr = dynamic_cast<const ParenthesizedExpression*>(expr);
    if (nullptr != parenExpr)
    {
        CollectFieldReferences(parenExpr->GetExpression(), fields);
        return;
    }

    // Check if this is a function call
    const FunctionCall* funcCall = dynamic_cast<const FunctionCall*>(expr);
    if (nullptr != funcCall)
    {
        for (size_t argIndex = 0; argIndex < funcCall->GetArguments().GetCount(); ++argIndex)
        {
            CollectFieldReferences(funcCall->GetArguments().At(argIndex).get(), fields);
        }
        return;
    }

    // Literals don't contain field references
}

bool SemanticAnalyzer::ValidateComputedFeatures(const ClassDeclaration* classDecl)
{
    RequireReturn(nullptr != classDecl, false);

    bool success = true;

    // Get all fields including inherited ones
    Array<const Field*> allFields;
    GetAllFields(classDecl, allFields);

    // Build set of available field names for quick lookup
    std::set<String> availableFields;
    for (size_t fieldIndex = 0; fieldIndex < allFields.GetCount(); ++fieldIndex)
    {
        availableFields.insert(allFields.GetValueAt(fieldIndex)->GetName());
    }

    // Validate each computed feature and alias
    for (size_t fieldIndex = 0; fieldIndex < classDecl->GetFields().GetCount(); ++fieldIndex)
    {
        const Field* field = classDecl->GetFields().At(fieldIndex).get();
        if (field->IsAlias() == true)
        {
            if (ValidateAliasField(field, classDecl, availableFields) == false)
            {
                success = false;
            }
        }
        else if (field->IsComputed() == true)
        {
            if (ValidateComputedFeatureExpression(field, classDecl, availableFields) == false)
            {
                success = false;
            }
        }
    }

    return success;
}

bool SemanticAnalyzer::ValidateAliasField(const Field* field, const ClassDeclaration* classDecl, const std::set<String>& availableFields)
{
    RequireReturn(nullptr != field, false);
    RequireReturn(nullptr != classDecl, false);

    bool              success = true;
    const Expression* expr    = field->GetInitializer();

    if (nullptr == expr)
    {
        ReportError("Alias '" + field->GetName() + "' in class '" + classDecl->GetName() + "' has no target field specified");
        return false;
    }

    // Alias target must be a simple FieldReference, not a complex expression
    const FieldReference* fieldRef = dynamic_cast<const FieldReference*>(expr);
    if (nullptr == fieldRef)
    {
        ReportError("Alias '" + field->GetName() + "' in class '" + classDecl->GetName() + "' must reference a simple field, not a complex expression");
        return false;
    }

    // Validate that the target field exists
    const String& targetFieldName = fieldRef->GetFieldName();
    if (availableFields.count(targetFieldName) == 0)
    {
        ReportError("Alias '" + field->GetName() + "' in class '" + classDecl->GetName() + "' references undefined field '" + targetFieldName + "'");
        return false;
    }

    // Find the target field to validate it's not another alias (prevent alias chains)
    const Field* targetField = FindFieldInClass(classDecl, targetFieldName);
    if (nullptr != targetField && targetField->IsAlias() == true)
    {
        ReportError(
            "Alias '" + field->GetName() + "' in class '" + classDecl->GetName() + "' cannot reference another alias '" + targetFieldName +
            "' - alias chaining is not allowed");
        success = false;
    }

    return success;
}

bool SemanticAnalyzer::ValidateComputedFeatureExpression(const Field* field, const ClassDeclaration* classDecl, const std::set<String>& availableFields)
{
    RequireReturn(nullptr != field, false);
    RequireReturn(nullptr != classDecl, false);

    bool              success = true;
    const Expression* expr    = field->GetInitializer();

    if (nullptr == expr)
    {
        return true; // Not a computed feature
    }

    // Check cardinality - computed features must be single-valued [1]
    const CardinalityModifier* cardinality = field->GetCardinalityModifier();
    if (nullptr != cardinality)
    {
        if (cardinality->IsArray() == true)
        {
            ReportError(
                "Computed feature '" + field->GetName() + "' in class '" + classDecl->GetName() +
                "' cannot be an array - computed features must have cardinality [1]");
            success = false;
        }
    }

    // Collect field references from the expression
    std::set<String> referencedFields;
    CollectFieldReferences(expr, referencedFields);

    // Validate that all referenced fields exist
    for (const String& refField : referencedFields)
    {
        if (availableFields.count(refField) == 0)
        {
            ReportError("Computed feature '" + field->GetName() + "' in class '" + classDecl->GetName() + "' references undefined field '" + refField + "'");
            success = false;
        }
    }

    // Validate that no optional fields are referenced
    for (const String& refField : referencedFields)
    {
        const Field* referencedField = FindFieldInClass(classDecl, refField);
        if (nullptr != referencedField)
        {
            const CardinalityModifier* refCardinality = referencedField->GetCardinalityModifier();
            if (nullptr != refCardinality && refCardinality->IsOptional() == true)
            {
                ReportError(
                    "Computed feature '" + field->GetName() + "' in class '" + classDecl->GetName() + "' references optional field '" + refField +
                    "' - computed features cannot use optional fields because they may not have a value");
                success = false;
            }
        }
    }

    // Validate member access expressions
    if (ValidateMemberAccessInExpression(expr, classDecl, "computed feature '" + field->GetName() + "'") == false)
    {
        success = false;
    }

    // Type checking - verify expression type matches declared field type
    const Expression::Type exprType = InferExpressionType(expr, classDecl);
    if (Expression::Type::UNKNOWN != exprType)
    {
        if (IsTypeCompatible(exprType, field->GetType()) == false)
        {
            const TypeSpec* fieldTypeSpec = field->GetType();
            String     fieldTypeName;
            if (fieldTypeSpec->IsPrimitive() == true)
            {
                const PrimitiveTypeSpec* primType = static_cast<const PrimitiveTypeSpec*>(fieldTypeSpec);
                fieldTypeName                     = PrimitiveTypeSpec::TypeToString(primType->GetType());
            }
            else
            {
                const UserDefinedTypeSpec* userType = static_cast<const UserDefinedTypeSpec*>(fieldTypeSpec);
                fieldTypeName                       = userType->GetTypeName();
            }

            // Map Expression::Type to string for error message
            String exprTypeName;
            switch (exprType)
            {
                case Expression::Type::INT:
                    exprTypeName = "Int";
                    break;
                case Expression::Type::REAL:
                    exprTypeName = "Real";
                    break;
                case Expression::Type::STRING:
                    exprTypeName = "String";
                    break;
                case Expression::Type::BOOL:
                    exprTypeName = "Bool";
                    break;
                case Expression::Type::TIMESTAMP:
                    exprTypeName = "Timestamp";
                    break;
                case Expression::Type::TIMESPAN:
                    exprTypeName = "Timespan";
                    break;
                case Expression::Type::GUID:
                    exprTypeName = "Guid";
                    break;
                default:
                    exprTypeName = "Unknown";
                    break;
            }

            ReportError(
                "Computed feature '" + field->GetName() + "' in class '" + classDecl->GetName() + "' has type mismatch: declared as '" + fieldTypeName +
                "' but expression evaluates to '" + exprTypeName + "'");
            success = false;
        }
    }

    return success;
}

bool SemanticAnalyzer::ValidateMemberAccessInExpression(const Expression* expr, const ClassDeclaration* classDecl, const String& errorContext)
{
    RequireReturn(nullptr != expr, true);

    bool success = true;

    // Check if this is a member access expression
    const MemberAccessExpression* memberAccess = dynamic_cast<const MemberAccessExpression*>(expr);
    if (nullptr != memberAccess)
    {
        if (ValidateMemberAccess(memberAccess, classDecl, errorContext) == false)
        {
            success = false;
        }
        // Don't recurse - ValidateMemberAccess handles the entire chain
        return success;
    }

    // Recursively check binary expressions
    const BinaryExpression* binExpr = dynamic_cast<const BinaryExpression*>(expr);
    if (nullptr != binExpr)
    {
        if (ValidateMemberAccessInExpression(binExpr->GetLeft(), classDecl, errorContext) == false)
        {
            success = false;
        }
        if (ValidateMemberAccessInExpression(binExpr->GetRight(), classDecl, errorContext) == false)
        {
            success = false;
        }
        return success;
    }

    // Recursively check unary expressions
    const UnaryExpression* unaryExpr = dynamic_cast<const UnaryExpression*>(expr);
    if (nullptr != unaryExpr)
    {
        return ValidateMemberAccessInExpression(unaryExpr->GetOperand(), classDecl, errorContext);
    }

    // Recursively check parenthesized expressions
    const ParenthesizedExpression* parenExpr = dynamic_cast<const ParenthesizedExpression*>(expr);
    if (nullptr != parenExpr)
    {
        return ValidateMemberAccessInExpression(parenExpr->GetExpression(), classDecl, errorContext);
    }

    // Recursively check function call arguments
    const FunctionCall* funcCall = dynamic_cast<const FunctionCall*>(expr);
    if (nullptr != funcCall)
    {
        for (size_t argIndex = 0; argIndex < funcCall->GetArguments().GetCount(); ++argIndex)
        {
            if (ValidateMemberAccessInExpression(funcCall->GetArguments().At(argIndex).get(), classDecl, errorContext) == false)
            {
                success = false;
            }
        }
        return success;
    }

    // Field references and literals don't need member access validation
    return true;
}

bool SemanticAnalyzer::ValidateMemberAccess(const MemberAccessExpression* memberAccess, const ClassDeclaration* classDecl, const String& errorContext)
{
    RequireReturn(nullptr != memberAccess, false);
    RequireReturn(nullptr != classDecl, false);

    bool success = true;

    // Get the object expression (left side of the dot)
    const Expression* object = memberAccess->GetObject();

    // Check if object is a field reference
    const FieldReference* fieldRef = dynamic_cast<const FieldReference*>(object);
    if (nullptr != fieldRef)
    {
        const String& objectFieldName = fieldRef->GetFieldName();

        // Find the field type
        const TypeSymbol* fieldType = GetFieldType(classDecl, objectFieldName);
        if (nullptr == fieldType)
        {
            ReportError("In " + errorContext + ": field '" + objectFieldName + "' not found in class '" + classDecl->GetName() + "'");
            return false;
        }

        // Verify it's a user-defined type (not a primitive)
        if (TypeSymbol::Kind::CLASS != fieldType->kind)
        {
            ReportError("In " + errorContext + ": cannot access member '" + memberAccess->GetMemberName() + "' on non-class field '" + objectFieldName + "'");
            return false;
        }

        // Verify the member exists in the field's type
        const ClassDeclaration* fieldClass = fieldType->classDecl;
        const TypeSymbol*       memberType = GetFieldType(fieldClass, memberAccess->GetMemberName());
        if (nullptr == memberType)
        {
            ReportError("In " + errorContext + ": class '" + fieldType->name + "' has no member '" + memberAccess->GetMemberName() + "'");
            return false;
        }
    }
    else
    {
        // Could be a nested member access (e.g., a.b.c) - recurse
        const MemberAccessExpression* nestedAccess = dynamic_cast<const MemberAccessExpression*>(object);
        if (nullptr != nestedAccess)
        {
            // Validate the nested access first
            if (ValidateMemberAccess(nestedAccess, classDecl, errorContext) == false)
            {
                return false;
            }
            // TODO: Get the type of the nested access and validate the member on that type
            // For now, we just validate the chain exists
        }
    }

    return success;
}

const TypeSymbol* SemanticAnalyzer::GetFieldType(const ClassDeclaration* classDecl, const String& fieldName) const
{
    RequireReturn(nullptr != classDecl, nullptr);
    RequireReturn(fieldName.IsEmpty() == false, nullptr);

    // Get all fields including inherited
    Array<const Field*> allFields;
    const_cast<SemanticAnalyzer*>(this)->GetAllFields(classDecl, allFields);

    // Find the field
    for (size_t fieldIndex = 0; fieldIndex < allFields.GetCount(); ++fieldIndex)
    {
        const Field* field = allFields.GetValueAt(fieldIndex);
        if (field->GetName() == fieldName)
        {
            // Handle alias fields - get type from target field
            if (field->IsAlias() == true)
            {
                const Expression*     expr     = field->GetInitializer();
                const FieldReference* fieldRef = dynamic_cast<const FieldReference*>(expr);
                if (nullptr != fieldRef)
                {
                    // Recursively get the type of the target field
                    return GetFieldType(classDecl, fieldRef->GetFieldName());
                }
                return nullptr;
            }

            const TypeSpec* typeSpec = field->GetType();

            if (nullptr != typeSpec && typeSpec->IsPrimitive() == true)
            {
                const PrimitiveTypeSpec* primType = static_cast<const PrimitiveTypeSpec*>(typeSpec);
                const String        typeName = PrimitiveTypeSpec::TypeToString(primType->GetType());
                return LookupType(typeName);
            }
            else if (nullptr != typeSpec && typeSpec->IsUserDefined())
            {
                const UserDefinedTypeSpec* userType = static_cast<const UserDefinedTypeSpec*>(typeSpec);
                return LookupType(userType->GetTypeName());
            }
        }
    }

    return nullptr;
}

Expression::Type SemanticAnalyzer::InferExpressionType(const Expression* expr, const ClassDeclaration* classDecl) const
{
    RequireReturn(nullptr != expr, Expression::Type::UNKNOWN);

    // Check for literal expressions
    const LiteralExpression* literal = dynamic_cast<const LiteralExpression*>(expr);
    if (nullptr != literal)
    {
        return literal->GetResultType();
    }

    // Check for field references
    const FieldReference* fieldRef = dynamic_cast<const FieldReference*>(expr);
    if (nullptr != fieldRef)
    {
        const TypeSymbol* fieldType = GetFieldType(classDecl, fieldRef->GetFieldName());
        if (nullptr != fieldType)
        {
            if (TypeSymbol::Kind::PRIMITIVE == fieldType->kind)
            {
                return PrimitiveNameToExpressionType(fieldType->name);
            }
            // User-defined types don't have Expression::Type representation
            return Expression::Type::UNKNOWN;
        }
        return Expression::Type::UNKNOWN;
    }

    // Check for member access expressions
    const MemberAccessExpression* memberAccess = dynamic_cast<const MemberAccessExpression*>(expr);
    if (nullptr != memberAccess)
    {
        // Get the type of the object
        const FieldReference* objectRef = dynamic_cast<const FieldReference*>(memberAccess->GetObject());
        if (nullptr != objectRef)
        {
            const TypeSymbol* objectType = GetFieldType(classDecl, objectRef->GetFieldName());
            if (nullptr != objectType && TypeSymbol::Kind::CLASS == objectType->kind)
            {
                // Look up the member type in the object's class
                const TypeSymbol* memberType = GetFieldType(objectType->classDecl, memberAccess->GetMemberName());
                if (nullptr != memberType && TypeSymbol::Kind::PRIMITIVE == memberType->kind)
                {
                    return PrimitiveNameToExpressionType(memberType->name);
                }
            }
        }
        // Could be nested member access - would need recursive handling
        return Expression::Type::UNKNOWN;
    }

    // Check for binary expressions
    const BinaryExpression* binExpr = dynamic_cast<const BinaryExpression*>(expr);
    if (nullptr != binExpr)
    {
        // For comparison and logical operators, result is always BOOL
        BinaryExpression::Op op = binExpr->GetOperator();
        if (BinaryExpression::Op::LT == op || BinaryExpression::Op::GT == op || BinaryExpression::Op::LE == op || BinaryExpression::Op::GE == op ||
            BinaryExpression::Op::EQ == op || BinaryExpression::Op::NE == op || BinaryExpression::Op::AND == op || BinaryExpression::Op::OR == op)
        {
            return Expression::Type::BOOL;
        }

        // For arithmetic operators, infer from operands
        const Expression::Type leftType  = InferExpressionType(binExpr->GetLeft(), classDecl);
        const Expression::Type rightType = InferExpressionType(binExpr->GetRight(), classDecl);

        // If either is UNKNOWN, we can't infer
        if (leftType == Expression::Type::UNKNOWN || rightType == Expression::Type::UNKNOWN)
        {
            return Expression::Type::UNKNOWN;
        }

        // Type widening: if either is REAL, result is REAL
        if (leftType == Expression::Type::REAL || rightType == Expression::Type::REAL || leftType == Expression::Type::TIMESTAMP ||
            rightType == Expression::Type::TIMESTAMP || leftType == Expression::Type::TIMESPAN || rightType == Expression::Type::TIMESPAN)
        {
            return Expression::Type::REAL;
        }

        // If both are INT, result is INT
        if (leftType == Expression::Type::INT && rightType == Expression::Type::INT)
        {
            return Expression::Type::INT;
        }

        // If both are STRING, result is STRING (for concatenation)
        if (leftType == Expression::Type::STRING && rightType == Expression::Type::STRING && BinaryExpression::Op::ADD == op)
        {
            return Expression::Type::STRING;
        }

        return Expression::Type::UNKNOWN;
    }

    // Check for unary expressions
    const UnaryExpression* unaryExpr = dynamic_cast<const UnaryExpression*>(expr);
    if (nullptr != unaryExpr)
    {
        // NOT operator returns BOOL
        if (unaryExpr->GetOperator() == UnaryExpression::Op::NOT)
        {
            return Expression::Type::BOOL;
        }
    }

    // Check for parenthesized expressions
    const ParenthesizedExpression* parenExpr = dynamic_cast<const ParenthesizedExpression*>(expr);
    if (nullptr != parenExpr)
    {
        return InferExpressionType(parenExpr->GetExpression(), classDecl);
    }

    // Check for function calls
    const FunctionCall* funcCall = dynamic_cast<const FunctionCall*>(expr);
    if (nullptr != funcCall)
    {
        return funcCall->GetResultType();
    }

    return Expression::Type::UNKNOWN;
}

bool SemanticAnalyzer::IsTypeCompatible(Expression::Type exprType, const TypeSpec* fieldTypeSpec) const
{
    RequireReturn(nullptr != fieldTypeSpec, false);

    // User-defined types can't be validated this way
    if (fieldTypeSpec->IsUserDefined())
    {
        return true; // For now, assume user-defined types are OK
    }

    const PrimitiveTypeSpec* primType  = static_cast<const PrimitiveTypeSpec*>(fieldTypeSpec);
    const Expression::Type   fieldType = PrimitiveNameToExpressionType(PrimitiveTypeSpec::TypeToString(primType->GetType()));

    // Exact match
    if (exprType == fieldType)
    {
        return true;
    }

    // Allow Int -> Real (widening conversion, safe)
    if (Expression::Type::INT == exprType && Expression::Type::REAL == fieldType)
    {
        return true;
    }

    // Timestamp and Timespan are both represented as Real internally
    if (Expression::Type::REAL == exprType && (Expression::Type::TIMESTAMP == fieldType || Expression::Type::TIMESPAN == fieldType))
    {
        return true;
    }

    if ((Expression::Type::TIMESTAMP == exprType || Expression::Type::TIMESPAN == exprType) && Expression::Type::REAL == fieldType)
    {
        return true;
    }

    // No other implicit conversions allowed
    return false;
}

Expression::Type SemanticAnalyzer::PrimitiveNameToExpressionType(const String& typeName) const
{
    if (typeName == "Int")
    {
        return Expression::Type::INT;
    }
    if (typeName == "Real")
    {
        return Expression::Type::REAL;
    }
    if (typeName == "String")
    {
        return Expression::Type::STRING;
    }
    if (typeName == "Bool")
    {
        return Expression::Type::BOOL;
    }
    if (typeName == "Timestamp")
    {
        return Expression::Type::TIMESTAMP;
    }
    if (typeName == "Timespan")
    {
        return Expression::Type::TIMESPAN;
    }
    if (typeName == "Guid")
    {
        return Expression::Type::GUID;
    }

    return Expression::Type::UNKNOWN;
}

bool SemanticAnalyzer::TypeExists(const String& typeName) const
{
    return symbolTable_.ContainsKey(typeName);
}

const TypeSymbol* SemanticAnalyzer::LookupType(const String& typeName) const
{
    return symbolTable_.GetValuePtr(typeName);
}

const Field* SemanticAnalyzer::FindFieldInClass(const ClassDeclaration* classDecl, const String& fieldName) const
{
    RequireReturn(nullptr != classDecl, nullptr);
    RequireReturn(fieldName.IsEmpty() == false, nullptr);

    // Get all fields including inherited ones
    Array<const Field*> allFields;
    GetAllFields(classDecl, allFields);

    // Search for the field by name
    for (size_t fieldIndex = 0; fieldIndex < allFields.GetCount(); ++fieldIndex)
    {
        const Field* field = allFields.GetValueAt(fieldIndex);
        if (nullptr != field && field->GetName() == fieldName)
        {
            return field;
        }
    }

    return nullptr;
}

void SemanticAnalyzer::ReportError(const String& message)
{
    Console::ReportError("Semantic error: " + message);
    hasErrors_ = true;
}

bool SemanticAnalyzer::HasErrors() const
{
    return hasErrors_;
}

const Dictionary<String, TypeSymbol>& SemanticAnalyzer::GetSymbolTable() const
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

    for (size_t i = 0; i < symbolTable_.GetCount(); ++i)
    {
        const TypeSymbol& entry = symbolTable_.GetValueAt(i);
        if (TypeSymbol::Kind::PRIMITIVE == entry.kind)
        {
            primitiveCount++;
        }
        else if (TypeSymbol::Kind::ENUM == entry.kind)
        {
            enumCount++;
        }
        else if (TypeSymbol::Kind::CLASS == entry.kind)
        {
            classCount++;
        }
    }

    std::cout << "Total Symbols: " << symbolTable_.GetCount() << "\n";
    std::cout << "  Primitive Types: " << primitiveCount << "\n";
    std::cout << "  Enumerations: " << enumCount << "\n";
    std::cout << "  Classes: " << classCount << "\n";
    std::cout << "\n";

    // Dump primitive types
    if (primitiveCount > 0)
    {
        std::cout << "Primitive Types:\n";
        std::cout << "----------------\n";
        for (size_t i = 0; i < symbolTable_.GetCount(); ++i)
        {
            const TypeSymbol& entry = symbolTable_.GetValueAt(i);
            if (TypeSymbol::Kind::PRIMITIVE == entry.kind)
            {
                std::cout << "  <builtin>::" << entry.name << "\n";
            }
        }
        std::cout << "\n";
    }

    // Dump enums
    if (enumCount > 0)
    {
        std::cout << "Enumerations:\n";
        std::cout << "-------------\n";
        String nsPrefix = FormatNamespacePrefix();
        for (size_t i = 0; i < symbolTable_.GetCount(); ++i)
        {
            const TypeSymbol& entry = symbolTable_.GetValueAt(i);
            if (TypeSymbol::Kind::ENUM == entry.kind)
            {
                std::cout << "  enum " << nsPrefix << entry.name << " {\n";
                const auto& values = entry.enumDecl->GetValues();
                for (size_t i = 0; i < values.GetCount(); ++i)
                {
                    std::cout << "    " << values.GetValueAt(i);
                    if (i + 1 < values.GetCount())
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
        const String nsPrefix = FormatNamespacePrefix();
        for (size_t i = 0; i < symbolTable_.GetCount(); ++i)
        {
            const TypeSymbol& entry = symbolTable_.GetValueAt(i);
            if (TypeSymbol::Kind::CLASS == entry.kind)
            {
                std::cout << "  class " << nsPrefix << entry.name;

                // Show inheritance
                const String& baseType = entry.classDecl->GetBaseType();
                if (baseType.IsEmpty() == false)
                {
                    std::cout << " inherits " << nsPrefix << baseType;
                }
                std::cout << " {\n";

                // Show fields (including inherited)
                Array<const Field*> allFields;
                GetAllFields(entry.classDecl, allFields);

                // Also get just the local fields for comparison
                const auto&            localFields = entry.classDecl->GetFields();
                std::set<const Field*> localFieldSet;
                for (size_t lfIndex = 0; lfIndex < localFields.GetCount(); ++lfIndex)
                {
                    localFieldSet.insert(localFields.At(lfIndex).get());
                }

                if (allFields.GetCount() > 0)
                {
                    std::cout << "    Features:\n";
                    for (size_t afIndex = 0; afIndex < allFields.GetCount(); ++afIndex)
                    {
                        const Field* field = allFields.GetValueAt(afIndex);
                        // Determine if this is a local or inherited field
                        const bool isLocal = (localFieldSet.count(field) != 0);
                        std::cout << "      " << (isLocal == true ? "Self::" : "Base::") << field->GetName() << ": ";

                        // Handle alias fields specially
                        if (field->IsAlias() == true)
                        {
                            // For aliases, resolve the target field's type
                            const TypeSymbol* targetType = GetFieldType(entry.classDecl, field->GetName());
                            if (nullptr != targetType)
                            {
                                if (TypeSymbol::Kind::PRIMITIVE == targetType->kind)
                                {
                                    std::cout << "<builtin>::" << targetType->name;
                                }
                                else
                                {
                                    std::cout << nsPrefix << targetType->name;
                                }
                            }
                            else
                            {
                                std::cout << "<unresolved>";
                            }
                        }
                        else
                        {
                            // Get type name based on TypeSpec type
                            const TypeSpec* typeSpec = field->GetType();
                            if (nullptr != typeSpec)
                            {
                                if (typeSpec->IsPrimitive() == true)
                                {
                                    const PrimitiveTypeSpec* primType = dynamic_cast<const PrimitiveTypeSpec*>(typeSpec);
                                    std::cout << "<builtin>::" << PrimitiveTypeSpec::TypeToString(primType->GetType());
                                }
                                else
                                {
                                    const UserDefinedTypeSpec* userType = dynamic_cast<const UserDefinedTypeSpec*>(typeSpec);
                                    std::cout << nsPrefix << userType->GetTypeName();
                                }
                            }
                            else
                            {
                                std::cout << "<null type>";
                            }
                        }

                        // Show modifiers
                        const auto& modifiers = field->GetModifiers();
                        if (modifiers.GetCount() > 0)
                        {
                            std::cout << " [";
                            for (size_t i = 0; i < modifiers.GetCount(); ++i)
                            {
                                if (const CardinalityModifier* cardMod = dynamic_cast<const CardinalityModifier*>(modifiers.At(i).get()))
                                {
                                    std::cout << cardMod->GetMin() << ".." << cardMod->GetMax();
                                }
                                else if (dynamic_cast<const UniqueModifier*>(modifiers.At(i).get()))
                                {
                                    std::cout << "unique";
                                }

                                if (i + 1 < modifiers.GetCount())
                                {
                                    std::cout << ", ";
                                }
                            }
                            std::cout << "]";
                        }

                        // Show computed feature expression with annotated field origins
                        if (field->IsComputed() == true)
                        {
                            const Expression* initializer = field->GetInitializer();
                            if (nullptr != initializer)
                            {
                                String annotatedExpr = AnnotateExpressionWithOrigin(initializer, entry.classDecl, localFieldSet);
                                std::cout << " = " << annotatedExpr;
                            }
                        }

                        // Show alias target
                        if (field->IsAlias() == true)
                        {
                            const Expression* initializer = field->GetInitializer();
                            if (nullptr != initializer)
                            {
                                const FieldReference* fieldRef = dynamic_cast<const FieldReference*>(initializer);
                                if (nullptr != fieldRef)
                                {
                                    std::cout << " (alias -> " << fieldRef->GetFieldName() << ")";
                                }
                            }
                        }

                        std::cout << "\n";
                    }
                }

                // Show invariants (including inherited)
                Array<const Invariant*> allInvariants;
                GetAllInvariants(entry.classDecl, allInvariants);

                // Also get just the local invariants for comparison
                const auto&                localInvariants = entry.classDecl->GetInvariants();
                std::set<const Invariant*> localInvariantSet;
                for (size_t liIndex = 0; liIndex < localInvariants.GetCount(); ++liIndex)
                {
                    localInvariantSet.insert(localInvariants.At(liIndex).get());
                }

                if (allInvariants.GetCount() > 0)
                {
                    std::cout << "    Invariants:\n";
                    for (size_t aiIndex = 0; aiIndex < allInvariants.GetCount(); ++aiIndex)
                    {
                        const Invariant* invariant = allInvariants.GetValueAt(aiIndex);
                        // Determine if this is a local or inherited invariant
                        const bool isLocal = (localInvariantSet.count(invariant) != 0);
                        std::cout << "      " << (isLocal == true ? "Self::" : "Base::") << invariant->GetName() << ": ";
                        if (nullptr != invariant->GetExpression())
                        {
                            std::cout << invariant->GetExpression()->ToString();
                        }
                        std::cout << "\n";
                    }
                }

                std::cout << "  }\n\n";
            }
        }
    }

    std::cout << "========================================\n";
}

String
    SemanticAnalyzer::AnnotateExpressionWithOrigin(const Expression* expr, const ClassDeclaration* classDecl, const std::set<const Field*>& localFields) const
{
    RequireReturn(nullptr != expr, String{});

    // Handle different expression types
    if (const BinaryExpression* binExpr = dynamic_cast<const BinaryExpression*>(expr))
    {
        const String left  = AnnotateExpressionWithOrigin(binExpr->GetLeft(), classDecl, localFields);
        const String right = AnnotateExpressionWithOrigin(binExpr->GetRight(), classDecl, localFields);
        String       op;

        switch (binExpr->GetOperator())
        {
            case BinaryExpression::Op::ADD:
                op = " + ";
                break;
            case BinaryExpression::Op::SUB:
                op = " - ";
                break;
            case BinaryExpression::Op::MUL:
                op = " * ";
                break;
            case BinaryExpression::Op::DIV:
                op = " / ";
                break;
            case BinaryExpression::Op::MOD:
                op = " % ";
                break;
            case BinaryExpression::Op::LT:
                op = " < ";
                break;
            case BinaryExpression::Op::GT:
                op = " > ";
                break;
            case BinaryExpression::Op::LE:
                op = " <= ";
                break;
            case BinaryExpression::Op::GE:
                op = " >= ";
                break;
            case BinaryExpression::Op::EQ:
                op = " == ";
                break;
            case BinaryExpression::Op::NE:
                op = " != ";
                break;
            case BinaryExpression::Op::AND:
                op = " && ";
                break;
            case BinaryExpression::Op::OR:
                op = " || ";
                break;
        }

        return left + op + right;
    }
    else if (const UnaryExpression* unaryExpr = dynamic_cast<const UnaryExpression*>(expr))
    {
        const String operand = AnnotateExpressionWithOrigin(unaryExpr->GetOperand(), classDecl, localFields);
        if (unaryExpr->GetOperator() == UnaryExpression::Op::NEG)
        {
            return "-" + operand;
        }
        else if (unaryExpr->GetOperator() == UnaryExpression::Op::NOT)
        {
            return "!" + operand;
        }
    }
    else if (const FieldReference* fieldRef = dynamic_cast<const FieldReference*>(expr))
    {
        // Look up the field to determine if it's local or inherited
        const String& fieldName = fieldRef->GetFieldName();

        // Find this field in all fields
        Array<const Field*> allFields;
        GetAllFields(classDecl, allFields);

        bool         isLocal    = false;
        const Field* foundField = nullptr;
        for (size_t fieldIndex = 0; fieldIndex < allFields.GetCount(); ++fieldIndex)
        {
            const Field* field = allFields.GetValueAt(fieldIndex);
            if (fieldName == field->GetName())
            {
                foundField = field;
                isLocal    = (localFields.count(field) != 0);
                break;
            }
        }

        // Annotate with origin marker
        if (nullptr != foundField)
        {
            return (true == isLocal ? "Self::" : "Base::") + fieldName;
        }
        else
        {
            return fieldName; // Fallback if not found
        }
    }
    else if (const MemberAccessExpression* memberExpr = dynamic_cast<const MemberAccessExpression*>(expr))
    {
        const String object = AnnotateExpressionWithOrigin(memberExpr->GetObject(), classDecl, localFields);
        const String member = memberExpr->GetMemberName();
        return object + "." + member;
    }
    else if (const LiteralExpression* litExpr = dynamic_cast<const LiteralExpression*>(expr))
    {
        return litExpr->ToString();
    }
    else if (const ParenthesizedExpression* parenExpr = dynamic_cast<const ParenthesizedExpression*>(expr))
    {
        const String inner = AnnotateExpressionWithOrigin(parenExpr->GetExpression(), classDecl, localFields);
        return "(" + inner + ")";
    }
    else if (const FunctionCall* funcCall = dynamic_cast<const FunctionCall*>(expr))
    {
        return funcCall->ToString(); // Function calls not yet implemented, use default
    }

    return expr->ToString(); // Fallback
}
} // namespace runtime
