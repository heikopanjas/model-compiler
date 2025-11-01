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

    // Validate computed features
    if (!ValidateComputedFeatures(classDecl))
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
        const Expression* expr = invariant->GetExpression();
        if (nullptr == expr)
        {
            ReportError("Invariant '" + invariant->GetName() + "' in class '" + classDecl->GetName() + "' has no expression");
            success = false;
            continue;
        }

        // Collect all field references in the expression
        std::set<std::string> referencedFields;
        CollectFieldReferences(expr, referencedFields);

        // Validate that all referenced fields exist
        for (const std::string& fieldName : referencedFields)
        {
            if (0 == fieldNames.count(fieldName))
            {
                ReportError("Invariant '" + invariant->GetName() + "' in class '" + classDecl->GetName() + "' references undefined field '" + fieldName + "'");
                success = false;
            }
        }
    }

    return success;
}

void SemanticAnalyzer::CollectFieldReferences(const Expression* expr, std::set<std::string>& fields) const
{
    if (nullptr == expr)
    {
        return;
    }

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
        for (const auto& arg : funcCall->GetArguments())
        {
            CollectFieldReferences(arg.get(), fields);
        }
        return;
    }

    // Literals don't contain field references
}

bool SemanticAnalyzer::ValidateComputedFeatures(const ClassDeclaration* classDecl)
{
    bool success = true;

    // Get all fields including inherited ones
    std::vector<const Field*> allFields;
    GetAllFields(classDecl, allFields);

    // Build set of available field names for quick lookup
    std::set<std::string> availableFields;
    for (const auto* field : allFields)
    {
        availableFields.insert(field->GetName());
    }

    // Validate each computed feature
    for (const auto& field : classDecl->GetFields())
    {
        if (field->IsComputed())
        {
            if (!ValidateComputedFeatureExpression(field.get(), classDecl, availableFields))
            {
                success = false;
            }
        }
    }

    return success;
}

bool SemanticAnalyzer::ValidateComputedFeatureExpression(const Field* field, const ClassDeclaration* classDecl, const std::set<std::string>& availableFields)
{
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
        if (cardinality->IsArray())
        {
            ReportError(
                "Computed feature '" + field->GetName() + "' in class '" + classDecl->GetName() +
                "' cannot be an array - computed features must have cardinality [1]");
            success = false;
        }
    }

    // Collect field references from the expression
    std::set<std::string> referencedFields;
    CollectFieldReferences(expr, referencedFields);

    // Validate that all referenced fields exist
    for (const std::string& refField : referencedFields)
    {
        if (0 == availableFields.count(refField))
        {
            ReportError("Computed feature '" + field->GetName() + "' in class '" + classDecl->GetName() + "' references undefined field '" + refField + "'");
            success = false;
        }
    }

    // Validate member access expressions
    if (!ValidateMemberAccessInExpression(expr, classDecl, "computed feature '" + field->GetName() + "'"))
    {
        success = false;
    }

    // Type checking - verify expression type matches declared field type
    Expression::Type exprType = InferExpressionType(expr, classDecl);
    if (Expression::Type::UNKNOWN != exprType)
    {
        if (!IsTypeCompatible(exprType, field->GetType()))
        {
            const TypeSpec* fieldTypeSpec = field->GetType();
            std::string     fieldTypeName;
            if (fieldTypeSpec->IsPrimitive())
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
            std::string exprTypeName;
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

bool SemanticAnalyzer::ValidateMemberAccessInExpression(const Expression* expr, const ClassDeclaration* classDecl, const std::string& errorContext)
{
    if (nullptr == expr)
    {
        return true;
    }

    bool success = true;

    // Check if this is a member access expression
    const MemberAccessExpression* memberAccess = dynamic_cast<const MemberAccessExpression*>(expr);
    if (nullptr != memberAccess)
    {
        if (!ValidateMemberAccess(memberAccess, classDecl, errorContext))
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
        if (!ValidateMemberAccessInExpression(binExpr->GetLeft(), classDecl, errorContext))
        {
            success = false;
        }
        if (!ValidateMemberAccessInExpression(binExpr->GetRight(), classDecl, errorContext))
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
        for (const auto& arg : funcCall->GetArguments())
        {
            if (!ValidateMemberAccessInExpression(arg.get(), classDecl, errorContext))
            {
                success = false;
            }
        }
        return success;
    }

    // Field references and literals don't need member access validation
    return true;
}

bool SemanticAnalyzer::ValidateMemberAccess(const MemberAccessExpression* memberAccess, const ClassDeclaration* classDecl, const std::string& errorContext)
{
    bool success = true;

    // Get the object expression (left side of the dot)
    const Expression* object = memberAccess->GetObject();

    // Check if object is a field reference
    const FieldReference* fieldRef = dynamic_cast<const FieldReference*>(object);
    if (nullptr != fieldRef)
    {
        const std::string& objectFieldName = fieldRef->GetFieldName();

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
            if (!ValidateMemberAccess(nestedAccess, classDecl, errorContext))
            {
                return false;
            }
            // TODO: Get the type of the nested access and validate the member on that type
            // For now, we just validate the chain exists
        }
    }

    return success;
}

const TypeSymbol* SemanticAnalyzer::GetFieldType(const ClassDeclaration* classDecl, const std::string& fieldName) const
{
    // Get all fields including inherited
    std::vector<const Field*> allFields;
    const_cast<SemanticAnalyzer*>(this)->GetAllFields(classDecl, allFields);

    // Find the field
    for (const auto* field : allFields)
    {
        if (field->GetName() == fieldName)
        {
            const TypeSpec* typeSpec = field->GetType();

            if (typeSpec->IsPrimitive())
            {
                const PrimitiveTypeSpec* primType = static_cast<const PrimitiveTypeSpec*>(typeSpec);
                const std::string        typeName = PrimitiveTypeSpec::TypeToString(primType->GetType());
                return LookupType(typeName);
            }
            else if (typeSpec->IsUserDefined())
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
    if (nullptr == expr)
    {
        return Expression::Type::UNKNOWN;
    }

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
        Expression::Type leftType  = InferExpressionType(binExpr->GetLeft(), classDecl);
        Expression::Type rightType = InferExpressionType(binExpr->GetRight(), classDecl);

        // If either is UNKNOWN, we can't infer
        if (Expression::Type::UNKNOWN == leftType || Expression::Type::UNKNOWN == rightType)
        {
            return Expression::Type::UNKNOWN;
        }

        // Type widening: if either is REAL, result is REAL
        if (Expression::Type::REAL == leftType || Expression::Type::REAL == rightType || Expression::Type::TIMESTAMP == leftType ||
            Expression::Type::TIMESTAMP == rightType || Expression::Type::TIMESPAN == leftType || Expression::Type::TIMESPAN == rightType)
        {
            return Expression::Type::REAL;
        }

        // If both are INT, result is INT
        if (Expression::Type::INT == leftType && Expression::Type::INT == rightType)
        {
            return Expression::Type::INT;
        }

        // If both are STRING, result is STRING (for concatenation)
        if (Expression::Type::STRING == leftType && Expression::Type::STRING == rightType && BinaryExpression::Op::ADD == op)
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
        if (UnaryExpression::Op::NOT == unaryExpr->GetOperator())
        {
            return Expression::Type::BOOL;
        }
        // NEG operator returns same type as operand
        return InferExpressionType(unaryExpr->GetOperand(), classDecl);
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
    if (nullptr == fieldTypeSpec)
    {
        return false;
    }

    // User-defined types can't be validated this way
    if (fieldTypeSpec->IsUserDefined())
    {
        return true; // For now, assume user-defined types are OK
    }

    const PrimitiveTypeSpec* primType  = static_cast<const PrimitiveTypeSpec*>(fieldTypeSpec);
    Expression::Type         fieldType = PrimitiveNameToExpressionType(PrimitiveTypeSpec::TypeToString(primType->GetType()));

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

Expression::Type SemanticAnalyzer::PrimitiveNameToExpressionType(const std::string& typeName) const
{
    if ("Int" == typeName)
    {
        return Expression::Type::INT;
    }
    if ("Real" == typeName)
    {
        return Expression::Type::REAL;
    }
    if ("String" == typeName)
    {
        return Expression::Type::STRING;
    }
    if ("Bool" == typeName)
    {
        return Expression::Type::BOOL;
    }
    if ("Timestamp" == typeName)
    {
        return Expression::Type::TIMESTAMP;
    }
    if ("Timespan" == typeName)
    {
        return Expression::Type::TIMESPAN;
    }
    if ("Guid" == typeName)
    {
        return Expression::Type::GUID;
    }

    return Expression::Type::UNKNOWN;
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
                        std::cout << invariant->GetName() << ": ";
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
} // namespace bbfm
