#include "Driver.h"
#include "AST.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// External C functions from Flex/Bison
extern "C" {
    extern FILE* yyin;
}

extern int yyparse(void);

// Global AST variable used for communication between parser and driver.
// This must be global because yyparse() has signature int yyparse(void)
// with no parameters or return value for the AST. The parser sets this
// variable when parsing completes, and the driver retrieves it afterward.
// This is the standard pattern for Bison parsers.
extern std::unique_ptr<bbfm::AST> g_ast;

// Global filename and source lines for error reporting
extern std::string              g_current_filename;
extern std::vector<std::string> g_source_lines;

namespace bbfm {
// ============================================================================
// Driver Implementation
// ============================================================================

Driver::Driver(std::vector<std::string> sourceFiles) : sourceFiles_(std::move(sourceFiles)), hasErrors_(false) {}

std::unique_ptr<AST> Driver::Phase0()
{
    // Currently we only support single file compilation
    // Multi-file support will be added later
    if (sourceFiles_.empty())
    {
        std::cerr << "Error: No source files provided\n";
        hasErrors_ = true;
        return nullptr;
    }

    if (sourceFiles_.size() > 1)
    {
        std::cerr << "Error: Multi-file compilation not yet supported\n";
        hasErrors_ = true;
        return nullptr;
    }

    const std::string& filename = sourceFiles_[0];

    // Set global filename for error reporting
    g_current_filename = filename;

    // Read source file lines for error reporting
    g_source_lines.clear();
    std::ifstream infile(filename);
    if (infile.is_open())
    {
        std::string line;
        while (std::getline(infile, line))
        {
            g_source_lines.push_back(line);
        }
        infile.close();
    }

    // Open the source file for parsing
    yyin = fopen(filename.c_str(), "r");
    if (nullptr == yyin)
    {
        std::cerr << "Error: Could not open file '" << filename << "'\n";
        hasErrors_ = true;
        return nullptr;
    }

    // Parse the file
    int result = yyparse();

    // Close the file
    if (yyin != stdin)
    {
        fclose(yyin);
    }

    // Check parsing result
    if (0 != result)
    {
        hasErrors_ = true;
        return nullptr;
    }

    // Transfer ownership of AST from global to caller
    if (nullptr == g_ast)
    {
        std::cerr << "Error: Parser succeeded but no AST was created\n";
        hasErrors_ = true;
        return nullptr;
    }

    std::cout << "Phase 0 (Parsing) completed successfully!\n";
    return std::move(g_ast);
}

bool Driver::HasErrors() const
{
    return hasErrors_;
}
} // namespace bbfm
