#include <cstdlib>
#include <fstream>
#include <iostream>

extern "C" {
    extern FILE* yyin;
}

extern int yyparse(void);

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

    return result;
}
