#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

extern FILE *yyin;
extern int yyparse(void);

int main(int argc, char *argv[]) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            fprintf(stderr, "Error: Could not open file '%s'\n", argv[1]);
            return 1;
        }
    } else {
        printf("Usage: %s <source_file>\n", argv[0]);
        return 1;
    }

    int result = yyparse();

    if (yyin != stdin) {
        fclose(yyin);
    }

    return result;
}
