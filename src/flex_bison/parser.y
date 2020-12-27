%{
#include <stdio.h>

#include "Naja/utils.h"

int  yylex();
void yyerror(const char*);
%}

%define api.value.type { void* }

    // Structure
%token INDENT DEDENT EOL
    // Symbols
%token COL LPAR RPAR
    // Values
%token INT FLOAT NAME STRING
    // Keywords
%token PASS DEF IF ELSE RAISE RETURN

%%

program: statements;

statements: %empty
          | statements statement
          ;

statement: EOL    statement
         |        compound
         |        expression EOL
         | RETURN expression EOL
         | RAISE  expression EOL
         ;

compound: definition
        | conditional
        ;

block: COL EOL INDENT statements DEDENT
     ;

definition: DEF names block;

names:       NAME
     | names NAME
     ;

conditional: IF expression block
           | IF expression ELSE block
           | IF expression ELSE conditional
           ;

expression: application
          | value
          ;

application: NAME values;

values:        value
      | values value
      ;

value: INT
     | FLOAT
     | STRING
     | NAME
     | LPAR expression RPAR
     ;

%%

void yyerror(const char* e)
{
    fprintf( stderr
           , "Parser error (%lu,%lu): %s\n\n  %s\n"
           , n_lineno, n_col, e, n_line
    );
    for (size_t i = n_col+2; i > 0; i--)
        fprintf(stderr, " ");
    fprintf(stderr, "^\n");
}