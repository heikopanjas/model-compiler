#ifndef __DRIVER_H_INCL__
#define __DRIVER_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

#include "runtime/AST.h"
#include "runtime/Array.h"
#include "runtime/SemanticAnalyzer.h"
#include "runtime/String.h"
#include <memory>

class GeneratorPluginManager;

/// \brief Main driver for the model compiler
///
/// The Driver class orchestrates the compilation phases:
/// - Phase 0: Lexical analysis and parsing (runtime::AST construction)
/// - Phase 1: Semantic analysis
/// - Phase 2: Code generation (to be implemented)
class Driver
{
public:
    /// \brief Construct a driver with source files
    /// \param sourceFiles Vector of source file paths to compile
    /// \param targetClassPrefix Prefix to add to generated class and enum names (optional)
    /// \param targetNamespace Target namespace for generated code (optional)
    explicit Driver(
        runtime::Array<runtime::String> sourceFiles, const runtime::String& targetClassPrefix = runtime::String(),
        const runtime::String& targetNamespace = runtime::String());

    /// \brief Destructor
    virtual ~Driver() = default;

    /// \brief Phase 0: Lexical analysis and parsing
    ///
    /// Parses all source files and constructs the Abstract Syntax Tree.
    /// \return Unique pointer to the constructed runtime::AST (nullptr on failure)
    std::unique_ptr<runtime::AST> Phase0();

    /// \brief Phase 1: Semantic analysis
    ///
    /// Performs semantic analysis on the runtime::AST including type checking,
    /// symbol table construction, and validation.
    /// \param ast Pointer to the runtime::AST to analyze
    /// \return Unique pointer to the semantic analyzer (nullptr on failure)
    std::unique_ptr<runtime::SemanticAnalyzer> Phase1(const runtime::AST* ast);

    /// \brief Phase 2: Code generation
    ///
    /// Generates code from the validated runtime::AST and symbol table.
    /// \param ast Pointer to the validated runtime::AST
    /// \param analyzer Pointer to the semantic analyzer
    /// \param outputPath Path to the output file
    /// \param language Target language for code generation (e.g. "c++", "rust", "swift")
    /// \param pluginManager Loaded generator plugin manager
    /// \return True if code generation succeeded, false on failure
    bool Phase2(
        const runtime::AST* ast, const runtime::SemanticAnalyzer* analyzer, const runtime::String& outputPath, const runtime::String& language,
        const GeneratorPluginManager& pluginManager);

    /// \brief Check if compilation has encountered errors
    /// \return True if errors were encountered
    bool HasErrors() const;

    /// \brief Get the target class prefix for code generation
    /// \return The target class prefix string
    const runtime::String& GetTargetClassPrefix() const;

    /// \brief Get the target namespace for code generation
    /// \return The target namespace string
    const runtime::String& GetTargetNamespace() const;

    /// \brief Get combined namespaces from command-line and source
    /// \param ast Pointer to the runtime::AST containing source namespace
    /// \return Vector of namespace names (outer to inner)
    runtime::Array<runtime::String> GetCombinedNamespaces(const runtime::AST* ast) const;

private:
    runtime::Array<runtime::String> sourceFiles_;
    runtime::String                 targetClassPrefix_;
    runtime::String                 targetNamespace_;
    bool                            hasErrors_;
};

// Restore previous alignment
#pragma pack(pop)

#endif // __DRIVER_H_INCL__
