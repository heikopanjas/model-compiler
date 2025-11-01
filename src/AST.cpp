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

Invariant::Invariant(const std::string& name, std::unique_ptr<Expression> expression) : name_(name), expression_(std::move(expression)) {}

const std::string& Invariant::GetName() const
{
    return name_;
}

const Expression* Invariant::GetExpression() const
{
    return expression_.get();
}

void Invariant::Dump(const int indent) const
{
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
            Type leftType  = left_->GetResultType();
            Type rightType = right_->GetResultType();

            // If either is REAL, result is REAL
            if (Type::REAL == leftType || Type::REAL == rightType || Type::TIMESTAMP == leftType || Type::TIMESTAMP == rightType ||
                Type::TIMESPAN == leftType || Type::TIMESPAN == rightType)
            {
                return Type::REAL;
            }

            // Otherwise, if both are INT, result is INT
            if (Type::INT == leftType && Type::INT == rightType)
            {
                return Type::INT;
            }

            return Type::UNKNOWN;
        }

        default:
            return Type::UNKNOWN;
    }
}

std::string BinaryExpression::ToString() const
{
    return "(" + left_->ToString() + " " + std::string(OpToString(op_)) + " " + right_->ToString() + ")";
}

void BinaryExpression::Dump(const int indent) const
{
    PrintIndent(indent);
    std::cout << "BinaryExpression [" << OpToString(op_) << "]\n";
    left_->Dump(indent + 1);
    right_->Dump(indent + 1);
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

std::string UnaryExpression::ToString() const
{
    return std::string(OpToString(op_)) + operand_->ToString();
}

void UnaryExpression::Dump(const int indent) const
{
    PrintIndent(indent);
    std::cout << "UnaryExpression [" << OpToString(op_) << "]\n";
    operand_->Dump(indent + 1);
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
FieldReference::FieldReference(const std::string& fieldName) : fieldName_(fieldName) {}

Expression::Type FieldReference::GetResultType() const
{
    // Type must be determined by semantic analyzer based on field type
    return Type::UNKNOWN;
}

std::string FieldReference::ToString() const
{
    return fieldName_;
}

void FieldReference::Dump(const int indent) const
{
    PrintIndent(indent);
    std::cout << "FieldReference: " << fieldName_ << "\n";
}

const std::string& FieldReference::GetFieldName() const
{
    return fieldName_;
}

// LiteralExpression
LiteralExpression::LiteralExpression(const int64_t value) : type_(Type::INT), intValue_(value), realValue_(0.0), boolValue_(false) {}

LiteralExpression::LiteralExpression(const double value) : type_(Type::REAL), intValue_(0), realValue_(value), boolValue_(false) {}

LiteralExpression::LiteralExpression(const std::string& value) : type_(Type::STRING), intValue_(0), realValue_(0.0), stringValue_(value), boolValue_(false) {}

LiteralExpression::LiteralExpression(const bool value) : type_(Type::BOOL), intValue_(0), realValue_(0.0), boolValue_(value) {}

Expression::Type LiteralExpression::GetResultType() const
{
    return type_;
}

std::string LiteralExpression::ToString() const
{
    switch (type_)
    {
        case Type::INT:
            return std::to_string(intValue_);
        case Type::REAL:
            return std::to_string(realValue_);
        case Type::STRING:
            return "\"" + stringValue_ + "\"";
        case Type::BOOL:
            return boolValue_ ? "true" : "false";
        default:
            return "?";
    }
}

void LiteralExpression::Dump(const int indent) const
{
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

const std::string& LiteralExpression::GetStringValue() const
{
    return stringValue_;
}

bool LiteralExpression::GetBoolValue() const
{
    return boolValue_;
}

// FunctionCall
FunctionCall::FunctionCall(const std::string& functionName, std::vector<std::unique_ptr<Expression>> arguments) :
    functionName_(functionName), arguments_(std::move(arguments))
{
}

Expression::Type FunctionCall::GetResultType() const
{
    // Type must be determined by semantic analyzer based on function signature
    return Type::UNKNOWN;
}

std::string FunctionCall::ToString() const
{
    std::string result = functionName_ + "(";
    for (size_t i = 0; i < arguments_.size(); ++i)
    {
        if (i > 0)
        {
            result += ", ";
        }
        result += arguments_[i]->ToString();
    }
    result += ")";
    return result;
}

void FunctionCall::Dump(const int indent) const
{
    PrintIndent(indent);
    std::cout << "FunctionCall: " << functionName_ << "\n";
    for (const auto& arg : arguments_)
    {
        arg->Dump(indent + 1);
    }
}

const std::string& FunctionCall::GetFunctionName() const
{
    return functionName_;
}

const std::vector<std::unique_ptr<Expression>>& FunctionCall::GetArguments() const
{
    return arguments_;
}

// ParenthesizedExpression
ParenthesizedExpression::ParenthesizedExpression(std::unique_ptr<Expression> expr) : expr_(std::move(expr)) {}

Expression::Type ParenthesizedExpression::GetResultType() const
{
    return expr_->GetResultType();
}

std::string ParenthesizedExpression::ToString() const
{
    return "(" + expr_->ToString() + ")";
}

void ParenthesizedExpression::Dump(const int indent) const
{
    PrintIndent(indent);
    std::cout << "ParenthesizedExpression\n";
    expr_->Dump(indent + 1);
}

const Expression* ParenthesizedExpression::GetExpression() const
{
    return expr_.get();
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
