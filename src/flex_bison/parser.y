%{
#include <stdio.h>

#include "Naja/utils.h"

int  yylex();
void yyerror(const char*);
%}

%define api.value.type { void* }

%token INDENT DEDENT EOL

%%

program: statements;

statements:
          | statements statement { }
          ;

statement: EOL      { }
         | INDENT   { }
         | DEDENT   { }
         ;

%%

void yyerror(const char* e)
{
    fprintf(stderr, "Parser error (%lu,%lu): %s\n", n_line, n_col, e);
}