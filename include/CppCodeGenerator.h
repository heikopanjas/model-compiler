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

    /// \brief Generate setter method for a field
    /// \param field The field to generate setter for
    /// \param classDecl The class declaration containing the field
    void GenerateSetter(const Field* field, const ClassDeclaration* classDecl);

    /// \brief Generate private checker function for a field
    /// \param field The field to generate checker for
    /// \param classDecl The class declaration containing the field
    /// \param invariants Vector of invariants that reference this field
    void GenerateFieldChecker(const Field* field, const ClassDeclaration* classDecl, const std::vector<const Invariant*>& invariants);

    /// \brief Get invariants that reference a specific field
    /// \param fieldName The name of the field
    /// \param classDecl The class declaration
    /// \return Vector of invariants that reference the field
    std::vector<const Invariant*> GetInvariantsForField(const std::string& fieldName, const ClassDeclaration* classDecl) const;

    /// \brief Convert expression to C++ code
    /// \param expr The expression to convert
    /// \param objectPrefix Optional prefix for field references (e.g., "obj." for static functions)
    /// \param fieldToReplace Optional field name to replace with a different value
    /// \param replacementValue Optional value to use instead of field reference
    /// \return C++ code string
    std::string ExpressionToCpp(
        const Expression* expr, const std::string& objectPrefix = "", const std::string& fieldToReplace = "", const std::string& replacementValue = "") const;

    /// \brief Generate invariant validation methods
    /// \param classDecl The class declaration
    void GenerateInvariantMethods(const ClassDeclaration* classDecl);

    /// \brief Generate static checker functions for invariants
    /// \param classDecl The class declaration
    void GenerateStaticCheckerFunctions(const ClassDeclaration* classDecl);

    /// \brief Generate constructor implementation
    /// \param classDecl The class declaration
    void GenerateConstructorImplementation(const ClassDeclaration* classDecl);

    /// \brief Generate checker function for a specific field and invariant
    /// \param field The field being checked
    /// \param invariant The invariant to check
    /// \param classDecl The class declaration
    void GenerateCheckerFunction(const Field* field, const Invariant* invariant, const ClassDeclaration* classDecl);

    /// \brief Determine wrapper type for a field
    /// \param field The field to wrap
    /// \param classDecl The class declaration containing the field
    /// \return The wrapper type string (e.g., "BoundedValue", "UnboundedValue")
    std::string GetWrapperType(const Field* field, const ClassDeclaration* classDecl) const;

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
