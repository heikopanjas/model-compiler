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

    Kind                    kind;
    std::string             name;
    const EnumDeclaration*  enumDecl;  // Non-null if kind == ENUM
    const ClassDeclaration* classDecl; // Non-null if kind == CLASS

    /// \brief Construct a primitive type symbol
    /// \param typeName The primitive type name
    TypeSymbol(const std::string& typeName) : kind(Kind::PRIMITIVE), name(typeName), enumDecl(nullptr), classDecl(nullptr) {}

    /// \brief Construct an enum type symbol
    /// \param enumDeclaration Pointer to the enum declaration
    TypeSymbol(const EnumDeclaration* enumDeclaration) : kind(Kind::ENUM), name(enumDeclaration->GetName()), enumDecl(enumDeclaration), classDecl(nullptr) {}

    /// \brief Construct a class type symbol
    /// \param classDeclaration Pointer to the class declaration
    TypeSymbol(const ClassDeclaration* classDeclaration) : kind(Kind::CLASS), name(classDeclaration->GetName()), enumDecl(nullptr), classDecl(classDeclaration)
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

    /// \brief Dump the symbol table to stdout
    void DumpSymbolTable() const;

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

    /// \brief Validate a single class declaration
    /// \param classDecl The class declaration to validate
    /// \return True if valid, false if errors found
    bool ValidateClassDeclaration(const ClassDeclaration* classDecl);

    /// \brief Check for cycles in inheritance chain
    /// \param className Name of the class to check
    /// \param visited Set of visited class names for cycle detection
    /// \return True if cycle detected, false otherwise
    bool HasInheritanceCycle(const std::string& className, std::set<std::string>& visited);

    /// \brief Get all fields for a class including inherited fields
    /// \param classDecl The class declaration
    /// \param allFields Output vector to store all fields
    void GetAllFields(const ClassDeclaration* classDecl, std::vector<const Field*>& allFields) const;

    /// \brief Helper function to get all fields with cycle detection
    /// \param classDecl The class declaration
    /// \param allFields Output vector to store all fields
    /// \param visited Set of visited class names for cycle detection
    void GetAllFieldsHelper(const ClassDeclaration* classDecl, std::vector<const Field*>& allFields, std::set<std::string>& visited) const;

    /// \brief Get all invariants for a class including inherited invariants
    /// \param classDecl The class declaration
    /// \param allInvariants Output vector to store all invariants
    void GetAllInvariants(const ClassDeclaration* classDecl, std::vector<const Invariant*>& allInvariants) const;

    /// \brief Helper function to get all invariants with cycle detection
    /// \param classDecl The class declaration
    /// \param allInvariants Output vector to store all invariants
    /// \param visited Set of visited class names for cycle detection
    void GetAllInvariantsHelper(const ClassDeclaration* classDecl, std::vector<const Invariant*>& allInvariants, std::set<std::string>& visited) const;

    /// \brief Validate field uniqueness within a class
    /// \param classDecl The class declaration to validate
    /// \return True if all fields are unique, false otherwise
    bool ValidateFieldUniqueness(const ClassDeclaration* classDecl);

    /// \brief Validate invariants for a class declaration
    /// \param classDecl The class declaration to validate
    /// \return True if all invariants are valid, false otherwise
    bool ValidateInvariants(const ClassDeclaration* classDecl);

    /// \brief Collect all field references from an expression
    /// \param expr The expression to analyze
    /// \param fields Output set to store field names
    void CollectFieldReferences(const Expression* expr, std::set<std::string>& fields) const;

    /// \brief Validate computed features for a class declaration
    /// \param classDecl The class declaration to validate
    /// \return True if all computed features are valid, false otherwise
    bool ValidateComputedFeatures(const ClassDeclaration* classDecl);

    /// \brief Validate a single computed feature expression
    /// \param field The field with computed feature
    /// \param classDecl The containing class
    /// \param availableFields Set of fields available for reference
    /// \return True if valid, false otherwise
    bool ValidateComputedFeatureExpression(const Field* field, const ClassDeclaration* classDecl, const std::set<std::string>& availableFields);

    /// \brief Validate member access expression
    /// \param memberAccess The member access expression
    /// \param classDecl The containing class
    /// \param errorContext Context string for error messages
    /// \return True if valid, false otherwise
    bool ValidateMemberAccess(const MemberAccessExpression* memberAccess, const ClassDeclaration* classDecl, const std::string& errorContext);

    /// \brief Validate member access in an expression recursively
    /// \param expr The expression to validate
    /// \param classDecl The containing class
    /// \param errorContext Context string for error messages
    /// \return True if valid, false otherwise
    bool ValidateMemberAccessInExpression(const Expression* expr, const ClassDeclaration* classDecl, const std::string& errorContext);

    /// \brief Get the type of a field by name in a class
    /// \param classDecl The class to search
    /// \param fieldName The field name
    /// \return Pointer to TypeSymbol or nullptr if not found
    const TypeSymbol* GetFieldType(const ClassDeclaration* classDecl, const std::string& fieldName) const;

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
