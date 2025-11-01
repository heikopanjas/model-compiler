#include "Console.h"
#include "Driver.h"
#include <cxxopts.hpp>
#include <iostream>
#include <memory>
#include <vector>

int main(int argc, char* argv[])
{
    try
    {
        // Setup command line options
        cxxopts::Options options("model-compiler", "BBFM Model Compiler - Compiles .fm source files to C++ and SQL");

        options.add_options()("h,help", "Print usage information")("v,version", "Print version information")(
            "dump-ast", "Dump the Abstract Syntax Tree after parsing")("dump-symtab", "Dump the Symbol Table after semantic analysis")(
            "class-prefix", "Prefix to add to generated class and enum names",
            cxxopts::value<std::string>()->default_value(""))("input", "Input source file(s)", cxxopts::value<std::vector<std::string>>());

        options.parse_positional({"input"});
        options.positional_help("<source_file>");

        // Parse command line arguments
        auto result = options.parse(argc, argv);

        // Handle --help
        if (result.count("help"))
        {
            std::cout << options.help() << std::endl;
            return 0;
        }

        // Handle --version
        if (result.count("version"))
        {
            std::cout << "BBFM Model Compiler v0.1.0" << std::endl;
            return 0;
        }

        // Check for input files
        if (0 == result.count("input"))
        {
            bbfm::Console::ReportError("Error: No input file specified");
            std::cout << "\n" << options.help() << std::endl;
            return 1;
        }

        // Collect source files from command line
        std::vector<std::string> sourceFiles = result["input"].as<std::vector<std::string>>();

        // Get class prefix option
        std::string classPrefix = result["class-prefix"].as<std::string>();

        // Create driver with source files
        bbfm::Driver driver(sourceFiles, classPrefix);

        // Report class prefix if set
        if (false == classPrefix.empty())
        {
            bbfm::Console::ReportStatus("Class prefix: " + classPrefix);
        }

        // Phase 0: Lexical analysis and parsing
        std::unique_ptr<bbfm::AST> ast = driver.Phase0();
        if (nullptr == ast)
        {
            return 1;
        }

        // Dump the AST if requested
        if (result.count("dump-ast"))
        {
            std::cout << "\n";
            ast->Dump();
        }

        // Phase 1: Semantic analysis
        std::unique_ptr<bbfm::SemanticAnalyzer> analyzer = driver.Phase1(ast.get());
        if (nullptr == analyzer)
        {
            return 1;
        }

        // Dump the symbol table if requested
        if (result.count("dump-symtab"))
        {
            std::cout << "\n";
            analyzer->DumpSymbolTable();
        }

        bbfm::Console::ReportStatus("\nCompilation completed successfully!");
        return 0;
    }
    catch (const cxxopts::exceptions::exception& e)
    {
        bbfm::Console::ReportError("Error parsing options: " + std::string(e.what()));
        return 1;
    }
    catch (const std::exception& e)
    {
        bbfm::Console::ReportError("Error: " + std::string(e.what()));
        return 1;
    }
}
