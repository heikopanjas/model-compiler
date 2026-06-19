#ifndef __RUST_CODE_GENERATOR_H_INCL__
#define __RUST_CODE_GENERATOR_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

#include "runtime/Array.h"
#include "runtime/AST.h"
#include "runtime/Dictionary.h"
#include "runtime/ICodeGenerator.h"
#include "runtime/String.h"
#include <set>
#include <sstream>

/// \brief Rust code generator
///
/// Generates Rust module files from .fm models. Produces:
/// - enum declarations
/// - struct declarations for classes without inheritance
/// - enum variants for inheritance hierarchies (base class becomes enum)
/// - impl blocks with getters, setters, and computed methods
/// - Result-based invariant validation
/// - Universal metadata fields via FabricMetadata struct
class RustCodeGenerator : public runtime::ICodeGenerator
{
public:
    /// \brief Construct a Rust code generator
    /// \param ast Pointer to the validated runtime::AST
    /// \param analyzer Pointer to the semantic analyzer with symbol table
    /// \param namespaces Vector of namespace names (outer to inner)
    /// \param classPrefix Optional prefix to add to generated class and enum names
    RustCodeGenerator(
        const runtime::AST* ast, const runtime::SemanticAnalyzer* analyzer, const runtime::Array<runtime::String>& namespaces,
        const runtime::String& classPrefix);

    /// \brief Virtual destructor
    virtual ~RustCodeGenerator() = default;

    /// \brief Generate Rust code and write to output file
    /// \param outputPath The path to the output file
    /// \return True if generation succeeded, false on error
    bool Generate(const runtime::String& outputPath) override;

    /// \brief Get the file extension for Rust modules
    /// \return ".rs"
    runtime::String GetFileExtension() const override;

    /// \brief Get the target language name
    /// \return "Rust"
    runtime::String GetLanguageName() const override;

private:
    std::ostringstream output_;

    // Track inheritance relationships
    runtime::Dictionary<runtime::String, runtime::Array<const runtime::ClassDeclaration*>> derivedClasses_;
    std::set<runtime::String>                                                              baseClasses_;

    /// \brief Map model primitive type to Rust type
    /// \param type The primitive type to map
    /// \return Rust type string
    runtime::String MapPrimitiveType(runtime::PrimitiveType type) const;

    /// \brief Map model type to Rust type (primitive or user-defined)
    /// \param typeSpec The type specification
    /// \param cardinality The cardinality modifier (optional)
    /// \return Rust type string
    runtime::String MapType(const runtime::TypeSpec* typeSpec, const runtime::CardinalityModifier* cardinality = nullptr) const;

    /// \brief Generate file header with imports and common types
    /// \param filename The output filename
    void GenerateFileHeader(const runtime::String& filename);

    /// \brief Generate file footer (empty for Rust)
    void GenerateFileFooter();

    /// \brief Generate an enum declaration
    /// \param enumDecl The enum declaration from runtime::AST
    void GenerateEnum(const runtime::EnumDeclaration* enumDecl);

    /// \brief Generate a class as struct or enum
    /// \param classDecl The class declaration from runtime::AST
    void GenerateClass(const runtime::ClassDeclaration* classDecl);

    /// \brief Generate enum with variants for inherited classes
    /// \param baseClass The base class declaration
    void GenerateInheritedEnum(const runtime::ClassDeclaration* baseClass);

    /// \brief Generate base struct for shared fields
    /// \param baseClass The base class declaration
    void GenerateBaseStruct(const runtime::ClassDeclaration* baseClass);

    /// \brief Generate struct for standalone class
    /// \param classDecl The class declaration
    void GenerateStruct(const runtime::ClassDeclaration* classDecl);

    /// \brief Generate impl block for class
    /// \param classDecl The class declaration
    void GenerateImpl(const runtime::ClassDeclaration* classDecl);

    /// \brief Generate impl block for inherited enum
    /// \param baseClass The base class declaration
    void GenerateInheritedEnumImpl(const runtime::ClassDeclaration* baseClass);

    /// \brief Generate getter method for a field
    /// \param field The field to generate getter for
    /// \param className The containing class name
    /// \param isInEnum Whether this is for an enum variant
    void GenerateGetter(const runtime::Field* field, const runtime::String& className, bool isInEnum);

    /// \brief Generate setter method for a field
    /// \param field The field to generate setter for
    /// \param classDecl The class declaration containing the field
    /// \param isInEnum Whether this is for an enum variant
    void GenerateSetter(const runtime::Field* field, const runtime::ClassDeclaration* classDecl, bool isInEnum);

    /// \brief Generate computed feature method
    /// \param field The computed field
    /// \param classDecl The class declaration
    /// \param isInEnum Whether this is for an enum variant
    void GenerateComputedGetter(const runtime::Field* field, const runtime::ClassDeclaration* classDecl, bool isInEnum);

    /// \brief Get invariants that reference a specific field
    /// \param fieldName The name of the field
    /// \param classDecl The class declaration
    /// \return Vector of invariants that reference the field
    runtime::Array<const runtime::Invariant*> GetInvariantsForField(const runtime::String& fieldName, const runtime::ClassDeclaration* classDecl) const;

    /// \brief Convert expression to Rust code
    /// \param expr The expression to convert
    /// \param prefix Optional prefix for field references (e.g., "self.")
    /// \return Rust code string
    runtime::String ExpressionToRust(const runtime::Expression* expr, const runtime::String& prefix = "self.") const;

    /// \brief Apply class prefix to name if prefix is set
    /// \param name The original name
    /// \return Prefixed name or original if no prefix
    runtime::String ApplyClassPrefix(const runtime::String& name) const;

    /// \brief Convert camelCase or PascalCase to snake_case
    /// \param name The name to convert
    /// \return snake_case name
    runtime::String ToSnakeCase(const runtime::String& name) const;

    /// \brief Build inheritance hierarchy map
    void BuildInheritanceMap();

    /// \brief Check if a class is a base class (has derived classes)
    /// \param className The class name to check
    /// \return True if class has derived classes
    bool IsBaseClass(const runtime::String& className) const;

    /// \brief Write indentation
    /// \param level Indentation level (each level = 4 spaces)
    void WriteIndent(int level);
};

// Restore previous alignment
#pragma pack(pop)

#endif // __RUST_CODE_GENERATOR_H_INCL__
