#ifndef DRIVER_H
#define DRIVER_H

#include "AST.h"
#include <memory>
#include <string>
#include <vector>

namespace bbfm {
/// \brief Main driver for the P3 compiler
///
/// The Driver class orchestrates the compilation phases:
/// - Phase 0: Lexical analysis and parsing (AST construction)
/// - Phase 1: Semantic analysis (to be implemented)
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

    /// \brief Check if compilation has encountered errors
    /// \return True if errors were encountered
    bool HasErrors() const;
};
} // namespace bbfm

#endif // DRIVER_H
