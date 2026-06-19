#include "CppCodeGenerator.h"
#include "Common.h"
#include "Console.h"
#include "Contracts.h"
#include "GeneratorPlugin.h"
#include "runtime/SemanticAnalyzer.h"
#include <cctype>
#include <fstream>
#include <set>

// ============================================================================
// CppCodeGenerator Implementation
// ============================================================================

CppCodeGenerator::CppCodeGenerator(
    const runtime::AST* ast, const runtime::SemanticAnalyzer* analyzer, const runtime::Array<runtime::String>& namespaces, const runtime::String& classPrefix) :
    runtime::ICodeGenerator(ast, analyzer, namespaces, classPrefix)
{
}

bool CppCodeGenerator::Generate(const runtime::String& outputPath)
{
    if (nullptr == ast_ || nullptr == analyzer_)
    {
        ReportError("Cannot generate code: runtime::AST or analyzer is null");
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
        if (runtime::Declaration::Kind::ENUM == decl->GetKind())
        {
            GenerateEnum(decl->AsEnum());
        }
        else if (runtime::Declaration::Kind::CLASS == decl->GetKind())
        {
            GenerateClass(decl->AsClass());
        }
    }

    // Generate file footer
    GenerateFileFooter();

    // Close include guard
    const runtime::String includeGuard = GenerateIncludeGuardName(outputPath);
    output_ << "#endif // " << includeGuard << "\n";

    // Write to output file
    std::ofstream outFile(outputPath.GetValue());
    if (outFile.is_open() == false)
    {
        ReportError("Failed to open output file: " + outputPath);
        return false;
    }

    outFile << output_.str();
    outFile.close();

    if (outFile.good() == false)
    {
        ReportError("Error writing to output file: " + outputPath);
        return false;
    }

    Console::ReportStatus("C++ code generated successfully: " + outputPath);
    return true;
}

runtime::String CppCodeGenerator::GetFileExtension() const
{
    return ".h";
}

runtime::String CppCodeGenerator::GetLanguageName() const
{
    return "C++";
}

runtime::String CppCodeGenerator::MapPrimitiveType(const runtime::PrimitiveType type) const
{
    // TODO: Implement in Step 7
    switch (type)
    {
        case runtime::PrimitiveType::STRING:
            return "runtime::String";
        case runtime::PrimitiveType::INT:
            return "int64_t";
        case runtime::PrimitiveType::REAL:
            return "double";
        case runtime::PrimitiveType::BOOL:
            return "bool";
        case runtime::PrimitiveType::TIMESTAMP:
            return "double";
        case runtime::PrimitiveType::TIMESPAN:
            return "double";
        case runtime::PrimitiveType::DATE:
            return "runtime::Date";
        case runtime::PrimitiveType::GUID:
            return "runtime::Guid";
        default:
            return "unknown";
    }
}

runtime::String CppCodeGenerator::MapType(const runtime::TypeSpec* typeSpec) const
{
    // TODO: Implement fully in Step 7
    if (typeSpec->IsPrimitive() == true)
    {
        const runtime::PrimitiveTypeSpec* primType = dynamic_cast<const runtime::PrimitiveTypeSpec*>(typeSpec);
        return MapPrimitiveType(primType->GetType());
    }
    else
    {
        const runtime::UserDefinedTypeSpec* userType = dynamic_cast<const runtime::UserDefinedTypeSpec*>(typeSpec);
        return FormatNamespacePrefix() + ApplyClassPrefix(userType->GetTypeName());
    }
}

const runtime::ClassDeclaration* CppCodeGenerator::FindClassDeclaringField(const runtime::ClassDeclaration* classDecl, const runtime::String& fieldName) const
{
    RequireReturn(nullptr != classDecl, nullptr);
    RequireReturn(fieldName.IsEmpty() == false, nullptr);

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
        const TypeSymbol* baseSymbol = symbolTable.GetValuePtr(classDecl->GetBaseType());
        if (nullptr != baseSymbol && nullptr != baseSymbol->classDecl)
        {
            return FindClassDeclaringField(baseSymbol->classDecl, fieldName);
        }
    }

    return nullptr;
}

runtime::String CppCodeGenerator::GetFieldWrapperType(const runtime::Field* field, const runtime::ClassDeclaration* classDecl) const
{
    RequireReturn(nullptr != field, runtime::String{"void"});
    RequireReturn(field->IsComputed() == false, runtime::String{"void"});
    RequireReturn(field->IsAlias() == false, runtime::String{"void"});

    runtime::String cppType   = MapType(field->GetType());
    runtime::String className = ApplyClassPrefix(classDecl->GetName());

    // Get cardinality
    const runtime::CardinalityModifier* cardMod    = field->GetCardinalityModifier();
    const bool                          isOptional = (nullptr != cardMod) && cardMod->IsOptional() == true;

    // Check if field has invariants
    const runtime::Array<const runtime::Invariant*> fieldInvariants = GetInvariantsForField(field->GetName(), classDecl);
    const bool                                   hasInvariants   = fieldInvariants.GetCount() > 0;

    // Build template parameters
    runtime::String templateParams = cppType + ", " + className;

    // Add checker functions if there are invariants
    if (hasInvariants == true)
    {
        for (const runtime::Invariant* inv : fieldInvariants)
        {
            templateParams = templateParams + ", &" + className + "::Require_" + field->GetName() + "_" + inv->GetName();
        }
    }

    // Choose wrapper based on invariants and optionality
    runtime::String wrapperType;
    if (hasInvariants == true && isOptional == true)
    {
        wrapperType = "runtime::OptionalBoundedValue";
    }
    else if (hasInvariants == true && isOptional == false)
    {
        wrapperType = "runtime::BoundedValue";
    }
    else if (hasInvariants == false && isOptional == true)
    {
        wrapperType = "runtime::OptionalUnboundedValue";
    }
    else
    {
        wrapperType = "runtime::UnboundedValue";
    }

    return wrapperType + "<" + templateParams + ">";
}

runtime::String CppCodeGenerator::GenerateIncludeGuardName(const runtime::String& filename) const
{
    // Convert filename to include guard format
    // Example: "podcast.h" -> "__GENERATED_PODCAST_H_INCL__"
    runtime::String guard = "__GENERATED_";

    // Extract base filename without extension
    const size_t lastSlash = filename.GetValue().find_last_of("/\\");
    runtime::String baseName = (lastSlash != std::string::npos) ? runtime::String(filename.GetValue().substr(lastSlash + 1)) : filename;

    const size_t lastDot = baseName.GetValue().find_last_of('.');
    if (lastDot != std::string::npos)
    {
        baseName = runtime::String(baseName.GetValue().substr(0, lastDot));
    }

    // Convert to uppercase and replace non-alphanumeric with underscore
    for (const char c : baseName.GetValue())
    {
        if (std::isalnum(c) != 0)
        {
            guard = guard + runtime::String(std::string(1, static_cast<char>(std::toupper(c))));
        }
        else
        {
            guard = guard + runtime::String("_");
        }
    }

    guard = guard + runtime::String("_H_INCL__");
    return guard;
}

void CppCodeGenerator::GenerateFileHeader(const runtime::String& filename)
{
    const runtime::String includeGuard = GenerateIncludeGuardName(filename);

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
    if (namespaces_.GetCount() == 0)
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
    if (namespaces_.GetCount() == 0)
    {
        return;
    }

    // Close namespaces in reverse order
    output_ << "\n";
    for (size_t i = namespaces_.GetCount(); i > 0; --i)
    {
        output_ << "} // namespace " << namespaces_.GetValueAt(i - 1) << "\n";
    }
}

void CppCodeGenerator::GenerateEnum(const runtime::EnumDeclaration* enumDecl)
{
    Require(nullptr != enumDecl);

    // Generate enum class with prefix if specified
    runtime::String enumName = ApplyClassPrefix(enumDecl->GetName());
    const auto& values   = enumDecl->GetValues();

    output_ << "/// \\brief " << enumName << " enumeration\n";
    output_ << "enum class " << enumName << "\n";
    output_ << "{\n";

    // Generate enum values
    for (size_t i = 0; i < values.GetCount(); ++i)
    {
        WriteIndent(1);
        output_ << values.GetValueAt(i);

        if (i < values.GetCount() - 1)
        {
            output_ << ",";
        }

        output_ << "\n";
    }

    output_ << "};\n\n";
}

void CppCodeGenerator::GenerateClass(const runtime::ClassDeclaration* classDecl)
{
    Require(nullptr != classDecl);

    runtime::String className = ApplyClassPrefix(classDecl->GetName());

    // Generate struct documentation
    output_ << "/// \\brief " << className << " struct\n";

    // Generate struct declaration (public by default)
    output_ << "struct " << className;

    // Determine inheritance
    const runtime::String& baseType = classDecl->GetBaseType();
    if (baseType.IsEmpty() == false)
    {
        // User-defined base class (which already inherits from Fabric)
        runtime::String baseClassName = ApplyClassPrefix(baseType);
        output_ << " : public " << baseClassName;
    }
    else
    {
        // No user-defined base class, so inherit from Fabric
        output_ << " : public runtime::Fabric";
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

void CppCodeGenerator::GenerateClassFields(const runtime::ClassDeclaration* classDecl)
{
    Require(nullptr != classDecl);

    const auto& fields = classDecl->GetFields();

    if (fields.GetCount() == 0)
    {
        // No user-defined fields (universal metadata inherited from Fabric)
        return;
    }

    // Add comment separator for user-defined fields
    WriteIndent(1);
    output_ << "// User-defined fields\n";

    // CRITICAL: Generate fields in declaration order to ensure proper C++ initialization
    // 1. Regular fields first (non-alias, non-computed)
    // 2. Computed fields second (may reference regular fields)
    // 3. Alias fields last (reference other fields - must be initialized after targets)

    // Pass 1: Generate regular fields (non-alias, non-computed)
    for (const auto& field : fields)
    {
        // Skip alias and computed fields - they'll be generated in later passes
        if (field->IsAlias() == true || field->IsComputed() == true)
        {
            continue;
        }

        // Get the base type
        runtime::String cppType = MapType(field->GetType());

        // Get cardinality modifier
        const runtime::CardinalityModifier* cardMod = field->GetCardinalityModifier();

        // Determine if array or optional
        const bool isArray    = (nullptr != cardMod) && cardMod->IsArray() == true;
        const bool isOptional = (nullptr != cardMod) && cardMod->IsOptional() == true && isArray == false;

        // Arrays use std::vector (no wrapper)
        if (isArray == true)
        {
            cppType = "runtime::Array<" + cppType + ">";
        }
        else
        {
            // Determine wrapper type for non-array fields
            // Check if field has invariants
            const runtime::Array<const runtime::Invariant*> fieldInvariants = GetInvariantsForField(field->GetName(), classDecl);
            const bool                                   hasInvariants   = fieldInvariants.GetCount() > 0;

            const runtime::String className = ApplyClassPrefix(classDecl->GetName());
            const runtime::String fieldName = field->GetName();

            // Build template parameters
            runtime::String templateParams = cppType; // Base type T

            // Add ParentT parameter
            templateParams = templateParams + ", " + className;

            // Add checker functions if there are invariants
            if (hasInvariants == true)
            {
                for (const runtime::Invariant* inv : fieldInvariants)
                {
                    templateParams = templateParams + ", &" + className + "::Require_" + fieldName + "_" + inv->GetName();
                }
            }

            // Choose wrapper based on invariants and optionality
            runtime::String wrapperType;
            if (hasInvariants == true && isOptional == true)
            {
                wrapperType = "runtime::OptionalBoundedValue";
            }
            else if (hasInvariants == true && isOptional == false)
            {
                wrapperType = "runtime::BoundedValue";
            }
            else if (hasInvariants == false && isOptional == true)
            {
                wrapperType = "runtime::OptionalUnboundedValue";
            }
            else
            {
                wrapperType = "runtime::UnboundedValue";
            }

            cppType = wrapperType + "<" + templateParams + ">";
        }

        // Generate field declaration with underscore postfix
        WriteIndent(1);
        output_ << cppType << " " << field->GetName() << "_";

        // Add comment for unique constraints
        if (field->HasUniqueConstraint() == true)
        {
            output_ << "; // Unique constraint\n";
        }
        else
        {
            output_ << ";\n";
        }
    }

    // Pass 2: Generate computed fields (may reference regular fields)
    for (const auto& field : fields)
    {
        if (field->IsComputed() == false)
        {
            continue;
        }

        const runtime::String cppType   = MapType(field->GetType());
        const runtime::String className = ApplyClassPrefix(classDecl->GetName());
        const runtime::String fieldName = field->GetName();

        // Use DynamicValue wrapper for computed fields
        WriteIndent(1);
        output_ << "runtime::DynamicValue<" << cppType << ", " << className << "> " << fieldName << "_;\n";
    }

    // Pass 3: Generate alias fields LAST (must come after their target fields)
    for (const auto& field : fields)
    {
        if (field->IsAlias() == false)
        {
            continue;
        }

        // Get the target field name from the runtime::FieldReference expression
        const runtime::Expression*     expr     = field->GetInitializer();
        const runtime::FieldReference* fieldRef = dynamic_cast<const runtime::FieldReference*>(expr);
        if (nullptr != fieldRef)
        {
            const runtime::String targetFieldName = fieldRef->GetFieldName();
            const runtime::String fieldName       = field->GetName();

            // Find the target field to determine its wrapper type
            const runtime::Field* targetField = analyzer_->FindFieldInClass(classDecl, targetFieldName);
            if (nullptr != targetField)
            {
                // Find which class actually declares this field (could be base class)
                const runtime::ClassDeclaration* targetFieldClass = FindClassDeclaringField(classDecl, targetFieldName);
                if (nullptr != targetFieldClass)
                {
                    const runtime::String targetWrapperType = GetFieldWrapperType(targetField, targetFieldClass);

                    // Use AliasValue wrapper that forwards to target field
                    WriteIndent(1);
                    output_ << "runtime::AliasValue<" << targetWrapperType << "> " << fieldName << "_;\n";
                }
            }
        }
    }
}

void CppCodeGenerator::GenerateGetter(const runtime::Field* field)
{
    Require(nullptr != field);

    // Get the C++ type
    runtime::String cppType = MapType(field->GetType());

    // Get cardinality
    const runtime::CardinalityModifier* cardMod    = field->GetCardinalityModifier();
    const bool                          isArray    = (nullptr != cardMod) && cardMod->IsArray() == true;
    const bool                          isOptional = (nullptr != cardMod) && cardMod->IsOptional() == true && isArray == false;

    // Wrap type if needed
    runtime::String fullType = cppType;
    if (isArray == true)
    {
        fullType = "runtime::Array<" + cppType + ">";
    }
    else if (isOptional == true)
    {
        fullType = "std::optional<" + cppType + ">";
    }

    // Generate getter method
    runtime::String getterName = "Get" + field->GetName();

    // Capitalize first letter of getter name
    if (getterName.IsEmpty() == false)
    {
        std::string getterNameMut = getterName.GetValue();
        getterNameMut[3]          = static_cast<char>(std::toupper(getterNameMut[3]));
        getterName                = runtime::String(getterNameMut);
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
    const bool isPrimitive   = field->GetType()->IsPrimitive();
    bool       returnByValue = isPrimitive == true && isArray == false && isOptional == false;

    // Exception: return custom runtime types by reference too
    if (isPrimitive == true)
    {
        const runtime::PrimitiveTypeSpec* primType = dynamic_cast<const runtime::PrimitiveTypeSpec*>(field->GetType());
        const runtime::PrimitiveType      pt       = primType->GetType();
        if (pt == runtime::PrimitiveType::STRING || pt == runtime::PrimitiveType::DATE || pt == runtime::PrimitiveType::GUID)
        {
            returnByValue = false; // Return by reference for runtime types
        }
    }

    if (true == returnByValue)
    {
        output_ << fullType << " " << getterName << "() const;\n";
    }
    else
    {
        output_ << "const " << fullType << "& " << getterName << "() const;\n";
    }

    output_ << "\n";
}

void CppCodeGenerator::GenerateComputedGetter(const runtime::Field* field)
{
    // NOTE: This method is currently unused - computed fields use DynamicValue wrappers instead
    // Kept for potential future use or backwards compatibility
    UNREFERENCED_PARAMETER(field);
}

runtime::String CppCodeGenerator::ExpressionToCpp(
    const runtime::Expression* expr, const runtime::String& objectPrefix, const runtime::String& fieldToReplace, const runtime::String& replacementValue,
    const runtime::ClassDeclaration* contextClass) const
{
    RequireReturn(nullptr != expr, runtime::String{});

    // Handle binary expressions (arithmetic, comparison, logical)
    if (const runtime::BinaryExpression* binExpr = dynamic_cast<const runtime::BinaryExpression*>(expr))
    {
        const runtime::String left  = ExpressionToCpp(binExpr->GetLeft(), objectPrefix, fieldToReplace, replacementValue, contextClass);
        const runtime::String right = ExpressionToCpp(binExpr->GetRight(), objectPrefix, fieldToReplace, replacementValue, contextClass);
        runtime::String       op;

        switch (binExpr->GetOperator())
        {
            case runtime::BinaryExpression::Op::ADD:
                op = " + ";
                break;
            case runtime::BinaryExpression::Op::SUB:
                op = " - ";
                break;
            case runtime::BinaryExpression::Op::MUL:
                op = " * ";
                break;
            case runtime::BinaryExpression::Op::DIV:
                op = " / ";
                break;
            case runtime::BinaryExpression::Op::MOD:
                op = " % ";
                break;
            case runtime::BinaryExpression::Op::LT:
                op = " < ";
                break;
            case runtime::BinaryExpression::Op::GT:
                op = " > ";
                break;
            case runtime::BinaryExpression::Op::LE:
                op = " <= ";
                break;
            case runtime::BinaryExpression::Op::GE:
                op = " >= ";
                break;
            case runtime::BinaryExpression::Op::EQ:
                op = " == ";
                break;
            case runtime::BinaryExpression::Op::NE:
                op = " != ";
                break;
            case runtime::BinaryExpression::Op::AND:
                op = " && ";
                break;
            case runtime::BinaryExpression::Op::OR:
                op = " || ";
                break;
            default:
                op = " ? ";
                break;
        }

        return left + op + right;
    }

    // Handle unary expressions (negation, logical not)
    if (const runtime::UnaryExpression* unaryExpr = dynamic_cast<const runtime::UnaryExpression*>(expr))
    {
        const runtime::String operand = ExpressionToCpp(unaryExpr->GetOperand(), objectPrefix, fieldToReplace, replacementValue, contextClass);
        runtime::String       op;

        switch (unaryExpr->GetOperator())
        {
            case runtime::UnaryExpression::Op::NEG:
                op = "-";
                break;
            case runtime::UnaryExpression::Op::NOT:
                op = "!";
                break;
            default:
                op = "?";
                break;
        }

        return op + operand;
    }

    // Handle field references
    if (const runtime::FieldReference* fieldRef = dynamic_cast<const runtime::FieldReference*>(expr))
    {
        const runtime::String fieldName = fieldRef->GetFieldName();

        // Check if this field should be replaced with a different value
        if (fieldToReplace.IsEmpty() == false && fieldName == fieldToReplace)
        {
            return replacementValue;
        }

        // runtime::Field references become member variable access with underscore postfix
        // Check if this field is computed (needs implicit conversion, no .value_)
        // or regular (needs .value_ accessor)
        bool isComputed = false;
        if (nullptr != contextClass && nullptr != analyzer_)
        {
            // Check if field exists and is computed or alias in the current class or base classes
            const runtime::Field* field = analyzer_->FindFieldInClass(contextClass, fieldName);
            if (nullptr != field)
            {
                isComputed = field->IsComputed() || field->IsAlias();
            }
        }

        // Add object prefix if provided (for static functions)
        if (true == isComputed)
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
    if (const runtime::MemberAccessExpression* memberAccess = dynamic_cast<const runtime::MemberAccessExpression*>(expr))
    {
        const runtime::String object = ExpressionToCpp(memberAccess->GetObject(), objectPrefix, fieldToReplace, replacementValue, contextClass);
        const runtime::String member = memberAccess->GetMemberName();

        // Convert member access to getter call
        // Capitalize first letter for getter name
        runtime::String getterName = "Get" + member;
        if (getterName.IsEmpty() == false && getterName.GetLength() > 3)
        {
            std::string getterNameMut = getterName.GetValue();
        getterNameMut[3]          = static_cast<char>(std::toupper(getterNameMut[3]));
        getterName                = runtime::String(getterNameMut);
        }

        return object + "." + getterName + "()";
    }

    // Handle literals
    if (const runtime::LiteralExpression* literal = dynamic_cast<const runtime::LiteralExpression*>(expr))
    {
        return literal->ToString();
    }

    // Handle parenthesized expressions
    if (const runtime::ParenthesizedExpression* parenExpr = dynamic_cast<const runtime::ParenthesizedExpression*>(expr))
    {
        return "(" + ExpressionToCpp(parenExpr->GetExpression(), objectPrefix, fieldToReplace, replacementValue, contextClass) + ")";
    }

    // Unknown expression type
    return "/* unknown expression */";
}

void CppCodeGenerator::GenerateInvariantMethods(const runtime::ClassDeclaration* classDecl)
{
    Require(nullptr != classDecl);

    const auto& invariants = classDecl->GetInvariants();

    if (invariants.GetCount() == 0)
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

runtime::Array<const runtime::Invariant*> CppCodeGenerator::GetInvariantsForField(const runtime::String& fieldName, const runtime::ClassDeclaration* classDecl) const
{
    RequireReturn(fieldName.IsEmpty() == false, runtime::Array<const runtime::Invariant*>{});

    runtime::Array<const runtime::Invariant*> result;

    if (nullptr == classDecl || nullptr == analyzer_)
    {
        return result;
    }

    // Get all invariants including inherited ones
    runtime::Array<const runtime::Invariant*> allInvariants;
    analyzer_->GetAllInvariants(classDecl, allInvariants);

    // Check each invariant to see if it references this field
    for (const runtime::Invariant* invariant : allInvariants)
    {
        if (nullptr == invariant || nullptr == invariant->GetExpression())
        {
            continue;
        }

        // Collect field references from the invariant expression
        std::set<runtime::String> referencedFields;
        analyzer_->CollectFieldReferences(invariant->GetExpression(), referencedFields);

        // If this field is referenced, add the invariant
        if (referencedFields.count(fieldName) != 0)
        {
            result.AddValue(invariant);
        }
    }

    return result;
}

void CppCodeGenerator::GenerateFieldChecker(
    const runtime::Field* field, const runtime::ClassDeclaration* classDecl, const runtime::Array<const runtime::Invariant*>& invariants)
{
    Require(nullptr != field);
    Require(invariants.GetCount() > 0);

    runtime::String fieldName = field->GetName();
    runtime::String cppType   = MapType(field->GetType());

    // Get cardinality
    const runtime::CardinalityModifier* cardMod    = field->GetCardinalityModifier();
    bool                                isArray    = (nullptr != cardMod) && cardMod->IsArray();
    bool                                isOptional = (nullptr != cardMod) && cardMod->IsOptional() && !isArray;

    // Wrap type if needed
    runtime::String fullType = cppType;
    if (isArray)
    {
        fullType = "runtime::Array<" + cppType + ">";
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
    for (const runtime::Invariant* invariant : invariants)
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
            runtime::String exprStr = ExpressionToCpp(invariant->GetExpression(), "", "", "", classDecl);
            // Escape quotes in the expression string
            for (char c : exprStr.GetValue())
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

void CppCodeGenerator::GenerateSetter(const runtime::Field* field, const runtime::ClassDeclaration* classDecl)
{
    Require(nullptr != field);
    Require(nullptr != classDecl);

    const runtime::String fieldName = field->GetName();
    const runtime::String cppType   = MapType(field->GetType());

    // Get cardinality
    const runtime::CardinalityModifier* cardMod    = field->GetCardinalityModifier();
    const bool                          isArray    = (nullptr != cardMod) && cardMod->IsArray() == true;
    const bool                          isOptional = (nullptr != cardMod) && cardMod->IsOptional() == true && isArray == false;

    // Wrap type if needed
    runtime::String fullType = cppType;
    if (isArray == true)
    {
        fullType = "runtime::Array<" + cppType + ">";
    }
    else if (isOptional == true)
    {
        fullType = "std::optional<" + cppType + ">";
    }

    // Determine parameter type (const reference for complex types, by value for primitives)
    const bool isPrimitive = field->GetType()->IsPrimitive();
    bool       passByValue = isPrimitive == true && isArray == false && isOptional == false;

    // Exception: pass custom runtime types by reference too
    if (isPrimitive == true)
    {
        const runtime::PrimitiveTypeSpec* primType = dynamic_cast<const runtime::PrimitiveTypeSpec*>(field->GetType());
        const runtime::PrimitiveType      pt       = primType->GetType();
        if (pt == runtime::PrimitiveType::STRING || pt == runtime::PrimitiveType::DATE || pt == runtime::PrimitiveType::GUID)
        {
            passByValue = false; // Pass by reference for runtime types
        }
    }

    runtime::String paramType;
    if (true == passByValue)
    {
        paramType = fullType;
    }
    else
    {
        paramType = "const " + fullType + "&";
    }

    // Generate setter name
    runtime::String setterName = "Set" + fieldName;
    if (setterName.IsEmpty() == false && setterName.GetLength() > 3)
    {
        std::string setterNameMut = setterName.GetValue();
        setterNameMut[3]          = static_cast<char>(std::toupper(setterNameMut[3]));
        setterName                = runtime::String(setterNameMut);
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
    const runtime::Array<const runtime::Invariant*> fieldInvariants = GetInvariantsForField(fieldName, classDecl);

    if (fieldInvariants.GetCount() > 0)
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

void CppCodeGenerator::GenerateUniversalMetadata(const runtime::ClassDeclaration* classDecl)
{
    // Universal metadata fields are now inherited from runtime::Fabric base class
    // This method is kept for future extensions but currently does nothing
    UNREFERENCED_PARAMETER(classDecl);
}

void CppCodeGenerator::GenerateConstructorImplementation(const runtime::ClassDeclaration* classDecl)
{
    Require(nullptr != classDecl);

    const auto&       fields    = classDecl->GetFields();
    const runtime::String className = ApplyClassPrefix(classDecl->GetName());

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
        if (field->IsAlias() == true)
        {
            const runtime::Expression*     expr     = field->GetInitializer();
            const runtime::FieldReference* fieldRef = dynamic_cast<const runtime::FieldReference*>(expr);
            if (nullptr != fieldRef)
            {
                const runtime::String targetFieldName = fieldRef->GetFieldName();
                if (true == firstInit)
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
        if (field->IsComputed() == true)
        {
            if (true == firstInit)
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
        const runtime::CardinalityModifier* cardMod = field->GetCardinalityModifier();
        const bool                          isArray = (nullptr != cardMod) && cardMod->IsArray() == true;
        if (isArray == true)
        {
            continue;
        }

        // Regular wrapped fields just need parent reference
        if (true == firstInit)
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

void CppCodeGenerator::GenerateStaticCheckerFunctions(const runtime::ClassDeclaration* classDecl)
{
    Require(nullptr != classDecl);
    Require(nullptr != analyzer_);

    const auto&       fields    = classDecl->GetFields();
    const runtime::String className = ApplyClassPrefix(classDecl->GetName());

    // Generate checker functions for each field that has invariants
    for (const auto& field : fields)
    {
        // Skip computed fields and alias fields
        if (field->IsComputed() == true || field->IsAlias() == true)
        {
            continue;
        }

        // Get invariants for this field
        const runtime::Array<const runtime::Invariant*> fieldInvariants = GetInvariantsForField(field->GetName(), classDecl);

        if (fieldInvariants.GetCount() == 0)
        {
            continue;
        }

        // Generate a static checker function for each invariant that references this field
        for (const runtime::Invariant* invariant : fieldInvariants)
        {
            GenerateCheckerFunction(field.get(), invariant, classDecl);
        }
    }
}

void CppCodeGenerator::GenerateCheckerFunction(const runtime::Field* field, const runtime::Invariant* invariant, const runtime::ClassDeclaration* classDecl)
{
    Require(nullptr != field);
    Require(nullptr != invariant);
    Require(nullptr != classDecl);

    const runtime::String className     = ApplyClassPrefix(classDecl->GetName());
    const runtime::String fieldName     = field->GetName();
    const runtime::String invariantName = invariant->GetName();
    const runtime::String cppType       = MapType(field->GetType());

    // Generate documentation
    WriteIndent(1);
    output_ << "/// \\brief Require invariant '" << invariantName << "' for field '" << fieldName << "'\n";
    WriteIndent(1);
    output_ << "/// \\param object Reference to the containing object\n";
    WriteIndent(1);
    output_ << "/// \\param newValue The new value being assigned to " << fieldName << "\n";
    WriteIndent(1);
    output_ << "/// \\return True if invariant is satisfied, false otherwise\n";
    WriteIndent(1);
    output_ << "static bool Require_" << fieldName << "_" << invariantName << "(const " << className << "& object, const " << cppType << "& newValue)\n";
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

runtime::String CppCodeGenerator::ApplyClassPrefix(const runtime::String& name) const
{
    if (classPrefix_.IsEmpty() == true)
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

namespace {
runtime::Array<runtime::String> CopyNamespaces(const GeneratorCreateContext* context)
{
    runtime::Array<runtime::String> namespaces;

    if (nullptr == context || nullptr == context->namespaces)
    {
        return namespaces;
    }

    for (size_t i = 0; i < context->namespaceCount; ++i)
    {
        if (nullptr != context->namespaces[i])
        {
            namespaces.AddValue(runtime::String(context->namespaces[i]));
        }
    }

    return namespaces;
}
} // namespace

extern "C" PLUGIN_EXPORT runtime::CapabilitiesDictionary* GetCapabilities()
{
    auto* capabilities = new runtime::CapabilitiesDictionary();
    capabilities->SetValue("ApiVersion", std::to_string(GENERATOR_PLUGIN_API_VERSION).c_str());
    capabilities->SetValue("Language", "C++");
    capabilities->SetValue("FileExtension", ".h");
    capabilities->SetValue("Experimental", "false");
    return capabilities;
}

extern "C" PLUGIN_EXPORT void DestroyCapabilities(runtime::CapabilitiesDictionary* capabilities)
{
    delete capabilities;
}

extern "C" PLUGIN_EXPORT runtime::ICodeGenerator* CreateGenerator(const GeneratorCreateContext* context)
{
    if (nullptr == context || nullptr == context->ast || nullptr == context->analyzer)
    {
        return nullptr;
    }

    const runtime::Array<runtime::String> namespaces  = CopyNamespaces(context);
    const runtime::String              classPrefix = (nullptr != context->classPrefix) ? context->classPrefix : "";

    return new CppCodeGenerator(context->ast, context->analyzer, namespaces, classPrefix);
}

extern "C" PLUGIN_EXPORT void DestroyGenerator(runtime::ICodeGenerator* generator)
{
    delete generator;
}
