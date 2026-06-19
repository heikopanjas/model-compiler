#ifndef __SWIFT_CODE_GENERATOR_H_INCL__
#define __SWIFT_CODE_GENERATOR_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

#include "runtime/AST.h"
#include "runtime/ICodeGenerator.h"

#include <sstream>
#include "runtime/Array.h"
#include "runtime/String.h"

/// \brief Swift code generator
///
/// Generates Swift source files from .fm models. The initial Swift backend
/// emits enums, classes, stored properties, basic defaults, and simple computed
/// properties.
class SwiftCodeGenerator : public runtime::ICodeGenerator
{
public:
    /// \brief Construct a Swift code generator
    /// \param ast Pointer to the validated runtime::AST
    /// \param analyzer Pointer to the semantic analyzer with symbol table
    /// \param namespaces Vector of namespace names (outer to inner)
    /// \param classPrefix Optional prefix to add to generated class and enum names
    SwiftCodeGenerator(const runtime::AST* ast, const runtime::SemanticAnalyzer* analyzer, const runtime::Array<runtime::String>& namespaces, const runtime::String& classPrefix);

    /// \brief Virtual destructor
    virtual ~SwiftCodeGenerator() = default;

    /// \brief Generate Swift code and write to output file
    /// \param outputPath The path to the output file
    /// \return True if generation succeeded, false on error
    bool Generate(const runtime::String& outputPath) override;

    /// \brief Get the file extension for Swift files
    /// \return ".swift"
    runtime::String GetFileExtension() const override;

    /// \brief Get the target language name
    /// \return "Swift"
    runtime::String GetLanguageName() const override;

private:
    std::ostringstream output_;

    runtime::String MapPrimitiveType(runtime::PrimitiveType type) const;
    runtime::String MapType(const runtime::TypeSpec* typeSpec, const runtime::CardinalityModifier* cardinality = nullptr) const;
    runtime::String DefaultValueForType(const runtime::TypeSpec* typeSpec, const runtime::CardinalityModifier* cardinality = nullptr) const;
    runtime::String ExpressionToSwift(const runtime::Expression* expr) const;
    runtime::String ApplyClassPrefix(const runtime::String& name) const;

    void GenerateFileHeader(const runtime::String& filename);
    void GenerateEnum(const runtime::EnumDeclaration* enumDecl);
    void GenerateClass(const runtime::ClassDeclaration* classDecl);
    void GenerateField(const runtime::Field* field);
    void WriteIndent(int level);
};

// Restore previous alignment
#pragma pack(pop)

#endif // __SWIFT_CODE_GENERATOR_H_INCL__
