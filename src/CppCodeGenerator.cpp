#include "CppCodeGenerator.h"
#include "Common.h"
#include "Console.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>

namespace bbfm {
// ============================================================================
// CppCodeGenerator Implementation
// ============================================================================

CppCodeGenerator::CppCodeGenerator(const AST* ast, const SemanticAnalyzer* analyzer, const std::vector<std::string>& namespaces, const std::string& classPrefix) :
    CodeGenerator(ast, analyzer, namespaces, classPrefix)
{
}

bool CppCodeGenerator::Generate(const std::string& outputPath)
{
    if (nullptr == ast_ || nullptr == analyzer_)
    {
        ReportError("Cannot generate code: AST or analyzer is null");
        return false;
    }

    // Clear output stream
    output_.str("");
    output_.clear();

    // Generate file header
    GenerateFileHeader(outputPath);

    // Generate all declarations
    const auto& declarations = ast_->GetDeclarations();
    for (const auto& decl : declarations)
    {
        if (Declaration::Kind::ENUM == decl->GetKind())
        {
            GenerateEnum(decl->AsEnum());
        }
        else if (Declaration::Kind::CLASS == decl->GetKind())
        {
            GenerateClass(decl->AsClass());
        }
    }

    // Generate file footer
    GenerateFileFooter();

    // Close include guard
    std::string includeGuard = GenerateIncludeGuardName(outputPath);
    output_ << "#endif // " << includeGuard << "\n";

    // Write to output file
    std::ofstream outFile(outputPath);
    if (false == outFile.is_open())
    {
        ReportError("Failed to open output file: " + outputPath);
        return false;
    }

    outFile << output_.str();
    outFile.close();

    if (false == outFile.good())
    {
        ReportError("Error writing to output file: " + outputPath);
        return false;
    }

    Console::ReportStatus("C++ code generated successfully: " + outputPath);
    return true;
}

std::string CppCodeGenerator::GetFileExtension() const
{
    return ".h";
}

std::string CppCodeGenerator::GetLanguageName() const
{
    return "C++";
}

std::string CppCodeGenerator::MapPrimitiveType(const PrimitiveType type) const
{
    // TODO: Implement in Step 7
    switch (type)
    {
        case PrimitiveType::STRING:
            return "bbfm::runtime::String";
        case PrimitiveType::INT:
            return "int64_t";
        case PrimitiveType::REAL:
            return "double";
        case PrimitiveType::BOOL:
            return "bool";
        case PrimitiveType::TIMESTAMP:
            return "double";
        case PrimitiveType::TIMESPAN:
            return "double";
        case PrimitiveType::DATE:
            return "bbfm::runtime::Date";
        case PrimitiveType::GUID:
            return "bbfm::runtime::Guid";
        default:
            return "unknown";
    }
}

std::string CppCodeGenerator::MapType(const TypeSpec* typeSpec) const
{
    // TODO: Implement fully in Step 7
    if (typeSpec->IsPrimitive())
    {
        const PrimitiveTypeSpec* primType = dynamic_cast<const PrimitiveTypeSpec*>(typeSpec);
        return MapPrimitiveType(primType->GetType());
    }
    else
    {
        const UserDefinedTypeSpec* userType = dynamic_cast<const UserDefinedTypeSpec*>(typeSpec);
        return FormatNamespacePrefix() + ApplyClassPrefix(userType->GetTypeName());
    }
}

std::string CppCodeGenerator::GenerateIncludeGuardName(const std::string& filename) const
{
    // Convert filename to include guard format
    // Example: "podcast.h" -> "__BBFM_GENERATED_PODCAST_H_INCL__"
    std::string guard = "__BBFM_GENERATED_";

    // Extract base filename without extension
    size_t lastSlash = filename.find_last_of("/\\");
    std::string baseName = (std::string::npos != lastSlash) ? filename.substr(lastSlash + 1) : filename;

    size_t lastDot = baseName.find_last_of('.');
    if (std::string::npos != lastDot)
    {
        baseName = baseName.substr(0, lastDot);
    }

    // Convert to uppercase and replace non-alphanumeric with underscore
    for (char c : baseName)
    {
        if (std::isalnum(c))
        {
            guard += static_cast<char>(std::toupper(c));
        }
        else
        {
            guard += '_';
        }
    }

    guard += "_H_INCL__";
    return guard;
}

void CppCodeGenerator::GenerateFileHeader(const std::string& filename)
{
    std::string includeGuard = GenerateIncludeGuardName(filename);

    // Include guard opening
    output_ << "#ifndef " << includeGuard << "\n";
    output_ << "#define " << includeGuard << "\n\n";

    // Alignment pragma
    output_ << "// Set 8-byte alignment for all types in this header\n";
    output_ << "#pragma pack(push, 8)\n\n";

    // Includes for runtime types
    output_ << "#include <cstdint>\n";
    output_ << "#include <vector>\n";
    output_ << "#include <optional>\n";
    output_ << "#include \"runtime/Fabric.h\"\n";
    output_ << "#include \"runtime/String.h\"\n";
    output_ << "#include \"runtime/Date.h\"\n";
    output_ << "#include \"runtime/Guid.h\"\n\n";

    // Open namespaces
    GenerateNamespaceOpen();
}

void CppCodeGenerator::GenerateFileFooter()
{
    // Close namespaces
    GenerateNamespaceClose();

    // Restore alignment
    output_ << "// Restore previous alignment\n";
    output_ << "#pragma pack(pop)\n\n";

    // Note: Include guard closing is added separately in Generate()
}

void CppCodeGenerator::GenerateNamespaceOpen()
{
    if (namespaces_.empty())
    {
        return;
    }

    // Generate nested namespace declarations
    for (const auto& ns : namespaces_)
    {
        output_ << "namespace " << ns << " {\n";
    }
    output_ << "\n";
}

void CppCodeGenerator::GenerateNamespaceClose()
{
    if (namespaces_.empty())
    {
        return;
    }

    // Close namespaces in reverse order
    output_ << "\n";
    for (auto it = namespaces_.rbegin(); it != namespaces_.rend(); ++it)
    {
        output_ << "} // namespace " << *it << "\n";
    }
}

void CppCodeGenerator::GenerateEnum(const EnumDeclaration* enumDecl)
{
    if (nullptr == enumDecl)
    {
        return;
    }

    // Generate enum class with prefix if specified
    std::string enumName = ApplyClassPrefix(enumDecl->GetName());
    const auto& values   = enumDecl->GetValues();

    output_ << "/// \\brief " << enumName << " enumeration\n";
    output_ << "enum class " << enumName << "\n";
    output_ << "{\n";

    // Generate enum values
    for (size_t i = 0; i < values.size(); ++i)
    {
        WriteIndent(1);
        output_ << values[i];

        if (i < values.size() - 1)
        {
            output_ << ",";
        }

        output_ << "\n";
    }

    output_ << "};\n\n";
}

void CppCodeGenerator::GenerateClass(const ClassDeclaration* classDecl)
{
    if (nullptr == classDecl)
    {
        return;
    }

    std::string className = ApplyClassPrefix(classDecl->GetName());

    // Generate class documentation
    output_ << "/// \\brief " << className << " class\n";

    // Generate class declaration
    output_ << "class " << className;

    // Determine inheritance
    const std::string& baseType = classDecl->GetBaseType();
    if (false == baseType.empty())
    {
        // User-defined base class (which already inherits from Fabric)
        std::string baseClassName = ApplyClassPrefix(baseType);
        output_ << " : public " << baseClassName;
    }
    else
    {
        // No user-defined base class, so inherit from Fabric
        output_ << " : public bbfm::runtime::Fabric";
    }

    output_ << "\n{\n";

    // Public section - getters and methods
    output_ << "public:\n";

    // Generate constructor
    WriteIndent(1);
    output_ << "/// \\brief Default constructor\n";
    WriteIndent(1);
    output_ << className << "();\n\n";

    // Generate virtual destructor
    WriteIndent(1);
    output_ << "/// \\brief Virtual destructor\n";
    WriteIndent(1);
    output_ << "virtual ~" << className << "() = default;\n\n";

    // Note: Universal metadata getters (GetTypeId, GetId, etc.) are inherited from Fabric

    // Generate getter methods for all fields
    const auto& fields = classDecl->GetFields();
    for (const auto& field : fields)
    {
        if (field->IsComputed())
        {
            GenerateComputedGetter(field.get());
        }
        else
        {
            GenerateGetter(field.get());
        }
    }

    // Generate invariant validation methods
    GenerateInvariantMethods(classDecl);

    // Private section (member variables)
    // Note: Universal metadata fields are inherited from Fabric base class
    // Note: If invariants were generated, they already added "private:" section
    // So we need to check if we need to add it here
    const auto& invariants = classDecl->GetInvariants();
    if (invariants.empty())
    {
        output_ << "private:\n";
    }

    // Generate user-defined fields only (universal metadata inherited from Fabric)
    GenerateClassFields(classDecl);

    output_ << "};\n\n";
}

void CppCodeGenerator::GenerateClassFields(const ClassDeclaration* classDecl)
{
    if (nullptr == classDecl)
    {
        return;
    }

    const auto& fields = classDecl->GetFields();

    if (fields.empty())
    {
        // No user-defined fields (universal metadata inherited from Fabric)
        return;
    }

    // Add comment separator for user-defined fields
    WriteIndent(1);
    output_ << "// User-defined fields\n";

    for (const auto& field : fields)
    {
        // Get the base type
        std::string cppType = MapType(field->GetType());

        // Get cardinality modifier
        const CardinalityModifier* cardMod = field->GetCardinalityModifier();

        // Determine if array or optional
        bool isArray    = (nullptr != cardMod) && cardMod->IsArray();
        bool isOptional = (nullptr != cardMod) && cardMod->IsOptional() && !isArray;

        // Wrap type for arrays or optional
        if (isArray)
        {
            cppType = "std::vector<" + cppType + ">";
        }
        else if (isOptional)
        {
            cppType = "std::optional<" + cppType + ">";
        }

        // Generate field declaration with underscore postfix
        WriteIndent(1);
        output_ << cppType << " " << field->GetName() << "_";

        // Add comment for computed features
        if (field->IsComputed())
        {
            output_ << "; // Computed feature\n";
        }
        // Add comment for unique constraints
        else if (field->HasUniqueConstraint())
        {
            output_ << "; // Unique constraint\n";
        }
        else
        {
            output_ << ";\n";
        }
    }
}

void CppCodeGenerator::GenerateGetter(const Field* field)
{
    if (nullptr == field)
    {
        return;
    }

    // Get the C++ type
    std::string cppType = MapType(field->GetType());

    // Get cardinality
    const CardinalityModifier* cardMod = field->GetCardinalityModifier();
    bool isArray    = (nullptr != cardMod) && cardMod->IsArray();
    bool isOptional = (nullptr != cardMod) && cardMod->IsOptional() && !isArray;

    // Wrap type if needed
    std::string fullType = cppType;
    if (isArray)
    {
        fullType = "std::vector<" + cppType + ">";
    }
    else if (isOptional)
    {
        fullType = "std::optional<" + cppType + ">";
    }

    // Generate getter method
    std::string getterName = "Get" + field->GetName();

    // Capitalize first letter of getter name
    if (false == getterName.empty())
    {
        getterName[3] = static_cast<char>(std::toupper(getterName[3]));
    }

    WriteIndent(1);
    output_ << "/// \\brief Get the " << field->GetName() << " field\n";
    WriteIndent(1);
    output_ << "/// \\return ";

    // Determine return type description
    if (isArray)
    {
        output_ << "Reference to the " << field->GetName() << " vector\n";
    }
    else if (isOptional)
    {
        output_ << "Reference to the optional " << field->GetName() << "\n";
    }
    else
    {
        output_ << "The " << field->GetName() << " value\n";
    }

    WriteIndent(1);

    // Return by const reference for complex types, by value for primitives
    bool isPrimitive = field->GetType()->IsPrimitive();
    bool returnByValue = isPrimitive && !isArray && !isOptional;

    // Exception: return custom runtime types by reference too
    if (isPrimitive)
    {
        const PrimitiveTypeSpec* primType = dynamic_cast<const PrimitiveTypeSpec*>(field->GetType());
        PrimitiveType pt = primType->GetType();
        if (PrimitiveType::STRING == pt || PrimitiveType::DATE == pt || PrimitiveType::GUID == pt)
        {
            returnByValue = false; // Return by reference for runtime types
        }
    }

    if (returnByValue)
    {
        output_ << fullType << " " << getterName << "() const;\n";
    }
    else
    {
        output_ << "const " << fullType << "& " << getterName << "() const;\n";
    }

    output_ << "\n";
}

void CppCodeGenerator::GenerateComputedGetter(const Field* field)
{
    if (nullptr == field || !field->IsComputed())
    {
        return;
    }

    // Get the C++ type (computed features are always mandatory [1])
    std::string cppType = MapType(field->GetType());

    // Generate getter method
    std::string getterName = "Get" + field->GetName();

    // Capitalize first letter of getter name
    if (false == getterName.empty())
    {
        getterName[3] = static_cast<char>(std::toupper(getterName[3]));
    }

    WriteIndent(1);
    output_ << "/// \\brief Get the computed " << field->GetName() << " field\n";
    WriteIndent(1);
    output_ << "/// \\return The computed " << field->GetName() << " value\n";
    WriteIndent(1);

    // Computed features return by value for primitives, by const reference for complex types
    bool isPrimitive = field->GetType()->IsPrimitive();
    bool returnByValue = isPrimitive;

    // Check if it's a custom runtime type
    if (isPrimitive)
    {
        const PrimitiveTypeSpec* primType = dynamic_cast<const PrimitiveTypeSpec*>(field->GetType());
        PrimitiveType pt = primType->GetType();
        if (PrimitiveType::STRING == pt || PrimitiveType::DATE == pt || PrimitiveType::GUID == pt)
        {
            returnByValue = false;
        }
    }

    if (returnByValue)
    {
        output_ << cppType << " " << getterName << "() const";
    }
    else
    {
        output_ << "const " << cppType << "& " << getterName << "() const";
    }

    // Inline implementation with expression (will be completed in Step 11)
    output_ << "\n";
    WriteIndent(1);
    output_ << "{\n";
    WriteIndent(2);
    output_ << "return ";

    // Convert expression to C++ (Step 11)
    if (nullptr != field->GetInitializer())
    {
        output_ << ExpressionToCpp(field->GetInitializer());
    }
    else
    {
        output_ << "/* TODO: expression */";
    }

    output_ << ";\n";
    WriteIndent(1);
    output_ << "}\n\n";
}

std::string CppCodeGenerator::ExpressionToCpp(const Expression* expr) const
{
    if (nullptr == expr)
    {
        return "";
    }

    // Handle binary expressions (arithmetic, comparison, logical)
    if (const BinaryExpression* binExpr = dynamic_cast<const BinaryExpression*>(expr))
    {
        std::string left  = ExpressionToCpp(binExpr->GetLeft());
        std::string right = ExpressionToCpp(binExpr->GetRight());
        std::string op;

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
            default:
                op = " ? ";
                break;
        }

        return left + op + right;
    }

    // Handle unary expressions (negation, logical not)
    if (const UnaryExpression* unaryExpr = dynamic_cast<const UnaryExpression*>(expr))
    {
        std::string operand = ExpressionToCpp(unaryExpr->GetOperand());
        std::string op;

        switch (unaryExpr->GetOperator())
        {
            case UnaryExpression::Op::NEG:
                op = "-";
                break;
            case UnaryExpression::Op::NOT:
                op = "!";
                break;
            default:
                op = "?";
                break;
        }

        return op + operand;
    }

    // Handle field references
    if (const FieldReference* fieldRef = dynamic_cast<const FieldReference*>(expr))
    {
        // Field references become member variable access with underscore postfix
        return fieldRef->GetFieldName() + "_";
    }

    // Handle member access (object.field)
    if (const MemberAccessExpression* memberAccess = dynamic_cast<const MemberAccessExpression*>(expr))
    {
        std::string object = ExpressionToCpp(memberAccess->GetObject());
        std::string member = memberAccess->GetMemberName();

        // Convert member access to getter call
        // Capitalize first letter for getter name
        std::string getterName = "Get" + member;
        if (false == getterName.empty() && getterName.length() > 3)
        {
            getterName[3] = static_cast<char>(std::toupper(getterName[3]));
        }

        return object + "." + getterName + "()";
    }

    // Handle literals
    if (const LiteralExpression* literal = dynamic_cast<const LiteralExpression*>(expr))
    {
        return literal->ToString();
    }

    // Handle parenthesized expressions
    if (const ParenthesizedExpression* parenExpr = dynamic_cast<const ParenthesizedExpression*>(expr))
    {
        return "(" + ExpressionToCpp(parenExpr->GetExpression()) + ")";
    }

    // Unknown expression type
    return "/* unknown expression */";
}

void CppCodeGenerator::GenerateInvariantMethods(const ClassDeclaration* classDecl)
{
    if (nullptr == classDecl)
    {
        return;
    }

    const auto& invariants = classDecl->GetInvariants();

    if (invariants.empty())
    {
        return;
    }

    // Generate public Validate() method
    WriteIndent(1);
    output_ << "/// \\brief Validate all invariants for this class\n";
    WriteIndent(1);
    output_ << "/// \\return True if all invariants are satisfied, false otherwise\n";
    WriteIndent(1);
    output_ << "bool Validate() const\n";
    WriteIndent(1);
    output_ << "{\n";

    // Call each invariant check
    for (const auto& invariant : invariants)
    {
        WriteIndent(2);
        output_ << "if (false == Check" << invariant->GetName() << "())\n";
        WriteIndent(2);
        output_ << "{\n";
        WriteIndent(3);
        output_ << "return false;\n";
        WriteIndent(2);
        output_ << "}\n";
    }

    WriteIndent(2);
    output_ << "return true;\n";
    WriteIndent(1);
    output_ << "}\n\n";

    // Generate private invariant check methods
    output_ << "private:\n";

    for (const auto& invariant : invariants)
    {
        WriteIndent(1);
        output_ << "/// \\brief Check invariant: " << invariant->GetName() << "\n";
        WriteIndent(1);
        output_ << "/// \\return True if invariant is satisfied\n";
        WriteIndent(1);
        output_ << "bool Check" << invariant->GetName() << "() const\n";
        WriteIndent(1);
        output_ << "{\n";
        WriteIndent(2);
        output_ << "return ";

        if (nullptr != invariant->GetExpression())
        {
            output_ << ExpressionToCpp(invariant->GetExpression());
        }
        else
        {
            output_ << "true";
        }

        output_ << ";\n";
        WriteIndent(1);
        output_ << "}\n\n";
    }

    // Return to private section for fields
    output_ << "private:\n";
}

void CppCodeGenerator::GenerateUniversalMetadata(const ClassDeclaration* classDecl)
{
    // Universal metadata fields are now inherited from bbfm::runtime::Fabric base class
    // This method is kept for future extensions but currently does nothing
    UNREFERENCED_PARAMETER(classDecl);
}

std::string CppCodeGenerator::ApplyClassPrefix(const std::string& name) const
{
    if (classPrefix_.empty())
    {
        return name;
    }
    return classPrefix_ + name;
}

void CppCodeGenerator::WriteIndent(const int level)
{
    for (int i = 0; i < level; ++i)
    {
        output_ << "    "; // 4 spaces per indentation level
    }
}
} // namespace bbfm
