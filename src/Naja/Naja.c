#include "utils.h"
#include "flex_bison/parser.h"

extern int yylex_destroy();
extern FILE* yyin;

int main(int argc, char const *argv[])
{
    n_init();
    if (argc > 1)
        yyin = fopen(argv[1], "r");
    yyparse();
    if (argc > 1)
        fclose(yyin);
    yylex_destroy();
    return 0;
}
