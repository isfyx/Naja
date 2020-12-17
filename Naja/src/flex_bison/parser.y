%{
#include <stdio.h>
#include <stdlib.h>

#include "Naja/utils.h"

int  yylex();
void yyerror(const char *s);

extern int yylineno;
%}

%code requires { #include "Naja/value.h" }

%union {
    long     lval;
    double   dval;
    char     sval[256];
    n_value  nval;
}

%token OR AND NOT
%token EQ GEQ LEQ NEQ GT LT

%token PLUS DASH SLASH AST PERCENT DAST
%token LPAR RPAR

%token EOL END_OF_FILE

%token<nval> INT
%token<dval> FLOAT

%type<sval> statement terminator
%type<lval> expression
%type<lval> disjuction conjuction negation comparrison
%type<lval> value term factor exp
%type<nval> atom

%%

program: statements;

statements:
          | statements statement    { printf("[%s:%04d] -> %s\n", N_FILENAME, yylineno, $2); }
          ;

statement:            terminator
         | expression terminator    { sprintf($$, "%ld", $1); }
         ;

terminator: EOL             { sprintf($$, "EMPTY_LINE"); }
          | END_OF_FILE     { sprintf($$, "END_OF_FILE"); }
          ;

expression: disjuction;

disjuction:               conjuction 
          | disjuction OR conjuction    { $$ = $1 ? $1 : $3; }
          ;

conjuction:                negation
          | conjuction AND negation     { $$ = !$1 ? $1 : $3; }
          ;

negation:     comparrison
        | NOT comparrison   { $$ = !$2; }
        ;

comparrison:           value
           | value EQ  value    { $$ = $1 == $3; }
           | value GEQ value    { $$ = $1 >= $3; }
           | value LEQ value    { $$ = $1 <= $3; }
           | value NEQ value    { $$ = $1 != $3; }
           | value GT  value    { $$ = $1 >  $3; }
           | value LT  value    { $$ = $1 <  $3; }
           ;

value:            factor
     | value PLUS factor { $$ = $1 + $3; }
     | value DASH factor { $$ = $1 - $3; }
     ;

factor:                term
      | factor AST     term   { $$ = $1 * $3; }
      | factor SLASH   term   { $$ = $3 != 0 ? $1 / $3 : 0; }
      | factor PERCENT term   { $$ = $1 % $3; }
      ;

term:      exp
    | PLUS exp      { $$ = $2;  }
    | DASH exp      { $$ = -$2; }
    ;

exp: atom
   | atom DAST atom     { { int p = 1;
                            for (int i = 0; i < $3; i++)
                                p *= $1;
                            $$ = p;
                        } }
   ;

atom: INT
    | FLOAT                     { $$ = (long)$1; }
    | LPAR expression RPAR      { $$ = $2;       }
    ;

%%

void yyerror(const char *s)
{
    fprintf(stderr, "error: %s\n", s);
}