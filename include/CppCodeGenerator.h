#ifndef __BBFM_CPP_CODE_GENERATOR_H_INCL__
#define __BBFM_CPP_CODE_GENERATOR_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

#include "AST.h"
#include "CodeGenerator.h"
#include <sstream>
#include <string>

namespace bbfm {
/// \brief C++ code generator
///
/// Generates C++ header files from BBFM models. Produces:
/// - enum class declarations
/// - Class declarations with inheritance
/// - Member variables (private with underscore postfix)
/// - Getter methods (public, const-correct)
/// - Computed feature methods
/// - Invariant validation methods
/// - Universal metadata fields
class CppCodeGenerator : public CodeGenerator
{
public:
    /// \brief Construct a C++ code generator
    /// \param ast Pointer to the validated AST
    /// \param analyzer Pointer to the semantic analyzer with symbol table
    /// \param namespaces Vector of namespace names (outer to inner)
    /// \param classPrefix Optional prefix to add to generated class and enum names
    CppCodeGenerator(const AST* ast, const SemanticAnalyzer* analyzer, const std::vector<std::string>& namespaces, const std::string& classPrefix);

    /// \brief Virtual destructor
    virtual ~CppCodeGenerator() = default;

    /// \brief Generate C++ code and write to output file
    /// \param outputPath The path to the output file
    /// \return True if generation succeeded, false on error
    bool Generate(const std::string& outputPath) override;

    /// \brief Get the file extension for C++ headers
    /// \return ".h"
    std::string GetFileExtension() const override;

    /// \brief Get the target language name
    /// \return "C++"
    std::string GetLanguageName() const override;

private:
    std::ostringstream output_;

    /// \brief Map BBFM primitive type to C++ type
    /// \param type The primitive type to map
    /// \return C++ type string
    std::string MapPrimitiveType(PrimitiveType type) const;

    /// \brief Map BBFM type to C++ type (primitive or user-defined)
    /// \param typeSpec The type specification
    /// \return C++ type string with namespace prefix
    std::string MapType(const TypeSpec* typeSpec) const;

    /// \brief Generate include guard name from filename
    /// \param filename The output filename
    /// \return Include guard macro name
    std::string GenerateIncludeGuardName(const std::string& filename) const;

    /// \brief Generate file header with includes and opening namespace
    /// \param filename The output filename
    void GenerateFileHeader(const std::string& filename);

    /// \brief Generate file footer with closing namespace
    void GenerateFileFooter();

    /// \brief Generate opening namespace declarations
    void GenerateNamespaceOpen();

    /// \brief Generate closing namespace declarations
    void GenerateNamespaceClose();

    /// \brief Generate an enum declaration
    /// \param enumDecl The enum declaration from AST
    void GenerateEnum(const EnumDeclaration* enumDecl);

    /// \brief Generate a class declaration
    /// \param classDecl The class declaration from AST
    void GenerateClass(const ClassDeclaration* classDecl);

    /// \brief Generate member variable declarations for a class
    /// \param classDecl The class declaration
    void GenerateClassFields(const ClassDeclaration* classDecl);

    /// \brief Generate getter method for a field
    /// \param field The field to generate getter for
    void GenerateGetter(const Field* field);

    /// \brief Generate getter for computed feature
    /// \param field The computed field
    void GenerateComputedGetter(const Field* field);

    /// \brief Convert expression to C++ code
    /// \param expr The expression to convert
    /// \return C++ code string
    std::string ExpressionToCpp(const Expression* expr) const;

    /// \brief Generate invariant validation methods
    /// \param classDecl The class declaration
    void GenerateInvariantMethods(const ClassDeclaration* classDecl);

    /// \brief Generate universal metadata fields
    /// \param classDecl The class declaration
    void GenerateUniversalMetadata(const ClassDeclaration* classDecl);

    /// \brief Apply class prefix to name if prefix is set
    /// \param name The original name
    /// \return Prefixed name or original if no prefix
    std::string ApplyClassPrefix(const std::string& name) const;

    /// \brief Write indentation
    /// \param level Indentation level (each level = 4 spaces)
    void WriteIndent(int level);
};
} // namespace bbfm

// Restore previous alignment
#pragma pack(pop)

#endif // __BBFM_CPP_CODE_GENERATOR_H_INCL__
