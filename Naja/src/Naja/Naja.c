#include <stdio.h>

#include "flex_bison/parser.h"

#include "utils.h"

extern FILE* yyin;
extern int yylineno;

extern n_FileStack* n_filestack;

int main(int argc, char** argv)
{
    yylineno    = 0;
    n_filestack = 0;

    if (argc > 1 && !(yyin = fopen(argv[1], "r"))) {
        perror(argv[1]);
        return 1;
    }
    yyparse();
}