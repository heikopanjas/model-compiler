#ifndef __RUNTIME_I_CODE_GENERATOR_H_INCL__
#define __RUNTIME_I_CODE_GENERATOR_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

#include "runtime/AST.h"
#include "runtime/SemanticAnalyzer.h"
#include "runtime/Array.h"
#include "runtime/String.h"

namespace runtime {
/// \brief Interface for code generators
///
/// This class provides the interface for generating code in various target
/// languages from the validated AST and symbol table. Concrete implementations
/// (CppCodeGenerator, SwiftCodeGenerator, etc.) derive from this class and
/// implement the language-specific generation logic.
///
/// The code generator takes:
/// - Validated AST with all type information
/// - Semantic analyzer with complete symbol table
/// - Namespace hierarchy for code organization
/// - Optional class prefix for generated class names
class ICodeGenerator
{
public:
    /// \brief Construct a code generator
    /// \param ast Pointer to the validated AST
    /// \param analyzer Pointer to the semantic analyzer with symbol table
    /// \param namespaces Vector of namespace names (outer to inner)
    /// \param classPrefix Optional prefix to add to generated class and enum names
    ICodeGenerator(const AST* ast, const SemanticAnalyzer* analyzer, const Array<String>& namespaces, const String& classPrefix);

    /// \brief Virtual destructor
    virtual ~ICodeGenerator() = default;

    /// \brief Generate code and write to output file
    /// \param outputPath The path to the output file
    /// \return True if generation succeeded, false on error
    virtual bool Generate(const String& outputPath) = 0;

    /// \brief Get the file extension for generated files
    /// \return File extension including the dot (e.g., ".h", ".cpp", ".swift")
    virtual String GetFileExtension() const = 0;

    /// \brief Get the target language name
    /// \return Name of the target language (e.g., "C++", "Swift")
    virtual String GetLanguageName() const = 0;

protected:
    const AST*               ast_;
    const SemanticAnalyzer*  analyzer_;
    Array<String> namespaces_;
    String                   classPrefix_;

    /// \brief Format namespace prefix string
    /// \return Namespace prefix (e.g., "ns1::ns2::" or "" if no namespaces)
    String FormatNamespacePrefix() const;

    /// \brief Check if code generator has errors
    /// \return True if errors occurred during generation
    bool HasErrors() const;

    /// \brief Report an error during code generation
    /// \param message The error message
    void ReportError(const String& message);

private:
    bool hasErrors_;
};
} // namespace runtime

// Restore previous alignment
#pragma pack(pop)

#endif // __RUNTIME_I_CODE_GENERATOR_H_INCL__
