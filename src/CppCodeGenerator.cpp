#include "CppCodeGenerator.h"
#include "Common.h"
#include "Console.h"
#include "SemanticAnalyzer.h"
#include <algorithm>
#include <cctype>
#include <chrono>
#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>

namespace bbfm {
// ============================================================================
// CppCodeGenerator Implementation
// ============================================================================

CppCodeGenerator::CppCodeGenerator(
    const AST* ast, const SemanticAnalyzer* analyzer, const std::vector<std::string>& namespaces, const std::string& classPrefix) :
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

const ClassDeclaration* CppCodeGenerator::FindClassDeclaringField(const ClassDeclaration* classDecl, const std::string& fieldName) const
{
    if (nullptr == classDecl)
    {
        return nullptr;
    }

    // Check if this class declares the field
    for (const auto& field : classDecl->GetFields())
    {
        if (field->GetName() == fieldName)
        {
            return classDecl;
        }
    }

    // Check base class
    if (classDecl->HasExplicitBase())
    {
        const auto& symbolTable = analyzer_->GetSymbolTable();
        auto        it          = symbolTable.find(classDecl->GetBaseType());
        if (it != symbolTable.end() && nullptr != it->second.classDecl)
        {
            return FindClassDeclaringField(it->second.classDecl, fieldName);
        }
    }

    return nullptr;
}

std::string CppCodeGenerator::GetFieldWrapperType(const Field* field, const ClassDeclaration* classDecl) const
{
    if (nullptr == field || field->IsComputed() || field->IsAlias())
    {
        return "void";
    }

    std::string cppType   = MapType(field->GetType());
    std::string className = ApplyClassPrefix(classDecl->GetName());

    // Get cardinality
    const CardinalityModifier* cardMod    = field->GetCardinalityModifier();
    bool                       isOptional = (nullptr != cardMod) && cardMod->IsOptional();

    // Check if field has invariants
    std::vector<const Invariant*> fieldInvariants = GetInvariantsForField(field->GetName(), classDecl);
    bool                          hasInvariants   = !fieldInvariants.empty();

    // Build template parameters
    std::string templateParams = cppType + ", " + className;

    // Add checker functions if there are invariants
    if (hasInvariants)
    {
        for (const Invariant* inv : fieldInvariants)
        {
            templateParams += ", &" + className + "::Check" + inv->GetName() + "_" + field->GetName();
        }
    }

    // Choose wrapper based on invariants and optionality
    std::string wrapperType;
    if (hasInvariants && isOptional)
    {
        wrapperType = "bbfm::runtime::OptionalBoundedValue";
    }
    else if (hasInvariants && !isOptional)
    {
        wrapperType = "bbfm::runtime::BoundedValue";
    }
    else if (!hasInvariants && isOptional)
    {
        wrapperType = "bbfm::runtime::OptionalUnboundedValue";
    }
    else
    {
        wrapperType = "bbfm::runtime::UnboundedValue";
    }

    return wrapperType + "<" + templateParams + ">";
}

std::string CppCodeGenerator::GenerateIncludeGuardName(const std::string& filename) const
{
    // Convert filename to include guard format
    // Example: "podcast.h" -> "__BBFM_GENERATED_PODCAST_H_INCL__"
    std::string guard = "__BBFM_GENERATED_";

    // Extract base filename without extension
    size_t      lastSlash = filename.find_last_of("/\\");
    std::string baseName  = (std::string::npos != lastSlash) ? filename.substr(lastSlash + 1) : filename;

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
    output_ << "#include <stdexcept>\n";
    output_ << "#include <vector>\n";
    output_ << "#include <optional>\n";
    output_ << "#include <functional>\n";
    output_ << "#include \"runtime/Fabric.h\"\n";
    output_ << "#include \"runtime/BoundedValue.h\"\n";
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

    // Generate struct documentation
    output_ << "/// \\brief " << className << " struct\n";

    // Generate struct declaration (public by default)
    output_ << "struct " << className;

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

    // Generate constructor implementation
    GenerateConstructorImplementation(classDecl);

    // Generate virtual destructor
    WriteIndent(1);
    output_ << "/// \\brief Virtual destructor\n";
    WriteIndent(1);
    output_ << "virtual ~" << className << "() = default;\n\n";

    // Note: Universal metadata getters (GetTypeId, GetId, etc.) are inherited from Fabric

    // Generate user-defined fields (including computed fields as DynamicValue wrappers)
    GenerateClassFields(classDecl);

    // Generate static checker functions for invariants
    GenerateStaticCheckerFunctions(classDecl);

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
        // Handle alias fields separately with AliasValue wrapper
        if (field->IsAlias())
        {
            // Get the target field name from the FieldReference expression
            const Expression*     expr     = field->GetInitializer();
            const FieldReference* fieldRef = dynamic_cast<const FieldReference*>(expr);
            if (nullptr != fieldRef)
            {
                std::string targetFieldName = fieldRef->GetFieldName();
                std::string fieldName       = field->GetName();

                // Find the target field to determine its wrapper type
                const Field* targetField = analyzer_->FindFieldInClass(classDecl, targetFieldName);
                if (nullptr != targetField)
                {
                    // Find which class actually declares this field (could be base class)
                    const ClassDeclaration* targetFieldClass = FindClassDeclaringField(classDecl, targetFieldName);
                    if (nullptr != targetFieldClass)
                    {
                        std::string targetWrapperType = GetFieldWrapperType(targetField, targetFieldClass);

                        // Use AliasValue wrapper that forwards to target field
                        WriteIndent(1);
                        output_ << "bbfm::runtime::AliasValue<" << targetWrapperType << "> " << fieldName << "_;\n";
                    }
                }
            }
            continue;
        }

        // Handle computed fields separately with DynamicValue wrapper
        if (field->IsComputed())
        {
            std::string cppType   = MapType(field->GetType());
            std::string className = ApplyClassPrefix(classDecl->GetName());
            std::string fieldName = field->GetName();

            // Use DynamicValue wrapper for computed fields
            WriteIndent(1);
            output_ << "bbfm::runtime::DynamicValue<" << cppType << ", " << className << "> " << fieldName << "_;\n";
            continue;
        }

        // Get the base type
        std::string cppType = MapType(field->GetType());

        // Get cardinality modifier
        const CardinalityModifier* cardMod = field->GetCardinalityModifier();

        // Determine if array or optional
        bool isArray    = (nullptr != cardMod) && cardMod->IsArray();
        bool isOptional = (nullptr != cardMod) && cardMod->IsOptional() && !isArray;

        // Arrays use std::vector (no wrapper)
        if (isArray)
        {
            cppType = "std::vector<" + cppType + ">";
        }
        else
        {
            // Determine wrapper type for non-array fields
            // Check if field has invariants
            std::vector<const Invariant*> fieldInvariants = GetInvariantsForField(field->GetName(), classDecl);
            bool                          hasInvariants   = !fieldInvariants.empty();

            std::string className = ApplyClassPrefix(classDecl->GetName());
            std::string fieldName = field->GetName();

            // Build template parameters
            std::string templateParams = cppType; // Base type T

            // Add ParentT parameter
            templateParams += ", " + className;

            // Add checker functions if there are invariants
            if (hasInvariants)
            {
                for (const Invariant* inv : fieldInvariants)
                {
                    templateParams += ", &" + className + "::Check" + inv->GetName() + "_" + fieldName;
                }
            }

            // Choose wrapper based on invariants and optionality
            std::string wrapperType;
            if (hasInvariants && isOptional)
            {
                wrapperType = "bbfm::runtime::OptionalBoundedValue";
            }
            else if (hasInvariants && !isOptional)
            {
                wrapperType = "bbfm::runtime::BoundedValue";
            }
            else if (!hasInvariants && isOptional)
            {
                wrapperType = "bbfm::runtime::OptionalUnboundedValue";
            }
            else
            {
                wrapperType = "bbfm::runtime::UnboundedValue";
            }

            cppType = wrapperType + "<" + templateParams + ">";
        }

        // Generate field declaration with underscore postfix
        WriteIndent(1);
        output_ << cppType << " " << field->GetName() << "_";

        // Add comment for unique constraints
        if (field->HasUniqueConstraint())
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
    const CardinalityModifier* cardMod    = field->GetCardinalityModifier();
    bool                       isArray    = (nullptr != cardMod) && cardMod->IsArray();
    bool                       isOptional = (nullptr != cardMod) && cardMod->IsOptional() && !isArray;

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
    bool isPrimitive   = field->GetType()->IsPrimitive();
    bool returnByValue = isPrimitive && !isArray && !isOptional;

    // Exception: return custom runtime types by reference too
    if (isPrimitive)
    {
        const PrimitiveTypeSpec* primType = dynamic_cast<const PrimitiveTypeSpec*>(field->GetType());
        PrimitiveType            pt       = primType->GetType();
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
    // NOTE: This method is currently unused - computed fields use DynamicValue wrappers instead
    // Kept for potential future use or backwards compatibility
    UNREFERENCED_PARAMETER(field);
}

std::string CppCodeGenerator::ExpressionToCpp(
    const Expression* expr, const std::string& objectPrefix, const std::string& fieldToReplace, const std::string& replacementValue,
    const ClassDeclaration* contextClass) const
{
    if (nullptr == expr)
    {
        return "";
    }

    // Handle binary expressions (arithmetic, comparison, logical)
    if (const BinaryExpression* binExpr = dynamic_cast<const BinaryExpression*>(expr))
    {
        std::string left  = ExpressionToCpp(binExpr->GetLeft(), objectPrefix, fieldToReplace, replacementValue, contextClass);
        std::string right = ExpressionToCpp(binExpr->GetRight(), objectPrefix, fieldToReplace, replacementValue, contextClass);
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
        std::string operand = ExpressionToCpp(unaryExpr->GetOperand(), objectPrefix, fieldToReplace, replacementValue, contextClass);
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
        std::string fieldName = fieldRef->GetFieldName();

        // Check if this field should be replaced with a different value
        if (!fieldToReplace.empty() && fieldName == fieldToReplace)
        {
            return replacementValue;
        }

        // Field references become member variable access with underscore postfix
        // Check if this field is computed (needs implicit conversion, no .value_)
        // or regular (needs .value_ accessor)
        bool isComputed = false;
        if (nullptr != contextClass && nullptr != analyzer_)
        {
            // Check if field exists and is computed or alias in the current class or base classes
            const Field* field = analyzer_->FindFieldInClass(contextClass, fieldName);
            if (nullptr != field)
            {
                isComputed = field->IsComputed() || field->IsAlias();
            }
        }

        // Add object prefix if provided (for static functions)
        if (isComputed)
        {
            // Computed fields and alias fields use implicit conversion - no .value_ suffix
            return objectPrefix + fieldName + "_";
        }
        else
        {
            // Regular wrapped fields need .value_ accessor
            return objectPrefix + fieldName + "_.value_";
        }
    }

    // Handle member access (object.field)
    if (const MemberAccessExpression* memberAccess = dynamic_cast<const MemberAccessExpression*>(expr))
    {
        std::string object = ExpressionToCpp(memberAccess->GetObject(), objectPrefix, fieldToReplace, replacementValue, contextClass);
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
        return "(" + ExpressionToCpp(parenExpr->GetExpression(), objectPrefix, fieldToReplace, replacementValue, contextClass) + ")";
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
            output_ << ExpressionToCpp(invariant->GetExpression(), "", "", "", classDecl);
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

std::vector<const Invariant*> CppCodeGenerator::GetInvariantsForField(const std::string& fieldName, const ClassDeclaration* classDecl) const
{
    std::vector<const Invariant*> result;

    if (nullptr == classDecl || nullptr == analyzer_)
    {
        return result;
    }

    // Get all invariants including inherited ones
    std::vector<const Invariant*> allInvariants;
    analyzer_->GetAllInvariants(classDecl, allInvariants);

    // Check each invariant to see if it references this field
    for (const Invariant* invariant : allInvariants)
    {
        if (nullptr == invariant || nullptr == invariant->GetExpression())
        {
            continue;
        }

        // Collect field references from the invariant expression
        std::set<std::string> referencedFields;
        analyzer_->CollectFieldReferences(invariant->GetExpression(), referencedFields);

        // If this field is referenced, add the invariant
        if (0 != referencedFields.count(fieldName))
        {
            result.push_back(invariant);
        }
    }

    return result;
}

void CppCodeGenerator::GenerateFieldChecker(const Field* field, const ClassDeclaration* classDecl, const std::vector<const Invariant*>& invariants)
{
    if (nullptr == field || invariants.empty())
    {
        return;
    }

    std::string fieldName = field->GetName();
    std::string cppType   = MapType(field->GetType());

    // Get cardinality
    const CardinalityModifier* cardMod    = field->GetCardinalityModifier();
    bool                       isArray    = (nullptr != cardMod) && cardMod->IsArray();
    bool                       isOptional = (nullptr != cardMod) && cardMod->IsOptional() && !isArray;

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

    // Generate checker function signature
    WriteIndent(1);
    output_ << "/// \\brief Check if setting " << fieldName << " would violate invariants\n";
    WriteIndent(1);
    output_ << "/// \\param oldValue The current value\n";
    WriteIndent(1);
    output_ << "/// \\param newValue The proposed new value\n";
    WriteIndent(1);
    output_ << "void Check" << fieldName << "(const " << fullType << "& oldValue, const " << fullType << "& newValue) const\n";
    WriteIndent(1);
    output_ << "{\n";

    // Temporarily set the field to newValue
    WriteIndent(2);
    output_ << "// Temporarily set field to new value\n";
    WriteIndent(2);
    output_ << "const_cast<" << ApplyClassPrefix(classDecl->GetName()) << "*>(this)->" << fieldName << "_ = newValue;\n\n";

    // Check each invariant
    WriteIndent(2);
    output_ << "// Check invariants\n";
    for (const Invariant* invariant : invariants)
    {
        WriteIndent(2);
        output_ << "if (false == Check" << invariant->GetName() << "())\n";
        WriteIndent(2);
        output_ << "{\n";
        WriteIndent(3);
        output_ << "// Restore old value\n";
        WriteIndent(3);
        output_ << "const_cast<" << ApplyClassPrefix(classDecl->GetName()) << "*>(this)->" << fieldName << "_ = oldValue;\n";
        WriteIndent(3);
        output_ << "throw std::invalid_argument(\"Setting " << fieldName << " would violate invariant " << invariant->GetName() << ": ";

        // Include the invariant condition expression
        if (nullptr != invariant->GetExpression())
        {
            std::string exprStr = ExpressionToCpp(invariant->GetExpression(), "", "", "", classDecl);
            // Escape quotes in the expression string
            for (char c : exprStr)
            {
                if ('\"' == c)
                {
                    output_ << "\\\"";
                }
                else
                {
                    output_ << c;
                }
            }
        }
        else
        {
            output_ << "true";
        }

        output_ << "\");\n";
        WriteIndent(2);
        output_ << "}\n";
    }

    // Restore old value
    WriteIndent(2);
    output_ << "\n";
    WriteIndent(2);
    output_ << "// Restore old value\n";
    WriteIndent(2);
    output_ << "const_cast<" << ApplyClassPrefix(classDecl->GetName()) << "*>(this)->" << fieldName << "_ = oldValue;\n";
    WriteIndent(1);
    output_ << "}\n\n";
}

void CppCodeGenerator::GenerateSetter(const Field* field, const ClassDeclaration* classDecl)
{
    if (nullptr == field || nullptr == classDecl)
    {
        return;
    }

    std::string fieldName = field->GetName();
    std::string cppType   = MapType(field->GetType());

    // Get cardinality
    const CardinalityModifier* cardMod    = field->GetCardinalityModifier();
    bool                       isArray    = (nullptr != cardMod) && cardMod->IsArray();
    bool                       isOptional = (nullptr != cardMod) && cardMod->IsOptional() && !isArray;

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

    // Determine parameter type (const reference for complex types, by value for primitives)
    bool isPrimitive = field->GetType()->IsPrimitive();
    bool passByValue = isPrimitive && !isArray && !isOptional;

    // Exception: pass custom runtime types by reference too
    if (isPrimitive)
    {
        const PrimitiveTypeSpec* primType = dynamic_cast<const PrimitiveTypeSpec*>(field->GetType());
        PrimitiveType            pt       = primType->GetType();
        if (PrimitiveType::STRING == pt || PrimitiveType::DATE == pt || PrimitiveType::GUID == pt)
        {
            passByValue = false; // Pass by reference for runtime types
        }
    }

    std::string paramType;
    if (passByValue)
    {
        paramType = fullType;
    }
    else
    {
        paramType = "const " + fullType + "&";
    }

    // Generate setter name
    std::string setterName = "Set" + fieldName;
    if (false == setterName.empty() && setterName.length() > 3)
    {
        setterName[3] = static_cast<char>(std::toupper(setterName[3]));
    }

    // Generate setter method
    WriteIndent(1);
    output_ << "/// \\brief Set the " << fieldName << " field\n";
    WriteIndent(1);
    output_ << "/// \\param value The new value for " << fieldName << "\n";
    WriteIndent(1);
    output_ << "void " << setterName << "(" << paramType << " value)\n";
    WriteIndent(1);
    output_ << "{\n";

    // Get invariants for this field
    std::vector<const Invariant*> fieldInvariants = GetInvariantsForField(fieldName, classDecl);

    if (false == fieldInvariants.empty())
    {
        // Call checker function
        WriteIndent(2);
        output_ << "Check" << fieldName << "(" << fieldName << "_, value);\n\n";
    }

    // Set the value
    WriteIndent(2);
    output_ << fieldName << "_ = value;\n";

    // Update modification date
    WriteIndent(2);
    output_ << "modificationDate_ = std::chrono::duration_cast<std::chrono::duration<double>>(\n";
    WriteIndent(3);
    output_ << "std::chrono::system_clock::now().time_since_epoch()).count();\n";

    WriteIndent(1);
    output_ << "}\n\n";
}

void CppCodeGenerator::GenerateUniversalMetadata(const ClassDeclaration* classDecl)
{
    // Universal metadata fields are now inherited from bbfm::runtime::Fabric base class
    // This method is kept for future extensions but currently does nothing
    UNREFERENCED_PARAMETER(classDecl);
}

void CppCodeGenerator::GenerateConstructorImplementation(const ClassDeclaration* classDecl)
{
    if (nullptr == classDecl)
    {
        return;
    }

    const auto& fields    = classDecl->GetFields();
    std::string className = ApplyClassPrefix(classDecl->GetName());

    // Generate inline constructor implementation with field initialization
    WriteIndent(1);
    output_ << "/// \\brief Constructor - initializes all fields with parent reference\n";
    WriteIndent(1);
    output_ << className << "()\n";

    // Generate initialization list for wrapped fields
    bool firstInit = true;
    for (const auto& field : fields)
    {
        // Alias fields get initialized with reference to target field
        if (field->IsAlias())
        {
            const Expression*     expr     = field->GetInitializer();
            const FieldReference* fieldRef = dynamic_cast<const FieldReference*>(expr);
            if (nullptr != fieldRef)
            {
                std::string targetFieldName = fieldRef->GetFieldName();
                if (firstInit)
                {
                    WriteIndent(2);
                    output_ << ": " << field->GetName() << "_(" << targetFieldName << "_)";
                    firstInit = false;
                }
                else
                {
                    output_ << ",\n";
                    WriteIndent(2);
                    output_ << "  " << field->GetName() << "_(" << targetFieldName << "_)";
                }
            }
            continue;
        }

        // Computed fields get initialized with lambda
        if (field->IsComputed())
        {
            if (firstInit)
            {
                WriteIndent(2);
                output_ << ": " << field->GetName() << "_(*this, [](const " << className << "& parent) { return ";
                firstInit = false;
            }
            else
            {
                output_ << ",\n";
                WriteIndent(2);
                output_ << "  " << field->GetName() << "_(*this, [](const " << className << "& parent) { return ";
            }

            if (nullptr != field->GetInitializer())
            {
                output_ << ExpressionToCpp(field->GetInitializer(), "parent.", "", "", classDecl);
            }
            else
            {
                output_ << "/* TODO: expression */";
            }
            output_ << "; })";
            continue;
        }

        // Skip array fields (they don't use wrappers, default constructed)
        const CardinalityModifier* cardMod = field->GetCardinalityModifier();
        bool                       isArray = (nullptr != cardMod) && cardMod->IsArray();
        if (isArray)
        {
            continue;
        }

        // Regular wrapped fields just need parent reference
        if (firstInit)
        {
            WriteIndent(2);
            output_ << ": " << field->GetName() << "_(*this)";
            firstInit = false;
        }
        else
        {
            output_ << ",\n";
            WriteIndent(2);
            output_ << "  " << field->GetName() << "_(*this)";
        }
    }

    output_ << "\n";
    WriteIndent(1);
    output_ << "{\n";
    WriteIndent(1);
    output_ << "}\n\n";
}

void CppCodeGenerator::GenerateStaticCheckerFunctions(const ClassDeclaration* classDecl)
{
    if (nullptr == classDecl || nullptr == analyzer_)
    {
        return;
    }

    const auto& fields    = classDecl->GetFields();
    std::string className = ApplyClassPrefix(classDecl->GetName());

    // Generate checker functions for each field that has invariants
    for (const auto& field : fields)
    {
        // Skip computed fields and alias fields
        if (field->IsComputed() || field->IsAlias())
        {
            continue;
        }

        // Get invariants for this field
        std::vector<const Invariant*> fieldInvariants = GetInvariantsForField(field->GetName(), classDecl);

        if (fieldInvariants.empty())
        {
            continue;
        }

        // Generate a static checker function for each invariant that references this field
        for (const Invariant* invariant : fieldInvariants)
        {
            GenerateCheckerFunction(field.get(), invariant, classDecl);
        }
    }
}

void CppCodeGenerator::GenerateCheckerFunction(const Field* field, const Invariant* invariant, const ClassDeclaration* classDecl)
{
    if (nullptr == field || nullptr == invariant || nullptr == classDecl)
    {
        return;
    }

    std::string className     = ApplyClassPrefix(classDecl->GetName());
    std::string fieldName     = field->GetName();
    std::string invariantName = invariant->GetName();
    std::string cppType       = MapType(field->GetType());

    // Generate documentation
    WriteIndent(1);
    output_ << "/// \\brief Check invariant '" << invariantName << "' for field '" << fieldName << "'\n";
    WriteIndent(1);
    output_ << "/// \\param object Reference to the containing object\n";
    WriteIndent(1);
    output_ << "/// \\param newValue The new value being assigned to " << fieldName << "\n";
    WriteIndent(1);
    output_ << "/// \\return True if invariant is satisfied, false otherwise\n";
    WriteIndent(1);
    output_ << "static bool Check" << invariantName << "_" << fieldName << "(const " << className << "& object, const " << cppType << "& newValue)\n";
    WriteIndent(1);
    output_ << "{\n";

    // Generate invariant check with return
    WriteIndent(2);
    output_ << "return ";

    if (nullptr != invariant->GetExpression())
    {
        // Pass fieldName to replace with newValue in the expression
        output_ << ExpressionToCpp(invariant->GetExpression(), "object.", fieldName, "newValue", classDecl);
    }
    else
    {
        output_ << "true";
    }
    output_ << ";\n";

    WriteIndent(1);
    output_ << "}\n\n";
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
