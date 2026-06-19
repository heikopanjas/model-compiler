#ifndef __CPP_CODE_GENERATOR_H_INCL__
#define __CPP_CODE_GENERATOR_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

#include "runtime/AST.h"
#include "runtime/ICodeGenerator.h"
#include "runtime/Array.h"
#include "runtime/String.h"
#include <sstream>
#include "runtime/Array.h"
#include "runtime/String.h"

/// \brief C++ code generator
///
/// Generates C++ header files from .fm models. Produces:
/// - enum class declarations
/// - Class declarations with inheritance
/// - Member variables (private with underscore postfix)
/// - Getter methods (public, const-correct)
/// - Computed feature methods
/// - runtime::Invariant validation methods
/// - Universal metadata fields
class CppCodeGenerator : public runtime::ICodeGenerator
{
public:
    /// \brief Construct a C++ code generator
    /// \param ast Pointer to the validated runtime::AST
    /// \param analyzer Pointer to the semantic analyzer with symbol table
    /// \param namespaces Vector of namespace names (outer to inner)
    /// \param classPrefix Optional prefix to add to generated class and enum names
    CppCodeGenerator(const runtime::AST* ast, const runtime::SemanticAnalyzer* analyzer, const runtime::Array<runtime::String>& namespaces, const runtime::String& classPrefix);

    /// \brief Virtual destructor
    virtual ~CppCodeGenerator() = default;

    /// \brief Generate C++ code and write to output file
    /// \param outputPath The path to the output file
    /// \return True if generation succeeded, false on error
    bool Generate(const runtime::String& outputPath) override;

    /// \brief Get the file extension for C++ headers
    /// \return ".h"
    runtime::String GetFileExtension() const override;

    /// \brief Get the target language name
    /// \return "C++"
    runtime::String GetLanguageName() const override;

private:
    std::ostringstream output_;

    /// \brief Map model primitive type to C++ type
    /// \param type The primitive type to map
    /// \return C++ type string
    runtime::String MapPrimitiveType(runtime::PrimitiveType type) const;

    /// \brief Map model type to C++ type (primitive or user-defined)
    /// \param typeSpec The type specification
    /// \return C++ type string with namespace prefix
    runtime::String MapType(const runtime::TypeSpec* typeSpec) const;

    /// \brief Find which class declares a specific field (including base classes)
    /// \param classDecl The class to start searching from
    /// \param fieldName The field name to find
    /// \return The class that declares the field or nullptr
    const runtime::ClassDeclaration* FindClassDeclaringField(const runtime::ClassDeclaration* classDecl, const runtime::String& fieldName) const;

    /// \brief Get the full wrapper type for a field (e.g., BoundedValue<int, Class, ...>)
    /// \param field The field
    /// \param classDecl The containing class
    /// \return Full wrapper type string
    runtime::String GetFieldWrapperType(const runtime::Field* field, const runtime::ClassDeclaration* classDecl) const;

    /// \brief Generate include guard name from filename
    /// \param filename The output filename
    /// \return Include guard macro name
    runtime::String GenerateIncludeGuardName(const runtime::String& filename) const;

    /// \brief Generate file header with includes and opening namespace
    /// \param filename The output filename
    void GenerateFileHeader(const runtime::String& filename);

    /// \brief Generate file footer with closing namespace
    void GenerateFileFooter();

    /// \brief Generate opening namespace declarations
    void GenerateNamespaceOpen();

    /// \brief Generate closing namespace declarations
    void GenerateNamespaceClose();

    /// \brief Generate an enum declaration
    /// \param enumDecl The enum declaration from runtime::AST
    void GenerateEnum(const runtime::EnumDeclaration* enumDecl);

    /// \brief Generate a class declaration
    /// \param classDecl The class declaration from runtime::AST
    void GenerateClass(const runtime::ClassDeclaration* classDecl);

    /// \brief Generate member variable declarations for a class
    /// \param classDecl The class declaration
    void GenerateClassFields(const runtime::ClassDeclaration* classDecl);

    /// \brief Generate getter method for a field
    /// \param field The field to generate getter for
    void GenerateGetter(const runtime::Field* field);

    /// \brief Generate getter for computed feature
    /// \param field The computed field
    void GenerateComputedGetter(const runtime::Field* field);

    /// \brief Generate setter method for a field
    /// \param field The field to generate setter for
    /// \param classDecl The class declaration containing the field
    void GenerateSetter(const runtime::Field* field, const runtime::ClassDeclaration* classDecl);

    /// \brief Generate private checker function for a field
    /// \param field The field to generate checker for
    /// \param classDecl The class declaration containing the field
    /// \param invariants Vector of invariants that reference this field
    void GenerateFieldChecker(const runtime::Field* field, const runtime::ClassDeclaration* classDecl, const runtime::Array<const runtime::Invariant*>& invariants);

    /// \brief Get invariants that reference a specific field
    /// \param fieldName The name of the field
    /// \param classDecl The class declaration
    /// \return Vector of invariants that reference the field
    runtime::Array<const runtime::Invariant*> GetInvariantsForField(const runtime::String& fieldName, const runtime::ClassDeclaration* classDecl) const;

    /// \brief Convert expression to C++ code
    /// \param expr The expression to convert
    /// \param objectPrefix Optional prefix for field references (e.g., "obj." for static functions)
    /// \param fieldToReplace Optional field name to replace with a different value
    /// \param replacementValue Optional value to use instead of field reference
    /// \param contextClass Optional class context to check if fields are computed
    /// \return C++ code string
    runtime::String ExpressionToCpp(
        const runtime::Expression* expr, const runtime::String& objectPrefix = "", const runtime::String& fieldToReplace = "", const runtime::String& replacementValue = "",
        const runtime::ClassDeclaration* contextClass = nullptr) const;

    /// \brief Generate invariant validation methods
    /// \param classDecl The class declaration
    void GenerateInvariantMethods(const runtime::ClassDeclaration* classDecl);

    /// \brief Generate static checker functions for invariants
    /// \param classDecl The class declaration
    void GenerateStaticCheckerFunctions(const runtime::ClassDeclaration* classDecl);

    /// \brief Generate constructor implementation
    /// \param classDecl The class declaration
    void GenerateConstructorImplementation(const runtime::ClassDeclaration* classDecl);

    /// \brief Generate checker function for a specific field and invariant
    /// \param field The field being checked
    /// \param invariant The invariant to check
    /// \param classDecl The class declaration
    void GenerateCheckerFunction(const runtime::Field* field, const runtime::Invariant* invariant, const runtime::ClassDeclaration* classDecl);

    /// \brief Determine wrapper type for a field
    /// \param field The field to wrap
    /// \param classDecl The class declaration containing the field
    /// \return The wrapper type string (e.g., "BoundedValue", "UnboundedValue")
    runtime::String GetWrapperType(const runtime::Field* field, const runtime::ClassDeclaration* classDecl) const;

    /// \brief Generate universal metadata fields
    /// \param classDecl The class declaration
    void GenerateUniversalMetadata(const runtime::ClassDeclaration* classDecl);

    /// \brief Apply class prefix to name if prefix is set
    /// \param name The original name
    /// \return Prefixed name or original if no prefix
    runtime::String ApplyClassPrefix(const runtime::String& name) const;

    /// \brief Write indentation
    /// \param level Indentation level (each level = 4 spaces)
    void WriteIndent(int level);
};

// Restore previous alignment
#pragma pack(pop)

#endif // __CPP_CODE_GENERATOR_H_INCL__
