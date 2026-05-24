#include "CodeGenerator.h"
#include "Console.h"

namespace bbfm {
// ============================================================================
// CodeGenerator Implementation
// ============================================================================

CodeGenerator::CodeGenerator(const AST* ast, const SemanticAnalyzer* analyzer, const std::vector<std::string>& namespaces, const std::string& classPrefix) :
    ast_(ast), analyzer_(analyzer), namespaces_(namespaces), classPrefix_(classPrefix), hasErrors_(false)
{
}

std::string CodeGenerator::FormatNamespacePrefix() const
{
    if (namespaces_.empty() == true)
    {
        return "";
    }

    std::string prefix;
    for (const auto& ns : namespaces_)
    {
        prefix += ns + "::";
    }

    return prefix;
}

bool CodeGenerator::HasErrors() const
{
    return hasErrors_;
}

void CodeGenerator::ReportError(const std::string& message)
{
    Console::ReportError(message);
    hasErrors_ = true;
}
} // namespace bbfm
