#include "Driver.h"
#include "AST.h"
#include "Console.h"
#include "Contracts.h"
#include "CppCodeGenerator.h"
#include "RustCodeGenerator.h"
#include "SemanticAnalyzer.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// External variables from Flex/Bison
// Note: These have C++ linkage because they're declared without extern "C" in generated code
extern FILE* yyin;
extern int   yyparse(void);

// Global AST variable used for communication between parser and driver.
// This must be global because yyparse() has signature int yyparse(void)
// with no parameters or return value for the AST. The parser sets this
// variable when parsing completes, and the driver retrieves it afterward.
// This is the standard pattern for Bison parsers.
extern std::unique_ptr<bbfm::AST> g_ast;

// Global filename and source lines for error reporting
extern std::string              g_current_filename;
extern std::vector<std::string> g_source_lines;

namespace {
/// \brief Combine target and source namespaces
/// \param targetNamespace The namespace from command-line option
/// \param sourceNamespace The namespace from source file
/// \return Vector of namespace names (outer to inner)
std::vector<std::string> CombineNamespaces(const std::string& targetNamespace, const std::string& sourceNamespace)
{
    std::vector<std::string> namespaces;

    if (targetNamespace.empty() == false)
    {
        namespaces.push_back(targetNamespace);
    }

    if (sourceNamespace.empty() == false)
    {
        namespaces.push_back(sourceNamespace);
    }

    return namespaces;
}
} // anonymous namespace

namespace bbfm {
// ============================================================================
// Driver Implementation
// ============================================================================

Driver::Driver(std::vector<std::string> sourceFiles, const std::string& targetClassPrefix, const std::string& targetNamespace) :
    sourceFiles_(std::move(sourceFiles)), targetClassPrefix_(targetClassPrefix), targetNamespace_(targetNamespace), hasErrors_(false)
{
}

std::unique_ptr<AST> Driver::Phase0()
{
    // Currently we only support single file compilation
    // Multi-file support will be added later
    if (sourceFiles_.empty() == true)
    {
        Console::ReportError("Error: No source files provided");
        hasErrors_ = true;
        return nullptr;
    }

    if (sourceFiles_.size() > 1)
    {
        Console::ReportError("Error: Multi-file compilation not yet supported");
        hasErrors_ = true;
        return nullptr;
    }

    const std::string& filename = sourceFiles_[0];

    // Set global filename for error reporting
    g_current_filename = filename;

    // Read source file lines for error reporting
    g_source_lines.clear();
    std::ifstream infile(filename);
    if (infile.is_open() == true)
    {
        std::string line;
        while (std::getline(infile, line))
        {
            g_source_lines.push_back(line);
        }
        infile.close();
    }

    // Open the source file for parsing
#ifdef _WIN32
    const errno_t err = fopen_s(&yyin, filename.c_str(), "r");
    if (err != 0 || nullptr == yyin)
#else
    yyin = fopen(filename.c_str(), "r");
    if (nullptr == yyin)
#endif
    {
        Console::ReportError("Error: Could not open file '" + filename + "'");
        hasErrors_ = true;
        return nullptr;
    }

    Console::ReportStatus("Phase 0 (Lexical Analysis) started...");

    // Parse the file
    const int result = yyparse();

    // Close the file
    if (stdin != yyin)
    {
        fclose(yyin);
    }

    // Check parsing result
    if (result != 0)
    {
        hasErrors_ = true;
        return nullptr;
    }

    // Transfer ownership of AST from global to caller
    if (nullptr == g_ast)
    {
        Console::ReportError("Error: Parser succeeded but no AST was created");
        hasErrors_ = true;
        return nullptr;
    }

    Console::ReportStatus("Phase 0 (Lexical Analysis) completed successfully!");
    return std::move(g_ast);
}

std::unique_ptr<SemanticAnalyzer> Driver::Phase1(const AST* ast)
{
    if (nullptr == ast)
    {
        Console::ReportError("Error: Cannot perform semantic analysis on null AST");
        hasErrors_ = true;
        return nullptr;
    }

    Console::ReportStatus("Phase 1 (Semantic Analysis) started...");

    // Pass combined namespaces to semantic analyzer
    const std::vector<std::string> combinedNamespaces = GetCombinedNamespaces(ast);
    auto                           analyzer           = std::make_unique<SemanticAnalyzer>(ast, combinedNamespaces);

    if (analyzer->Analyze() == false)
    {
        Console::ReportError("Phase 1 (Semantic Analysis) failed with errors.");
        hasErrors_ = true;
        return nullptr;
    }

    Console::ReportStatus("Phase 1 (Semantic Analysis) completed successfully!");
    return analyzer;
}

bool Driver::Phase2(const AST* ast, const SemanticAnalyzer* analyzer, const std::string& outputPath, const std::string& language)
{
    RequireReturn(nullptr != ast, false);
    RequireReturn(nullptr != analyzer, false);
    RequireReturn(outputPath.empty() == false, false);
    RequireReturn(language.empty() == false, false);

    Console::ReportStatus("Phase 2 (Code Generation) started...");

    // Get combined namespaces
    const std::vector<std::string> combinedNamespaces = GetCombinedNamespaces(ast);

    // Create code generator based on target language
    std::unique_ptr<CodeGenerator> generator;

    if ("c++" == language)
    {
        generator = std::make_unique<CppCodeGenerator>(ast, analyzer, combinedNamespaces, targetClassPrefix_);
    }
    else if ("rust" == language)
    {
        // EXPERIMENTAL: Rust code generation is not yet complete
        Console::ReportStatus("WARNING: Rust code generation is experimental and may produce incomplete output.");
        generator = std::make_unique<RustCodeGenerator>(ast, analyzer, combinedNamespaces, targetClassPrefix_);
    }
    else
    {
        Console::ReportError("Error: Unsupported language '" + language + "'");
        hasErrors_ = true;
        return false;
    }

    // Generate code
    if (generator->Generate(outputPath) == false)
    {
        Console::ReportError("Phase 2 (Code Generation) failed.");
        hasErrors_ = true;
        return false;
    }

    Console::ReportStatus("Phase 2 (Code Generation) completed successfully!");
    return true;
}

bool Driver::HasErrors() const
{
    return hasErrors_;
}

const std::string& Driver::GetTargetClassPrefix() const
{
    return targetClassPrefix_;
}

const std::string& Driver::GetTargetNamespace() const
{
    return targetNamespace_;
}

std::vector<std::string> Driver::GetCombinedNamespaces(const AST* ast) const
{
    return CombineNamespaces(targetNamespace_, ast->GetSourceNamespace());
}
} // namespace bbfm
