#include "AST.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>

extern "C" {
    extern FILE* yyin;
}

extern int                          yyparse(void);
extern std::unique_ptr<p3::Program> g_ast;

int main(int argc, char* argv[])
{
    if (argc > 1)
    {
        yyin = fopen(argv[1], "r");
        if (!yyin)
        {
            std::cerr << "Error: Could not open file '" << argv[1] << "'\n";
            return 1;
        }
    }
    else
    {
        std::cout << "Usage: " << argv[0] << " <source_file>\n";
        return 1;
    }

    int result = yyparse();

    if (yyin != stdin)
    {
        fclose(yyin);
    }

    if (result == 0 && g_ast)
    {
        std::cout << "\nParsing successful!\n\n";
        g_ast->Dump();
    }
    else
    {
        std::cerr << "Parsing failed.\n";
    }

    return result;
}
