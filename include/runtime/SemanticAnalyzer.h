#ifndef __RUNTIME_SEMANTIC_ANALYZER_H_INCL__
#define __RUNTIME_SEMANTIC_ANALYZER_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

#include "runtime/AST.h"
#include "TypeSymbol.h"
#include "runtime/Array.h"
#include "runtime/Dictionary.h"
#include "runtime/String.h"
#include <set>

namespace runtime {
/// \brief Semantic analyzer for model language
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
    /// \param namespaces Vector of namespace names (outer to inner)
    explicit SemanticAnalyzer(const AST* ast, const Array<String>& namespaces = {});

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
    const Dictionary<String, TypeSymbol>& GetSymbolTable() const;

    /// \brief Dump the symbol table to stdout
    void DumpSymbolTable() const;

    /// \brief Get all invariants for a class including inherited invariants
    /// \param classDecl The class declaration
    /// \param allInvariants Output vector to store all invariants
    void GetAllInvariants(const ClassDeclaration* classDecl, Array<const Invariant*>& allInvariants) const;

    /// \brief Collect all field references from an expression
    /// \param expr The expression to analyze
    /// \param fields Output set to store field names
    void CollectFieldReferences(const Expression* expr, std::set<String>& fields) const;

    /// \brief Find a field in a class (including inherited fields)
    /// \param classDecl The class to search
    /// \param fieldName The field name to find
    /// \return Pointer to the Field or nullptr if not found
    const Field* FindFieldInClass(const ClassDeclaration* classDecl, const String& fieldName) const;

private:
    const AST*                        ast_;
    Dictionary<String, TypeSymbol> symbolTable_;
    Array<String>                     namespaces_;
    bool                              hasErrors_;

    /// \brief Format namespace prefix for symbol names
    /// \return Namespace prefix string (e.g., "ns1::ns2::" or "" if no namespaces)
    String FormatNamespacePrefix() const;

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
    bool HasInheritanceCycle(const String& className, std::set<String>& visited);

    /// \brief Get all fields for a class including inherited fields
    /// \param classDecl The class declaration
    /// \param allFields Output vector to store all fields
    void GetAllFields(const ClassDeclaration* classDecl, Array<const Field*>& allFields) const;

    /// \brief Helper function to get all fields with cycle detection
    /// \param classDecl The class declaration
    /// \param allFields Output vector to store all fields
    /// \param visited Set of visited class names for cycle detection
    void GetAllFieldsHelper(const ClassDeclaration* classDecl, Array<const Field*>& allFields, std::set<String>& visited) const;

    /// \brief Helper function to get all invariants with cycle detection
    /// \param classDecl The class declaration
    /// \param allInvariants Output vector to store all invariants
    /// \param visited Set of visited class names for cycle detection
    void GetAllInvariantsHelper(const ClassDeclaration* classDecl, Array<const Invariant*>& allInvariants, std::set<String>& visited) const;

    /// \brief Validate field uniqueness within a class
    /// \param classDecl The class declaration to validate
    /// \return True if all fields are unique, false otherwise
    bool ValidateFieldUniqueness(const ClassDeclaration* classDecl);

    /// \brief Validate invariants for a class declaration
    /// \param classDecl The class declaration to validate
    /// \return True if all invariants are valid, false otherwise
    bool ValidateInvariants(const ClassDeclaration* classDecl);

    /// \brief Validate computed features for a class declaration
    /// \param classDecl The class declaration to validate
    /// \return True if all computed features are valid, false otherwise
    bool ValidateComputedFeatures(const ClassDeclaration* classDecl);

    /// \brief Validate an alias field
    /// \param field The alias field
    /// \param classDecl The containing class
    /// \param availableFields Set of available field names
    /// \return True if alias is valid
    bool ValidateAliasField(const Field* field, const ClassDeclaration* classDecl, const std::set<String>& availableFields);

    /// \brief Validate a single computed feature expression
    /// \param field The field with computed feature
    /// \param classDecl The containing class
    /// \param availableFields Set of fields available for reference
    /// \return True if valid, false otherwise
    bool ValidateComputedFeatureExpression(const Field* field, const ClassDeclaration* classDecl, const std::set<String>& availableFields);

    /// \brief Validate member access expression
    /// \param memberAccess The member access expression
    /// \param classDecl The containing class
    /// \param errorContext Context string for error messages
    /// \return True if valid, false otherwise
    bool ValidateMemberAccess(const MemberAccessExpression* memberAccess, const ClassDeclaration* classDecl, const String& errorContext);

    /// \brief Validate member access in an expression recursively
    /// \param expr The expression to validate
    /// \param classDecl The containing class
    /// \param errorContext Context string for error messages
    /// \return True if valid, false otherwise
    bool ValidateMemberAccessInExpression(const Expression* expr, const ClassDeclaration* classDecl, const String& errorContext);

    /// \brief Get the type of a field by name in a class
    /// \param classDecl The class to search
    /// \param fieldName The field name
    /// \return Pointer to TypeSymbol or nullptr if not found
    const TypeSymbol* GetFieldType(const ClassDeclaration* classDecl, const String& fieldName) const;

    /// \brief Infer the result type of an expression
    /// \param expr The expression to analyze
    /// \param classDecl The containing class (for field lookups)
    /// \return Expression::Type of the result
    Expression::Type InferExpressionType(const Expression* expr, const ClassDeclaration* classDecl) const;

    /// \brief Check if expression type is compatible with field type
    /// \param exprType The expression result type
    /// \param fieldTypeSpec The declared field type
    /// \return True if compatible, false otherwise
    bool IsTypeCompatible(Expression::Type exprType, const TypeSpec* fieldTypeSpec) const;

    /// \brief Convert primitive type name to Expression::Type
    /// \param typeName The primitive type name (String, Int, Real, etc.)
    /// \return Expression::Type enum value
    Expression::Type PrimitiveNameToExpressionType(const String& typeName) const;

    /// \brief Check if a type exists in symbol table
    /// \param typeName The type name to check
    /// \return True if type exists, false otherwise
    bool TypeExists(const String& typeName) const;

    /// \brief Look up a type in the symbol table
    /// \param typeName The type name to look up
    /// \return Pointer to type symbol or nullptr if not found
    const TypeSymbol* LookupType(const String& typeName) const;

    /// \brief Annotate expression with field origin markers
    /// \param expr The expression to annotate
    /// \param classDecl The containing class
    /// \param localFields Set of locally declared fields
    /// \return String representation with <base>/<self> markers
    String AnnotateExpressionWithOrigin(const Expression* expr, const ClassDeclaration* classDecl, const std::set<const Field*>& localFields) const;

    /// \brief Report a semantic error
    /// \param message The error message
    void ReportError(const String& message);
};
} // namespace runtime

// Restore previous alignment
#pragma pack(pop)

#endif // __RUNTIME_SEMANTIC_ANALYZER_H_INCL__
