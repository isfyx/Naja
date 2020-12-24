%{
#include <stdio.h>

#include "Naja/utils.h"

int  yylex();
void yyerror(const char*);
%}

%define api.value.type { void* }

%token INT FLOAT NAME

%token ASSIGN

%token INDENT DEDENT EOL

%%

program: statements;

statements:
          | statements statement { n_eval_stmt($2); }
          ;

statement: EOL
         | assignment EOL
         | expression EOL
         ;

assignment: NAME ASSIGN expression;

expression: value;

value: INT
     | FLOAT { printf("%f\n", *(double*)$1); }
     ;

%%

void yyerror(const char* e)
{
    fprintf(stderr, "Parser error (%lu,%lu): %s\n", n_line, n_col, e);
}