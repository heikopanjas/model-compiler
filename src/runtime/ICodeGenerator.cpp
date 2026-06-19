#include "runtime/ICodeGenerator.h"
#include "Console.h"

namespace runtime {
ICodeGenerator::ICodeGenerator(const AST* ast, const SemanticAnalyzer* analyzer, const Array<String>& namespaces, const String& classPrefix) :
    ast_(ast), analyzer_(analyzer), namespaces_(namespaces), classPrefix_(classPrefix), hasErrors_(false)
{
}

String ICodeGenerator::FormatNamespacePrefix() const
{
    if (namespaces_.GetCount() == 0)
    {
        return String();
    }

    String prefix;
    for (size_t i = 0; i < namespaces_.GetCount(); ++i)
    {
        prefix = prefix + namespaces_.GetValueAt(i) + String("::");
    }
    return prefix;
}

bool ICodeGenerator::HasErrors() const
{
    return hasErrors_;
}

void ICodeGenerator::ReportError(const String& message)
{
    Console::ReportError(message);
    hasErrors_ = true;
}
} // namespace runtime
