#include "value.h"

extern YYSTYPE yylval;
extern char*   yytext;

n_value n_new_int() {
    n_value value;
    value.type     = INT;
    value.size     = sizeof(long int);
    value.value    = malloc(value.size);
    *value.value   = atol(yytext);
    return n_value;
}