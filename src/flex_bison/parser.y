%{
#include <stdio.h>

#include "Naja/utils.h"

int  yylex();
void yyerror(const char*);
%}

%define api.value.type { void* }

    // Structure
%token INDENT DEDENT EOL
    // Values
%token INT FLOAT NAME STRING
    // Keywords
%token LAMBDA

%%

program: statements;

statements:
          | statements statement
          ;

statement: unparsed;

unparsed: EOL       { printf("EOL\n");   }
        | INDENT    { printf("INDENT "); }
        | DEDENT    { printf("DEDENT "); }
        | INT       { printf("INT ");    }
        | FLOAT     { printf("FLOAT ");  }
        | NAME      { printf("NAME ");   }
        | STRING    { printf("STRING "); }
        | ':'       { printf(": ");      }
        | '<'       { printf("< ");      }
        ;

%%

void yyerror(const char* e)
{
    fprintf(stderr, "Parser error (%lu,%lu): %s\n", n_line, n_col, e);
}