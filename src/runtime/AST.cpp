#include "runtime/AST.h"
#include "Common.h"
#include "runtime/Array.h"
#include "runtime/Dictionary.h"
#include "runtime/String.h"
#include <iostream>
#include <string>

namespace runtime {
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

void PrimitiveTypeSpec::Dump(const int indent, const String& nsPrefix) const
{
    UNREFERENCED_PARAMETER(indent);
    UNREFERENCED_PARAMETER(nsPrefix);
    std::cout << "<builtin>::" << TypeToString(type_);
}

// ============================================================================
// UserDefinedTypeSpec Implementation
// ============================================================================

const String& UserDefinedTypeSpec::GetTypeName() const
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

void UserDefinedTypeSpec::Dump(const int indent, const String& nsPrefix) const
{
    UNREFERENCED_PARAMETER(indent);
    std::cout << nsPrefix << typeName_;
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

void CardinalityModifier::Dump(const int indent, const String& nsPrefix) const
{
    UNREFERENCED_PARAMETER(indent);
    UNREFERENCED_PARAMETER(nsPrefix);
    std::cout << "[" << minCardinality_;
    if (-1 == maxCardinality_)
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

void UniqueModifier::Dump(const int indent, const String& nsPrefix) const
{
    UNREFERENCED_PARAMETER(indent);
    UNREFERENCED_PARAMETER(nsPrefix);
    std::cout << "[unique]";
}

// ============================================================================
// Field Implementation
// ============================================================================

const TypeSpec* Field::GetType() const
{
    return type_.get();
}

const String& Field::GetName() const
{
    return name_;
}

const Array<std::unique_ptr<Modifier>>& Field::GetModifiers() const
{
    return modifiers_;
}

bool Field::IsStatic() const
{
    return isStatic_;
}

bool Field::IsComputed() const
{
    return nullptr != initializer_ && false == isAlias_;
}

bool Field::IsAlias() const
{
    return isAlias_;
}

const Expression* Field::GetInitializer() const
{
    return initializer_.get();
}

const CardinalityModifier* Field::GetCardinalityModifier() const
{
    for (size_t i = 0; i < modifiers_.GetCount(); ++i)
    {
        if (modifiers_.At(i)->GetType() == ModifierType::CARDINALITY)
        {
            return static_cast<const CardinalityModifier*>(modifiers_.At(i).get());
        }
    }
    return nullptr;
}

bool Field::HasUniqueConstraint() const
{
    for (size_t i = 0; i < modifiers_.GetCount(); ++i)
    {
        if (modifiers_.At(i)->GetType() == ModifierType::UNIQUE)
        {
            return true;
        }
    }
    return false;
}

void Field::Dump(const int indent, const String& nsPrefix) const
{
    PrintIndent(indent);
    if (isStatic_)
    {
        std::cout << "static ";
    }

    // Indicate if this is an alias
    if (isAlias_)
    {
        std::cout << "alias " << name_ << " = ";
        if (nullptr != initializer_)
        {
            std::cout << initializer_->ToString();
        }
        else
        {
            std::cout << "(null)";
        }
        std::cout << ";\n";
        return;
    }

    std::cout << "feature " << name_ << ": ";
    if (nullptr != type_)
    {
        type_->Dump(0, nsPrefix);
    }
    else
    {
        std::cout << "(type not specified)";
    }

    // Print modifiers
    for (size_t i = 0; i < modifiers_.GetCount(); ++i)
    {
        std::cout << " ";
        modifiers_.At(i)->Dump(0, nsPrefix);
    }

    // Print initializer if present
    if (nullptr != initializer_)
    {
        std::cout << " = " << initializer_->ToString();
    }

    std::cout << ";\n";
}

// ============================================================================
// Invariant Implementation
// ============================================================================

Invariant::Invariant(const String& name, std::unique_ptr<Expression> expression) : name_(name), expression_(std::move(expression)) {}

const String& Invariant::GetName() const
{
    return name_;
}

const Expression* Invariant::GetExpression() const
{
    return expression_.get();
}

void Invariant::Dump(const int indent, const String& nsPrefix) const
{
    UNREFERENCED_PARAMETER(nsPrefix);
    PrintIndent(indent);
    std::cout << "invariant " << name_ << ": ";
    if (nullptr != expression_)
    {
        std::cout << expression_->ToString();
    }
    std::cout << ";\n";
}

// ============================================================================
// EnumDeclaration Implementation
// ============================================================================

const String& EnumDeclaration::GetName() const
{
    return name_;
}

const Array<String>& EnumDeclaration::GetValues() const
{
    return values_;
}

void EnumDeclaration::Dump(const int indent, const String& nsPrefix) const
{
    PrintIndent(indent);
    std::cout << "enum " << nsPrefix << name_ << " {\n";

    for (size_t i = 0; i < values_.GetCount(); ++i)
    {
        PrintIndent(indent + 1);
        std::cout << values_.GetValueAt(i);
        if (i < values_.GetCount() - 1)
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

const String& ClassDeclaration::GetName() const
{
    return name_;
}

const String& ClassDeclaration::GetBaseType() const
{
    return baseType_;
}

bool ClassDeclaration::HasExplicitBase() const
{
    return false == baseType_.IsEmpty();
}

const Array<std::unique_ptr<Field>>& ClassDeclaration::GetFields() const
{
    return fields_;
}

const Array<std::unique_ptr<Invariant>>& ClassDeclaration::GetInvariants() const
{
    return invariants_;
}

void ClassDeclaration::Dump(const int indent, const String& nsPrefix) const
{
    PrintIndent(indent);
    std::cout << "class " << nsPrefix << name_;

    if (false == baseType_.IsEmpty())
    {
        std::cout << " inherits " << nsPrefix << baseType_;
    }

    std::cout << " {\n";

    for (size_t i = 0; i < fields_.GetCount(); ++i)
    {
        fields_.At(i)->Dump(indent + 1, nsPrefix);
    }

    for (size_t i = 0; i < invariants_.GetCount(); ++i)
    {
        invariants_.At(i)->Dump(indent + 1, nsPrefix);
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

void Declaration::Dump(const int indent, const String& nsPrefix) const
{
    declaration_->Dump(indent, nsPrefix);
}

// ============================================================================
// Expression System Implementation
// ============================================================================

// BinaryExpression
BinaryExpression::BinaryExpression(std::unique_ptr<Expression> left, const Op op, std::unique_ptr<Expression> right) :
    left_(std::move(left)), right_(std::move(right)), op_(op)
{
}

Expression::Type BinaryExpression::GetResultType() const
{
    // For arithmetic operators, result type depends on operands
    // For comparison operators, result is always BOOL
    // For logical operators, result is always BOOL

    switch (op_)
    {
        case Op::LT:
        case Op::GT:
        case Op::LE:
        case Op::GE:
        case Op::EQ:
        case Op::NE:
        case Op::AND:
        case Op::OR:
            return Type::BOOL;

        case Op::ADD:
        case Op::SUB:
        case Op::MUL:
        case Op::DIV:
        case Op::MOD:
        {
            // Result type is the "wider" of the two operands
            const Type leftType  = left_->GetResultType();
            const Type rightType = right_->GetResultType();

            // If either is REAL, result is REAL
            if (leftType == Type::REAL || rightType == Type::REAL || leftType == Type::TIMESTAMP || rightType == Type::TIMESTAMP ||
                leftType == Type::TIMESPAN || rightType == Type::TIMESPAN)
            {
                return Type::REAL;
            }

            // Otherwise, if both are INT, result is INT
            if (leftType == Type::INT && rightType == Type::INT)
            {
                return Type::INT;
            }

            return Type::UNKNOWN;
        }

        default:
            return Type::UNKNOWN;
    }
}

String BinaryExpression::ToString() const
{
    return String("(") + left_->ToString() + String(" ") + String(OpToString(op_)) + String(" ") + right_->ToString() + String(")");
}

void BinaryExpression::Dump(const int indent, const String& nsPrefix) const
{
    PrintIndent(indent);
    std::cout << "BinaryExpression [" << OpToString(op_) << "]\n";
    left_->Dump(indent + 1, nsPrefix);
    right_->Dump(indent + 1, nsPrefix);
}

const Expression* BinaryExpression::GetLeft() const
{
    return left_.get();
}

const Expression* BinaryExpression::GetRight() const
{
    return right_.get();
}

BinaryExpression::Op BinaryExpression::GetOperator() const
{
    return op_;
}

const char* BinaryExpression::OpToString(const Op op)
{
    switch (op)
    {
        case Op::ADD:
            return "+";
        case Op::SUB:
            return "-";
        case Op::MUL:
            return "*";
        case Op::DIV:
            return "/";
        case Op::MOD:
            return "%";
        case Op::LT:
            return "<";
        case Op::GT:
            return ">";
        case Op::LE:
            return "<=";
        case Op::GE:
            return ">=";
        case Op::EQ:
            return "==";
        case Op::NE:
            return "!=";
        case Op::AND:
            return "&&";
        case Op::OR:
            return "||";
        default:
            return "?";
    }
}

// UnaryExpression
UnaryExpression::UnaryExpression(const Op op, std::unique_ptr<Expression> operand) : op_(op), operand_(std::move(operand)) {}

Expression::Type UnaryExpression::GetResultType() const
{
    switch (op_)
    {
        case Op::NEG:
            // Negation preserves the numeric type
            return operand_->GetResultType();
        case Op::NOT:
            // Logical NOT always returns BOOL
            return Type::BOOL;
        default:
            return Type::UNKNOWN;
    }
}

String UnaryExpression::ToString() const
{
    return String(OpToString(op_)) + operand_->ToString();
}

void UnaryExpression::Dump(const int indent, const String& nsPrefix) const
{
    PrintIndent(indent);
    std::cout << "UnaryExpression [" << OpToString(op_) << "]\n";
    operand_->Dump(indent + 1, nsPrefix);
}

const Expression* UnaryExpression::GetOperand() const
{
    return operand_.get();
}

UnaryExpression::Op UnaryExpression::GetOperator() const
{
    return op_;
}

const char* UnaryExpression::OpToString(const Op op)
{
    switch (op)
    {
        case Op::NEG:
            return "-";
        case Op::NOT:
            return "!";
        default:
            return "?";
    }
}

// FieldReference
FieldReference::FieldReference(const String& fieldName) : fieldName_(fieldName) {}

Expression::Type FieldReference::GetResultType() const
{
    // Type must be determined by semantic analyzer based on field type
    return Type::UNKNOWN;
}

String FieldReference::ToString() const
{
    return fieldName_;
}

void FieldReference::Dump(const int indent, const String& nsPrefix) const
{
    UNREFERENCED_PARAMETER(nsPrefix);
    PrintIndent(indent);
    std::cout << "FieldReference: " << fieldName_ << "\n";
}

const String& FieldReference::GetFieldName() const
{
    return fieldName_;
}

// MemberAccessExpression
MemberAccessExpression::MemberAccessExpression(std::unique_ptr<Expression> object, const String& memberName) :
    object_(std::move(object)), memberName_(memberName)
{
}

Expression::Type MemberAccessExpression::GetResultType() const
{
    // Type must be determined by semantic analyzer based on member type
    return Type::UNKNOWN;
}

String MemberAccessExpression::ToString() const
{
    return object_->ToString() + String(".") + memberName_;
}

void MemberAccessExpression::Dump(const int indent, const String& nsPrefix) const
{
    PrintIndent(indent);
    std::cout << "MemberAccess: ." << memberName_ << "\n";
    object_->Dump(indent + 2, nsPrefix);
}

const Expression* MemberAccessExpression::GetObject() const
{
    return object_.get();
}

const String& MemberAccessExpression::GetMemberName() const
{
    return memberName_;
}

// LiteralExpression
LiteralExpression::LiteralExpression(const int64_t value) : type_(Type::INT), intValue_(value), realValue_(0.0), boolValue_(false) {}

LiteralExpression::LiteralExpression(const double value) : type_(Type::REAL), intValue_(0), realValue_(value), boolValue_(false) {}

LiteralExpression::LiteralExpression(const String& value) : type_(Type::STRING), intValue_(0), realValue_(0.0), stringValue_(value), boolValue_(false) {}

LiteralExpression::LiteralExpression(const bool value) : type_(Type::BOOL), intValue_(0), realValue_(0.0), boolValue_(value) {}

Expression::Type LiteralExpression::GetResultType() const
{
    return type_;
}

String LiteralExpression::ToString() const
{
    switch (type_)
    {
        case Type::INT:
            return String(std::to_string(intValue_));
        case Type::REAL:
            return String(std::to_string(realValue_));
        case Type::STRING:
            return "\"" + stringValue_ + "\"";
        case Type::BOOL:
            return boolValue_ ? "true" : "false";
        default:
            return "?";
    }
}

void LiteralExpression::Dump(const int indent, const String& nsPrefix) const
{
    UNREFERENCED_PARAMETER(nsPrefix);
    PrintIndent(indent);
    std::cout << "Literal: " << ToString() << "\n";
}

int64_t LiteralExpression::GetIntValue() const
{
    return intValue_;
}

double LiteralExpression::GetRealValue() const
{
    return realValue_;
}

const String& LiteralExpression::GetStringValue() const
{
    return stringValue_;
}

bool LiteralExpression::GetBoolValue() const
{
    return boolValue_;
}

// FunctionCall
FunctionCall::FunctionCall(const String& functionName, Array<std::unique_ptr<Expression>> arguments) :
    functionName_(functionName), arguments_(std::move(arguments))
{
}

Expression::Type FunctionCall::GetResultType() const
{
    // Type must be determined by semantic analyzer based on function signature
    return Type::UNKNOWN;
}

String FunctionCall::ToString() const
{
    String result = functionName_ + String("(");
    for (size_t i = 0; i < arguments_.GetCount(); ++i)
    {
        if (i > 0)
        {
            result = result + String(", ");
        }
        result = result + arguments_.At(i)->ToString();
    }
    result = result + String(")");
    return result;
}

void FunctionCall::Dump(const int indent, const String& nsPrefix) const
{
    PrintIndent(indent);
    std::cout << "FunctionCall: " << functionName_ << "\n";
    for (size_t i = 0; i < arguments_.GetCount(); ++i)
    {
        arguments_.At(i)->Dump(indent + 1, nsPrefix);
    }
}

const String& FunctionCall::GetFunctionName() const
{
    return functionName_;
}

const Array<std::unique_ptr<Expression>>& FunctionCall::GetArguments() const
{
    return arguments_;
}

// ParenthesizedExpression
ParenthesizedExpression::ParenthesizedExpression(std::unique_ptr<Expression> expr) : expr_(std::move(expr)) {}

Expression::Type ParenthesizedExpression::GetResultType() const
{
    return expr_->GetResultType();
}

String ParenthesizedExpression::ToString() const
{
    return String("(") + expr_->ToString() + String(")");
}

void ParenthesizedExpression::Dump(const int indent, const String& nsPrefix) const
{
    PrintIndent(indent);
    std::cout << "ParenthesizedExpression\n";
    expr_->Dump(indent + 1, nsPrefix);
}

const Expression* ParenthesizedExpression::GetExpression() const
{
    return expr_.get();
}

// ============================================================================
// AST Implementation
// ============================================================================

AST::AST(const String& sourceNamespace, Array<std::unique_ptr<Declaration>> declarations) :
    sourceNamespace_(sourceNamespace), declarations_(std::move(declarations))
{
}

const String& AST::GetSourceNamespace() const
{
    return sourceNamespace_;
}

const Array<std::unique_ptr<Declaration>>& AST::GetDeclarations() const
{
    return declarations_;
}

void AST::Dump(const int indent, const String& nsPrefix) const
{
    PrintIndent(indent);
    std::cout << "=== Program AST ===\n";

    if (false == sourceNamespace_.IsEmpty())
    {
        PrintIndent(indent);
        std::cout << "Namespace: " << sourceNamespace_ << "\n";
    }

    std::cout << "\n";

    for (size_t i = 0; i < declarations_.GetCount(); ++i)
    {
        declarations_.At(i)->Dump(indent, nsPrefix);
        std::cout << "\n";
    }

    PrintIndent(indent);
    std::cout << "=== End of AST ===\n";
}
} // namespace runtime
