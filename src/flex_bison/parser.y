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
%token PASS DEF IF RAISE RETURN

%%

program: statements;

statements: %empty 
          | statements statement
          ;

statement:                   EOL
         |        block
         |        expression EOL
         | PASS              EOL
         | RETURN expression EOL
         | RAISE  expression EOL
         ;

block: head COL body;

head: DEF names
    | IF  expression
    ;

names:       NAME
     | names NAME
     ;

body: statement
    | EOL INDENT statements DEDENT
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