#ifndef __BBFM_RUST_CODE_GENERATOR_H_INCL__
#define __BBFM_RUST_CODE_GENERATOR_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

#include "AST.h"
#include "CodeGenerator.h"
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace bbfm {
/// \brief Rust code generator
///
/// Generates Rust module files from BBFM models. Produces:
/// - enum declarations
/// - struct declarations for classes without inheritance
/// - enum variants for inheritance hierarchies (base class becomes enum)
/// - impl blocks with getters, setters, and computed methods
/// - Result-based invariant validation
/// - Universal metadata fields via FabricMetadata struct
class RustCodeGenerator : public CodeGenerator
{
public:
    /// \brief Construct a Rust code generator
    /// \param ast Pointer to the validated AST
    /// \param analyzer Pointer to the semantic analyzer with symbol table
    /// \param namespaces Vector of namespace names (outer to inner)
    /// \param classPrefix Optional prefix to add to generated class and enum names
    RustCodeGenerator(const AST* ast, const SemanticAnalyzer* analyzer, const std::vector<std::string>& namespaces, const std::string& classPrefix);

    /// \brief Virtual destructor
    virtual ~RustCodeGenerator() = default;

    /// \brief Generate Rust code and write to output file
    /// \param outputPath The path to the output file
    /// \return True if generation succeeded, false on error
    bool Generate(const std::string& outputPath) override;

    /// \brief Get the file extension for Rust modules
    /// \return ".rs"
    std::string GetFileExtension() const override;

    /// \brief Get the target language name
    /// \return "Rust"
    std::string GetLanguageName() const override;

private:
    std::ostringstream output_;

    // Track inheritance relationships
    std::map<std::string, std::vector<const ClassDeclaration*>> derivedClasses_;
    std::set<std::string> baseClasses_;

    /// \brief Map BBFM primitive type to Rust type
    /// \param type The primitive type to map
    /// \return Rust type string
    std::string MapPrimitiveType(PrimitiveType type) const;

    /// \brief Map BBFM type to Rust type (primitive or user-defined)
    /// \param typeSpec The type specification
    /// \param cardinality The cardinality modifier (optional)
    /// \return Rust type string
    std::string MapType(const TypeSpec* typeSpec, const CardinalityModifier* cardinality = nullptr) const;

    /// \brief Generate file header with imports and common types
    /// \param filename The output filename
    void GenerateFileHeader(const std::string& filename);

    /// \brief Generate file footer (empty for Rust)
    void GenerateFileFooter();

    /// \brief Generate an enum declaration
    /// \param enumDecl The enum declaration from AST
    void GenerateEnum(const EnumDeclaration* enumDecl);

    /// \brief Generate a class as struct or enum
    /// \param classDecl The class declaration from AST
    void GenerateClass(const ClassDeclaration* classDecl);

    /// \brief Generate enum with variants for inherited classes
    /// \param baseClass The base class declaration
    void GenerateInheritedEnum(const ClassDeclaration* baseClass);

    /// \brief Generate base struct for shared fields
    /// \param baseClass The base class declaration
    void GenerateBaseStruct(const ClassDeclaration* baseClass);

    /// \brief Generate struct for standalone class
    /// \param classDecl The class declaration
    void GenerateStruct(const ClassDeclaration* classDecl);

    /// \brief Generate impl block for class
    /// \param classDecl The class declaration
    void GenerateImpl(const ClassDeclaration* classDecl);

    /// \brief Generate impl block for inherited enum
    /// \param baseClass The base class declaration
    void GenerateInheritedEnumImpl(const ClassDeclaration* baseClass);

    /// \brief Generate getter method for a field
    /// \param field The field to generate getter for
    /// \param className The containing class name
    /// \param isInEnum Whether this is for an enum variant
    void GenerateGetter(const Field* field, const std::string& className, bool isInEnum);

    /// \brief Generate setter method for a field
    /// \param field The field to generate setter for
    /// \param classDecl The class declaration containing the field
    /// \param isInEnum Whether this is for an enum variant
    void GenerateSetter(const Field* field, const ClassDeclaration* classDecl, bool isInEnum);

    /// \brief Generate computed feature method
    /// \param field The computed field
    /// \param classDecl The class declaration
    /// \param isInEnum Whether this is for an enum variant
    void GenerateComputedGetter(const Field* field, const ClassDeclaration* classDecl, bool isInEnum);

    /// \brief Get invariants that reference a specific field
    /// \param fieldName The name of the field
    /// \param classDecl The class declaration
    /// \return Vector of invariants that reference the field
    std::vector<const Invariant*> GetInvariantsForField(const std::string& fieldName, const ClassDeclaration* classDecl) const;

    /// \brief Convert expression to Rust code
    /// \param expr The expression to convert
    /// \param prefix Optional prefix for field references (e.g., "self.")
    /// \return Rust code string
    std::string ExpressionToRust(const Expression* expr, const std::string& prefix = "self.") const;

    /// \brief Apply class prefix to name if prefix is set
    /// \param name The original name
    /// \return Prefixed name or original if no prefix
    std::string ApplyClassPrefix(const std::string& name) const;

    /// \brief Convert camelCase or PascalCase to snake_case
    /// \param name The name to convert
    /// \return snake_case name
    std::string ToSnakeCase(const std::string& name) const;

    /// \brief Build inheritance hierarchy map
    void BuildInheritanceMap();

    /// \brief Check if a class is a base class (has derived classes)
    /// \param className The class name to check
    /// \return True if class has derived classes
    bool IsBaseClass(const std::string& className) const;

    /// \brief Write indentation
    /// \param level Indentation level (each level = 4 spaces)
    void WriteIndent(int level);
};
} // namespace bbfm

// Restore previous alignment
#pragma pack(pop)

#endif // __BBFM_RUST_CODE_GENERATOR_H_INCL__
