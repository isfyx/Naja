#include <stdio.h>
#include <stdlib.h>

#include "flex_bison/parser.h"

#include "values.h"
#include "utils.h"

extern FILE* yyin;

n_Scope* n_global_scope;
n_Scope* n_current_scope;
int      n_lineno;

int main(int argc, char** argv)
{
    n_global_scope  = n_new_scope(0);
    n_current_scope = n_global_scope;
    n_lineno        = 0;

    if (argc > 1 && !(yyin = fopen(argv[1], "r"))) {
        perror(argv[1]);
        return 1;
    }
    yyparse();
    fclose(yyin);
    n_destroy_scope(n_global_scope);
}