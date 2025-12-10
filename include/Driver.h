#ifndef __BBFM_DRIVER_H_INCL__
#define __BBFM_DRIVER_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

#include "AST.h"
#include "SemanticAnalyzer.h"
#include <memory>
#include <string>
#include <vector>

namespace bbfm {
/// \brief Main driver for the BBFM compiler
///
/// The Driver class orchestrates the compilation phases:
/// - Phase 0: Lexical analysis and parsing (AST construction)
/// - Phase 1: Semantic analysis
/// - Phase 2: Code generation (to be implemented)
class Driver
{
public:
    /// \brief Construct a driver with source files
    /// \param sourceFiles Vector of source file paths to compile
    /// \param targetClassPrefix Prefix to add to generated class and enum names (optional)
    /// \param targetNamespace Target namespace for generated code (optional)
    explicit Driver(std::vector<std::string> sourceFiles, const std::string& targetClassPrefix = "", const std::string& targetNamespace = "");

    /// \brief Destructor
    virtual ~Driver() = default;

    /// \brief Phase 0: Lexical analysis and parsing
    ///
    /// Parses all source files and constructs the Abstract Syntax Tree.
    /// \return Unique pointer to the constructed AST (nullptr on failure)
    std::unique_ptr<AST> Phase0();

    /// \brief Phase 1: Semantic analysis
    ///
    /// Performs semantic analysis on the AST including type checking,
    /// symbol table construction, and validation.
    /// \param ast Pointer to the AST to analyze
    /// \return Unique pointer to the semantic analyzer (nullptr on failure)
    std::unique_ptr<SemanticAnalyzer> Phase1(const AST* ast);

    /// \brief Phase 2: Code generation
    ///
    /// Generates C++ code from the validated AST and symbol table.
    /// \param ast Pointer to the validated AST
    /// \param analyzer Pointer to the semantic analyzer
    /// \param outputPath Path to the output file
    /// \return True if code generation succeeded, false on failure
    bool Phase2(const AST* ast, const SemanticAnalyzer* analyzer, const std::string& outputPath);

    /// \brief Check if compilation has encountered errors
    /// \return True if errors were encountered
    bool HasErrors() const;

    /// \brief Get the target class prefix for code generation
    /// \return The target class prefix string
    const std::string& GetTargetClassPrefix() const;

    /// \brief Get the target namespace for code generation
    /// \return The target namespace string
    const std::string& GetTargetNamespace() const;

    /// \brief Get combined namespaces from command-line and source
    /// \param ast Pointer to the AST containing source namespace
    /// \return Vector of namespace names (outer to inner)
    std::vector<std::string> GetCombinedNamespaces(const AST* ast) const;

private:
    std::vector<std::string> sourceFiles_;
    std::string              targetClassPrefix_;
    std::string              targetNamespace_;
    bool                     hasErrors_;
};
} // namespace bbfm

// Restore previous alignment
#pragma pack(pop)

#endif // __BBFM_DRIVER_H_INCL__
