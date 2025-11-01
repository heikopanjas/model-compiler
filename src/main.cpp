#include "Driver.h"
#include <iostream>
#include <memory>
#include <vector>

int main(int argc, char* argv[])
{
    // Parse command line arguments
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <source_file>\n";
        return 1;
    }

    // Collect source files from command line
    std::vector<std::string> sourceFiles;
    for (int i = 1; i < argc; ++i)
    {
        sourceFiles.push_back(argv[i]);
    }

    // Create driver with source files
    bbfm::Driver driver(sourceFiles);

    // Phase 0: Lexical analysis and parsing
    std::unique_ptr<bbfm::AST> ast = driver.Phase0();
    if (nullptr == ast)
    {
        return 1;
    }

    // Dump the AST for debugging
    std::cout << "\n";
    ast->Dump();

    // Phase 1: Semantic analysis
    std::cout << "\n";
    std::unique_ptr<bbfm::SemanticAnalyzer> analyzer = driver.Phase1(ast.get());
    if (nullptr == analyzer)
    {
        return 1;
    }

    std::cout << "\nCompilation completed successfully!\n";
    return 0;
}
