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
        cxxopts::Options options("model-compiler", "BBFM Model Compiler - Compiles .fm source files to C++");

        options.add_options()("h,help", "Print usage information")("v,version", "Print version information")(
            "dump-syntax-tree", "Dump the Abstract Syntax Tree after lexical analysis")("dump-symbol-table", "Dump the Symbol Table after semantic analysis")(
            "o,output", "Output file path (default: input filename with .h extension)",
            cxxopts::value<std::string>()->default_value(""))(
            "target-class-prefix", "Prefix to add to generated class and enum names",
            cxxopts::value<std::string>()->default_value(""))(
            "target-namespace", "Target namespace for generated code",
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

        // Get target class prefix option
        std::string targetClassPrefix = result["target-class-prefix"].as<std::string>();

        // Get target namespace option
        std::string targetNamespace = result["target-namespace"].as<std::string>();

        // Create driver with source files
        bbfm::Driver driver(sourceFiles, targetClassPrefix, targetNamespace);

        // Report target class prefix if set
        if (false == targetClassPrefix.empty())
        {
            bbfm::Console::ReportStatus("Target class prefix: " + targetClassPrefix);
        }

        // Report target namespace if set
        if (false == targetNamespace.empty())
        {
            bbfm::Console::ReportStatus("Target namespace: " + targetNamespace);
        }

        // Phase 0: Lexical analysis and parsing
        std::unique_ptr<bbfm::AST> ast = driver.Phase0();
        if (nullptr == ast)
        {
            return 1;
        }

        // Dump the AST if requested
        if (result.count("dump-syntax-tree"))
        {
            std::cout << "\n";

            // Get combined namespaces and format as prefix
            std::vector<std::string> namespaces = driver.GetCombinedNamespaces(ast.get());
            std::string nsPrefix;
            for (const auto& ns : namespaces)
            {
                nsPrefix += ns + "::";
            }

            ast->Dump(0, nsPrefix);
        }

        // Phase 1: Semantic analysis
        std::unique_ptr<bbfm::SemanticAnalyzer> analyzer = driver.Phase1(ast.get());
        if (nullptr == analyzer)
        {
            return 1;
        }

        // Dump the symbol table if requested
        if (result.count("dump-symbol-table"))
        {
            std::cout << "\n";
            analyzer->DumpSymbolTable();
        }

        // Phase 2: Code generation
        std::string outputPath = result["output"].as<std::string>();

        // If no output path specified, derive from input filename
        if (outputPath.empty())
        {
            const std::string& inputFile = sourceFiles[0];

            // Replace .fm extension with .h
            size_t lastDot = inputFile.find_last_of('.');
            if (std::string::npos != lastDot)
            {
                outputPath = inputFile.substr(0, lastDot) + ".h";
            }
            else
            {
                outputPath = inputFile + ".h";
            }
        }

        // Generate C++ code
        if (!driver.Phase2(ast.get(), analyzer.get(), outputPath))
        {
            return 1;
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
