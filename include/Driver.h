#ifndef __DRIVER_H_INCL__
#define __DRIVER_H_INCL__

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
private:
    std::vector<std::string> sourceFiles_;
    bool                     hasErrors_;

public:
    /// \brief Construct a driver with source files
    /// \param sourceFiles Vector of source file paths to compile
    explicit Driver(std::vector<std::string> sourceFiles);

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

    /// \brief Check if compilation has encountered errors
    /// \return True if errors were encountered
    bool HasErrors() const;
};
} // namespace bbfm

// Restore previous alignment
#pragma pack(pop)

#endif // __DRIVER_H_INCL__
