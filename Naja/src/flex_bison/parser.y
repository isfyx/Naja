%{
#include <stdio.h>
int  yylex();
void yyerror(const char *s);
%}

%token NUMBER

%%

program:
       | program NUMBER
       ;

%%

void yyerror(const char *s)
{
    fprintf(stderr, "error: %s\n", s);
}