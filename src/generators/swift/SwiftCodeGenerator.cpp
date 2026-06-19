#include "SwiftCodeGenerator.h"

#include "Contracts.h"
#include "GeneratorPlugin.h"

#include <fstream>
#include <vector>

SwiftCodeGenerator::SwiftCodeGenerator(
    const runtime::AST* ast, const runtime::SemanticAnalyzer* analyzer, const runtime::Array<runtime::String>& namespaces, const runtime::String& classPrefix) :
    runtime::ICodeGenerator(ast, analyzer, namespaces, classPrefix)
{
}

bool SwiftCodeGenerator::Generate(const runtime::String& outputPath)
{
    RequireReturn(outputPath.IsEmpty() == false, false);

    if (nullptr == ast_)
    {
        ReportError("Cannot generate Swift code from null runtime::AST");
        return false;
    }

    output_.str("");
    output_.clear();

    GenerateFileHeader(outputPath);

    for (const auto& declaration : ast_->GetDeclarations())
    {
        if (runtime::Declaration::Kind::ENUM == declaration->GetKind())
        {
            GenerateEnum(declaration->AsEnum());
        }
        else if (runtime::Declaration::Kind::CLASS == declaration->GetKind())
        {
            GenerateClass(declaration->AsClass());
        }
    }

    std::ofstream file(outputPath.GetValue());
    if (file.is_open() == false)
    {
        ReportError("Failed to open output file: " + outputPath);
        return false;
    }

    file << output_.str();
    return true;
}

runtime::String SwiftCodeGenerator::GetFileExtension() const
{
    return ".swift";
}

runtime::String SwiftCodeGenerator::GetLanguageName() const
{
    return "Swift";
}

runtime::String SwiftCodeGenerator::MapPrimitiveType(const runtime::PrimitiveType type) const
{
    switch (type)
    {
        case runtime::PrimitiveType::STRING:
            return "String";
        case runtime::PrimitiveType::INT:
            return "Int";
        case runtime::PrimitiveType::REAL:
            return "Double";
        case runtime::PrimitiveType::BOOL:
            return "Bool";
        case runtime::PrimitiveType::TIMESTAMP:
        case runtime::PrimitiveType::TIMESPAN:
            return "TimeInterval";
        case runtime::PrimitiveType::DATE:
            return "Date";
        case runtime::PrimitiveType::GUID:
            return "UUID";
    }

    return "String";
}

runtime::String SwiftCodeGenerator::MapType(const runtime::TypeSpec* typeSpec, const runtime::CardinalityModifier* cardinality) const
{
    RequireReturn(nullptr != typeSpec, "String");

    runtime::String baseType;
    if (typeSpec->IsPrimitive() == true)
    {
        const auto* primitive = dynamic_cast<const runtime::PrimitiveTypeSpec*>(typeSpec);
        baseType              = (nullptr != primitive) ? MapPrimitiveType(primitive->GetType()) : "String";
    }
    else
    {
        const auto* userDefined = dynamic_cast<const runtime::UserDefinedTypeSpec*>(typeSpec);
        baseType                = (nullptr != userDefined) ? ApplyClassPrefix(userDefined->GetTypeName()) : "String";
    }

    if (nullptr != cardinality && cardinality->IsArray() == true)
    {
        return "[" + baseType + "]";
    }

    if (nullptr != cardinality && cardinality->IsOptional() == true)
    {
        return baseType + "?";
    }

    return baseType;
}

runtime::String SwiftCodeGenerator::DefaultValueForType(const runtime::TypeSpec* typeSpec, const runtime::CardinalityModifier* cardinality) const
{
    RequireReturn(nullptr != typeSpec, "\"\"");

    if (nullptr != cardinality && cardinality->IsArray() == true)
    {
        return "[]";
    }

    if (nullptr != cardinality && cardinality->IsOptional() == true)
    {
        return "nil";
    }

    if (typeSpec->IsPrimitive() == false)
    {
        const auto* userDefined = dynamic_cast<const runtime::UserDefinedTypeSpec*>(typeSpec);
        return (nullptr != userDefined) ? ApplyClassPrefix(userDefined->GetTypeName()) + "()" : "\"\"";
    }

    const auto* primitive = dynamic_cast<const runtime::PrimitiveTypeSpec*>(typeSpec);
    if (nullptr == primitive)
    {
        return "\"\"";
    }

    switch (primitive->GetType())
    {
        case runtime::PrimitiveType::STRING:
            return "\"\"";
        case runtime::PrimitiveType::INT:
            return "0";
        case runtime::PrimitiveType::REAL:
        case runtime::PrimitiveType::TIMESTAMP:
        case runtime::PrimitiveType::TIMESPAN:
            return "0.0";
        case runtime::PrimitiveType::BOOL:
            return "false";
        case runtime::PrimitiveType::DATE:
            return "Date(timeIntervalSince1970: 0)";
        case runtime::PrimitiveType::GUID:
            return "UUID(uuidString: \"00000000-0000-0000-0000-000000000000\")!";
    }

    return "\"\"";
}

runtime::String SwiftCodeGenerator::ExpressionToSwift(const runtime::Expression* expr) const
{
    RequireReturn(nullptr != expr, "");

    return expr->ToString();
}

runtime::String SwiftCodeGenerator::ApplyClassPrefix(const runtime::String& name) const
{
    if (classPrefix_.IsEmpty() == true)
    {
        return name;
    }

    return classPrefix_ + name;
}

void SwiftCodeGenerator::GenerateFileHeader(const runtime::String& filename)
{
    output_ << "// Generated by Model Compiler\n";
    output_ << "// Source model output: " << filename << "\n\n";
    output_ << "import Foundation\n\n";

    if (namespaces_.GetCount() > 0)
    {
        output_ << "// model namespaces: ";
        for (size_t i = 0; i < namespaces_.GetCount(); ++i)
        {
            if (i > 0)
            {
                output_ << ".";
            }
            output_ << namespaces_.At(i);
        }
        output_ << "\n\n";
    }
}

void SwiftCodeGenerator::GenerateEnum(const runtime::EnumDeclaration* enumDecl)
{
    Require(nullptr != enumDecl);

    output_ << "public enum " << ApplyClassPrefix(enumDecl->GetName()) << ": String {\n";
    for (const auto& value : enumDecl->GetValues())
    {
        WriteIndent(1);
        output_ << "case " << value << " = \"" << value << "\"\n";
    }
    output_ << "}\n\n";
}

void SwiftCodeGenerator::GenerateClass(const runtime::ClassDeclaration* classDecl)
{
    Require(nullptr != classDecl);

    output_ << "public class " << ApplyClassPrefix(classDecl->GetName());
    if (classDecl->HasExplicitBase() == true)
    {
        output_ << ": " << ApplyClassPrefix(classDecl->GetBaseType());
    }
    output_ << " {\n";

    for (const auto& field : classDecl->GetFields())
    {
        GenerateField(field.get());
    }

    WriteIndent(1);
    output_ << "public init() {}\n";
    output_ << "}\n\n";
}

void SwiftCodeGenerator::GenerateField(const runtime::Field* field)
{
    Require(nullptr != field);

    const runtime::CardinalityModifier* cardinality = field->GetCardinalityModifier();
    const runtime::String          typeName    = MapType(field->GetType(), cardinality);

    WriteIndent(1);
    if (field->IsComputed() == true)
    {
        output_ << "public var " << field->GetName() << ": " << typeName << " {\n";
        WriteIndent(2);
        output_ << "return " << ExpressionToSwift(field->GetInitializer()) << "\n";
        WriteIndent(1);
        output_ << "}\n";
        return;
    }

    output_ << "public var " << field->GetName() << ": " << typeName << " = " << DefaultValueForType(field->GetType(), cardinality) << "\n";
}

void SwiftCodeGenerator::WriteIndent(const int level)
{
    for (int i = 0; i < level; ++i)
    {
        output_ << "    ";
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

    ;
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
    capabilities->SetValue("Language", "Swift");
    capabilities->SetValue("FileExtension", ".swift");
    capabilities->SetValue("Experimental", "true");
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

    return new SwiftCodeGenerator(context->ast, context->analyzer, namespaces, classPrefix);
}

extern "C" PLUGIN_EXPORT void DestroyGenerator(runtime::ICodeGenerator* generator)
{
    delete generator;
}
