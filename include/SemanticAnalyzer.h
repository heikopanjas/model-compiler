#ifndef __BBFM_SEMANTIC_ANALYZER_H_INCL__
#define __BBFM_SEMANTIC_ANALYZER_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

#include "AST.h"
#include <map>
#include <set>
#include <string>
#include <vector>

namespace bbfm {
/// \brief Symbol table entry for a declared type
struct TypeSymbol
{
    enum class Kind
    {
        ENUM,
        CLASS,
        PRIMITIVE
    };

    Kind                     kind;
    std::string              name;
    const EnumDeclaration*   enumDecl;   // Non-null if kind == ENUM
    const FabricDeclaration* fabricDecl; // Non-null if kind == CLASS

    /// \brief Construct a primitive type symbol
    /// \param typeName The primitive type name
    TypeSymbol(const std::string& typeName) : kind(Kind::PRIMITIVE), name(typeName), enumDecl(nullptr), fabricDecl(nullptr) {}

    /// \brief Construct an enum type symbol
    /// \param enumDeclaration Pointer to the enum declaration
    TypeSymbol(const EnumDeclaration* enumDeclaration) : kind(Kind::ENUM), name(enumDeclaration->GetName()), enumDecl(enumDeclaration), fabricDecl(nullptr) {}

    /// \brief Construct a class type symbol
    /// \param classDeclaration Pointer to the fabric declaration
    TypeSymbol(const FabricDeclaration* classDeclaration) :
        kind(Kind::CLASS), name(classDeclaration->GetName()), enumDecl(nullptr), fabricDecl(classDeclaration)
    {
    }
};

/// \brief Semantic analyzer for BBFM language
///
/// Performs semantic analysis including:
/// - Symbol table construction
/// - Type validation
/// - Inheritance cycle detection
/// - Field uniqueness checking
/// - Invariant validation
class SemanticAnalyzer
{
public:
    /// \brief Construct a semantic analyzer
    /// \param ast Pointer to the AST to analyze
    explicit SemanticAnalyzer(const AST* ast);

    /// \brief Destructor
    virtual ~SemanticAnalyzer() = default;

    /// \brief Perform semantic analysis on the AST
    /// \return True if analysis succeeded, false if errors occurred
    bool Analyze();

    /// \brief Check if errors were encountered during analysis
    /// \return True if errors occurred
    bool HasErrors() const;

    /// \brief Get the symbol table (for code generation phase)
    /// \return Reference to the symbol table
    const std::map<std::string, TypeSymbol>& GetSymbolTable() const;

private:
    const AST*                        ast_;
    std::map<std::string, TypeSymbol> symbolTable_;
    bool                              hasErrors_;

    /// \brief Register primitive types in symbol table
    void RegisterPrimitiveTypes();

    /// \brief Build symbol table from AST declarations
    /// \return True if successful, false if errors occurred
    bool BuildSymbolTable();

    /// \brief Validate all type references in declarations
    /// \return True if successful, false if errors occurred
    bool ValidateTypeReferences();

    /// \brief Validate a single fabric declaration
    /// \param fabric The fabric declaration to validate
    /// \return True if valid, false if errors found
    bool ValidateFabricDeclaration(const FabricDeclaration* fabric);

    /// \brief Check for cycles in inheritance chain
    /// \param className Name of the class to check
    /// \param visited Set of visited class names for cycle detection
    /// \return True if cycle detected, false otherwise
    bool HasInheritanceCycle(const std::string& className, std::set<std::string>& visited);

    /// \brief Get all fields for a class including inherited fields
    /// \param fabric The fabric declaration
    /// \param allFields Output vector to store all fields
    void GetAllFields(const FabricDeclaration* fabric, std::vector<const Field*>& allFields);

    /// \brief Helper function to get all fields with cycle detection
    /// \param fabric The fabric declaration
    /// \param allFields Output vector to store all fields
    /// \param visited Set of visited class names for cycle detection
    void GetAllFieldsHelper(const FabricDeclaration* fabric, std::vector<const Field*>& allFields, std::set<std::string>& visited);

    /// \brief Validate field uniqueness within a class
    /// \param fabric The fabric declaration to validate
    /// \return True if all fields are unique, false otherwise
    bool ValidateFieldUniqueness(const FabricDeclaration* fabric);

    /// \brief Validate invariants for a fabric declaration
    /// \param fabric The fabric declaration to validate
    /// \return True if all invariants are valid, false otherwise
    bool ValidateInvariants(const FabricDeclaration* fabric);

    /// \brief Check if a type exists in symbol table
    /// \param typeName The type name to check
    /// \return True if type exists, false otherwise
    bool TypeExists(const std::string& typeName) const;

    /// \brief Look up a type in the symbol table
    /// \param typeName The type name to look up
    /// \return Pointer to type symbol or nullptr if not found
    const TypeSymbol* LookupType(const std::string& typeName) const;

    /// \brief Report a semantic error
    /// \param message The error message
    void ReportError(const std::string& message);
};
} // namespace bbfm

// Restore previous alignment
#pragma pack(pop)

#endif // __BBFM_SEMANTIC_ANALYZER_H_INCL__
