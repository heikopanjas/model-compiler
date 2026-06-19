#include "Driver.h"
#include "Console.h"
#include "Contracts.h"
#include "GeneratorPluginManager.h"
#include "runtime/AST.h"
#include "runtime/SemanticAnalyzer.h"
#include <cstdio>
#include <fstream>
#include <string>

// External variables from Flex/Bison
extern FILE* yyin;
extern int   yyparse(void);

extern std::unique_ptr<runtime::AST> g_ast;

extern runtime::String              g_current_filename;
extern runtime::Array<runtime::String> g_source_lines;

namespace {
runtime::Array<runtime::String> CombineNamespaces(const runtime::String& targetNamespace, const runtime::String& sourceNamespace)
{
    runtime::Array<runtime::String> namespaces;

    if (targetNamespace.IsEmpty() == false)
    {
        namespaces.AddValue(targetNamespace);
    }

    if (sourceNamespace.IsEmpty() == false)
    {
        namespaces.AddValue(sourceNamespace);
    }

    return namespaces;
}
} // anonymous namespace

Driver::Driver(runtime::Array<runtime::String> sourceFiles, const runtime::String& targetClassPrefix, const runtime::String& targetNamespace) :
    sourceFiles_(std::move(sourceFiles)), targetClassPrefix_(targetClassPrefix), targetNamespace_(targetNamespace), hasErrors_(false)
{
}

std::unique_ptr<runtime::AST> Driver::Phase0()
{
    if (sourceFiles_.GetCount() == 0)
    {
        Console::ReportError(runtime::String("Error: No source files provided"));
        hasErrors_ = true;
        return nullptr;
    }

    if (sourceFiles_.GetCount() > 1)
    {
        Console::ReportError(runtime::String("Error: Multi-file compilation not yet supported"));
        hasErrors_ = true;
        return nullptr;
    }

    const runtime::String& filename = sourceFiles_.GetValueAt(0);

    g_current_filename = filename;

    g_source_lines.Clear();
    std::ifstream infile(filename.GetValue());
    if (infile.is_open() == true)
    {
        std::string line;
        while (std::getline(infile, line))
        {
            g_source_lines.AddValue(runtime::String(line));
        }
        infile.close();
    }

#ifdef _WIN32
    const errno_t err = fopen_s(&yyin, filename.GetValue().c_str(), "r");
    if (err != 0 || nullptr == yyin)
#else
    yyin = fopen(filename.GetValue().c_str(), "r");
    if (nullptr == yyin)
#endif
    {
        Console::ReportError(runtime::String("Error: Could not open file '") + filename + runtime::String("'"));
        hasErrors_ = true;
        return nullptr;
    }

    Console::ReportStatus(runtime::String("Phase 0 (Lexical Analysis) started..."));

    const int result = yyparse();

    if (stdin != yyin)
    {
        fclose(yyin);
    }

    if (result != 0)
    {
        hasErrors_ = true;
        return nullptr;
    }

    if (nullptr == g_ast)
    {
        Console::ReportError(runtime::String("Error: Parser succeeded but no runtime::AST was created"));
        hasErrors_ = true;
        return nullptr;
    }

    Console::ReportStatus(runtime::String("Phase 0 (Lexical Analysis) completed successfully!"));
    return std::move(g_ast);
}

std::unique_ptr<runtime::SemanticAnalyzer> Driver::Phase1(const runtime::AST* ast)
{
    if (nullptr == ast)
    {
        Console::ReportError(runtime::String("Error: Cannot perform semantic analysis on null runtime::AST"));
        hasErrors_ = true;
        return nullptr;
    }

    Console::ReportStatus(runtime::String("Phase 1 (Semantic Analysis) started..."));

    const runtime::Array<runtime::String> combinedNamespaces = GetCombinedNamespaces(ast);
    auto                                  analyzer           = std::make_unique<runtime::SemanticAnalyzer>(ast, combinedNamespaces);

    if (analyzer->Analyze() == false)
    {
        Console::ReportError(runtime::String("Phase 1 (Semantic Analysis) failed with errors."));
        hasErrors_ = true;
        return nullptr;
    }

    Console::ReportStatus(runtime::String("Phase 1 (Semantic Analysis) completed successfully!"));
    return analyzer;
}

bool Driver::Phase2(
    const runtime::AST* ast, const runtime::SemanticAnalyzer* analyzer, const runtime::String& outputPath, const runtime::String& language,
    const GeneratorPluginManager& pluginManager)
{
    RequireReturn(nullptr != ast, false);
    RequireReturn(nullptr != analyzer, false);
    RequireReturn(outputPath.IsEmpty() == false, false);
    RequireReturn(language.IsEmpty() == false, false);

    Console::ReportStatus(runtime::String("Phase 2 (Code Generation) started..."));

    const runtime::Array<runtime::String> combinedNamespaces = GetCombinedNamespaces(ast);

    GeneratorInstance generator = pluginManager.CreateGenerator(language, ast, analyzer, combinedNamespaces, targetClassPrefix_);
    if (generator.IsValid() == false)
    {
        Console::ReportError(runtime::String("Error: Could not create generator for language '") + language + runtime::String("'"));
        hasErrors_ = true;
        return false;
    }

    if (generator->Generate(outputPath) == false)
    {
        Console::ReportError(runtime::String("Phase 2 (Code Generation) failed."));
        hasErrors_ = true;
        return false;
    }

    Console::ReportStatus(runtime::String("Phase 2 (Code Generation) completed successfully!"));
    return true;
}

bool Driver::HasErrors() const
{
    return hasErrors_;
}

const runtime::String& Driver::GetTargetClassPrefix() const
{
    return targetClassPrefix_;
}

const runtime::String& Driver::GetTargetNamespace() const
{
    return targetNamespace_;
}

runtime::Array<runtime::String> Driver::GetCombinedNamespaces(const runtime::AST* ast) const
{
    return CombineNamespaces(targetNamespace_, ast->GetSourceNamespace());
}
