#include "Console.h"
#include "Driver.h"
#include "GeneratorPluginManager.h"
#include "runtime/Array.h"
#include "runtime/String.h"
#include <cxxopts.hpp>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace {
std::filesystem::path GetExecutableDirectory(const char* argv0)
{
    if (nullptr == argv0 || '\0' == argv0[0])
    {
        return std::filesystem::current_path();
    }

    const std::filesystem::path executablePath = std::filesystem::absolute(argv0);
    if (executablePath.has_parent_path() == false)
    {
        return std::filesystem::current_path();
    }

    return executablePath.parent_path();
}

void ConfigurePluginManager(GeneratorPluginManager& pluginManager, const std::string& pluginDirectory, const char* argv0)
{
    if (pluginDirectory.empty() == false)
    {
        pluginManager.AddSearchDirectory(pluginDirectory);
    }

    pluginManager.AddSearchDirectory(GetExecutableDirectory(argv0));
    pluginManager.LoadPlugins();
}

void PrintAvailableLanguages(const GeneratorPluginManager& pluginManager)
{
    const runtime::Array<GeneratorLanguageInfo> languages = pluginManager.GetLanguages();

    if (languages.GetCount() == 0)
    {
        std::cout << "No generator languages found." << std::endl;
        return;
    }

    std::cout << "Available generator languages:" << std::endl;
    for (size_t i = 0; i < languages.GetCount(); ++i)
    {
        const GeneratorLanguageInfo& language = languages.GetValueAt(i);
        std::cout << "  " << language.displayName.GetValue() << " (id: " << language.languageId.GetValue()
                  << ", extension: " << language.fileExtension.GetValue();
        if (language.experimental == true)
        {
            std::cout << ", experimental";
        }
        std::cout << ", plugin: " << language.path.string() << ")" << std::endl;
    }
}
} // namespace

int main(int argc, char* argv[])
{
    try
    {
        // Setup command line options
        cxxopts::Options options("model-compiler", "Model Compiler - Compiles .fm source files to C++");

        options.add_options()("h,help", "Print usage information")("v,version", "Print version information")(
            "list-languages", "Scan generator plugins and list available target languages")(
            "dump-syntax-tree", "Dump the Abstract Syntax Tree after lexical analysis")("dump-symbol-table", "Dump the Symbol Table after semantic analysis")(
            "lang", "Target language for code generation (optional). If not specified, only syntax and semantic validation is performed.",
            cxxopts::value<std::string>())(
            "plugin-dir", "Directory containing generator shared libraries (default: directory containing model-compiler)",
            cxxopts::value<std::string>()->default_value(""))(
            "o,output", "Output file path (default: input filename with generator file extension)", cxxopts::value<std::string>()->default_value(""))(
            "target-class-prefix", "Prefix to add to generated class and enum names", cxxopts::value<std::string>()->default_value(""))(
            "target-namespace", "Target namespace for generated code",
            cxxopts::value<std::string>()->default_value(""))("input", "Input source file(s)", cxxopts::value<std::vector<std::string>>());

        options.parse_positional({"input"});
        options.positional_help("<source_file>");

        // Parse command line arguments
        auto result = options.parse(argc, argv);

        // Handle --help
        if (result.count("help") != 0)
        {
            std::cout << options.help() << std::endl;
            return 0;
        }

        // Handle --version
        if (result.count("version") != 0)
        {
            std::cout << "Model Compiler v4.1.1" << std::endl;
            return 0;
        }

        const std::string pluginDirectory = result["plugin-dir"].as<std::string>();

        if (result.count("list-languages") != 0)
        {
            GeneratorPluginManager pluginManager;
            ConfigurePluginManager(pluginManager, pluginDirectory, argv[0]);
            PrintAvailableLanguages(pluginManager);
            return 0;
        }

        // Check for input files
        if (result.count("input") == 0)
        {
            Console::ReportError("Error: No input file specified");
            std::cout << "\n" << options.help() << std::endl;
            return 1;
        }

        // Collect source files from command line
        const std::vector<std::string> sourceFiles = result["input"].as<std::vector<std::string>>();

        // Check if code generation is requested
        const bool             generateCode = (result.count("lang") > 0);
        std::string            targetLanguage;
        GeneratorPluginManager pluginManager;

        if (generateCode == true)
        {
            targetLanguage = result["lang"].as<std::string>();

            ConfigurePluginManager(pluginManager, pluginDirectory, argv[0]);

            if (pluginManager.HasLanguage(targetLanguage) == false)
            {
                std::string supportedLanguages = pluginManager.FormatSupportedLanguages();
                if (supportedLanguages.empty() == true)
                {
                    supportedLanguages = "none";
                }

                Console::ReportError("Error: No generator plugin found for language '" + targetLanguage + "'. Loaded generators: " + supportedLanguages + ".");
                return 1;
            }
        }

        // Get target class prefix option
        const std::string targetClassPrefix = result["target-class-prefix"].as<std::string>();

        // Get target namespace option
        const std::string targetNamespace = result["target-namespace"].as<std::string>();

        runtime::Array<runtime::String> runtimeSourceFiles;
        for (const auto& sourceFile : sourceFiles)
        {
            runtimeSourceFiles.AddValue(runtime::String(sourceFile));
        }

        // Create driver with source files
        Driver driver(std::move(runtimeSourceFiles), runtime::String(targetClassPrefix), runtime::String(targetNamespace));

        // Report target language if code generation is requested
        if (generateCode == true)
        {
            const std::string displayName = pluginManager.GetDisplayName(targetLanguage);
            Console::ReportStatus("Target language: " + displayName);

            if (pluginManager.IsExperimental(targetLanguage) == true)
            {
                Console::ReportStatus("WARNING: " + displayName + " code generation is experimental and may produce incomplete output.");
            }
        }
        else
        {
            Console::ReportStatus("Validation mode: Syntax and semantic checking only (no code generation)");
        }

        // Report target class prefix if set
        if (targetClassPrefix.empty() == false)
        {
            Console::ReportStatus("Target class prefix: " + targetClassPrefix);
        }

        // Report target namespace if set
        if (targetNamespace.empty() == false)
        {
            Console::ReportStatus("Target namespace: " + targetNamespace);
        }

        // Phase 0: Lexical analysis and parsing
        const std::unique_ptr<runtime::AST> ast = driver.Phase0();
        if (nullptr == ast)
        {
            return 1;
        }

        // Dump the AST if requested
        if (result.count("dump-syntax-tree") != 0)
        {
            std::cout << "\n";

            // Get combined namespaces and format as prefix
            const runtime::Array<runtime::String> namespaces = driver.GetCombinedNamespaces(ast.get());
            runtime::String                       nsPrefix;
            for (size_t i = 0; i < namespaces.GetCount(); ++i)
            {
                nsPrefix = nsPrefix + namespaces.GetValueAt(i) + runtime::String("::");
            }

            ast->Dump(0, nsPrefix);
        }

        // Phase 1: Semantic analysis
        const std::unique_ptr<runtime::SemanticAnalyzer> analyzer = driver.Phase1(ast.get());
        if (nullptr == analyzer)
        {
            return 1;
        }

        // Dump the symbol table if requested
        if (result.count("dump-symbol-table") != 0)
        {
            std::cout << "\n";
            analyzer->DumpSymbolTable();
        }

        // Phase 2: Code generation (only if --lang was specified)
        if (generateCode == true)
        {
            std::string outputPath = result["output"].as<std::string>();

            // If no output path specified, derive from input filename
            if (outputPath.empty() == true)
            {
                const std::string& inputFile = sourceFiles[0];

                const std::string outputExtension = pluginManager.GetFileExtension(targetLanguage);

                // Replace .fm extension with the generator's default extension
                const size_t lastDot = inputFile.find_last_of('.');
                if (lastDot != std::string::npos)
                {
                    outputPath = inputFile.substr(0, lastDot) + outputExtension;
                }
                else
                {
                    outputPath = inputFile + outputExtension;
                }
            }

            // Generate code
            if (driver.Phase2(ast.get(), analyzer.get(), runtime::String(outputPath), runtime::String(targetLanguage), pluginManager) == false)
            {
                return 1;
            }

            Console::ReportStatus("\nCompilation completed successfully!");
        }
        else
        {
            Console::ReportStatus("\nValidation completed successfully!");
        }

        return 0;
    }
    catch (const cxxopts::exceptions::exception& e)
    {
        Console::ReportError("Error parsing options: " + std::string(e.what()));
        return 1;
    }
    catch (const std::exception& e)
    {
        Console::ReportError("Error: " + std::string(e.what()));
        return 1;
    }
}
