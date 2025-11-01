#include "Driver.h"
#include "AST.h"
#include <cstdio>
#include <iostream>

// External C functions from Flex/Bison
extern "C" {
    extern FILE* yyin;
}

extern int                        yyparse(void);
extern std::unique_ptr<bbfm::AST> g_ast;

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

    // Open the source file
    yyin = fopen(filename.c_str(), "r");
    if (!yyin)
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
    if (result != 0)
    {
        std::cerr << "Parsing failed.\n";
        hasErrors_ = true;
        return nullptr;
    }

    // Transfer ownership of AST from global to caller
    if (!g_ast)
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
