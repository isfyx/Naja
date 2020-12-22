%{
#include <stdio.h>
#include <stdlib.h>

#include "Naja/values.h"
#include "Naja/utils.h"

int  yylex();
void yyerror(const char *s);

extern n_Scope* n_current_scope;
extern int      n_lineno;
%}

%union {
    void* nval;
}

%token ASSIGN LAMBDA

%token OR AND NOT
%token EQ GEQ LEQ NEQ GT LT

%token COL
%token PLUS DASH SLASH DSLASH AST PERCENT DAST
%token LPAR RPAR

%token EOL END_OF_FILE

%token<nval> INT FLOAT BOOL N_NULL STRING NAME

%type<nval> statement terminator expression assignment lambda vars 
%type<nval> disjuction conjuction negation comparrison
%type<nval> value term factor exp
%type<nval> atom 

%%

program: statements;

statements:
          | statements statement    { n_trace(n_lineno, $2); n_destroy_value($2); }
          ;

statement:            terminator
         | expression terminator    { n_destroy_value($2); }
         ;

terminator: EOL             { $$ = n_new_null(n_current_scope); }
          | END_OF_FILE     { $$ = n_new_null(n_current_scope); }
          ;

expression: lambda
          | assignment
          | disjuction
          ;

lambda: LAMBDA vars COL expression     { $$ = n_new_lambda($2, $4); n_destoy_vars($2); n_destoy_value($4); }

vars:               { $$ = 0; }
    | vars NAME
    ;

assignment: NAME ASSIGN expression  { $$ = n_assign_name(n_current_scope, $3, $1); free($1); n_destroy_value($3); }
          ;

disjuction:               conjuction 
          | disjuction OR conjuction    { $$ = n_disjunction($1, $3); n_destroy_value($1); n_destroy_value($3); }
          ;

conjuction:                negation
          | conjuction AND negation     { $$ = n_conjunction($1, $3); n_destroy_value($1); n_destroy_value($3); }
          ;

negation:     comparrison
        | NOT comparrison   { $$ = n_negate($2); n_destroy_value($2); }
        ;

comparrison:           value
           | value EQ  value    { $$ = n_compare_eq($1, $3);  n_destroy_value($1); n_destroy_value($3); }
           | value GEQ value    { $$ = n_compare_geq($1, $3); n_destroy_value($1); n_destroy_value($3); }
           | value LEQ value    { $$ = n_compare_leq($1, $3); n_destroy_value($1); n_destroy_value($3); }
           | value NEQ value    { $$ = n_compare_neq($1, $3); n_destroy_value($1); n_destroy_value($3); }
           | value GT  value    { $$ = n_compare_gt($1, $3);  n_destroy_value($1); n_destroy_value($3); }
           | value LT  value    { $$ = n_compare_lt($1, $3);  n_destroy_value($1); n_destroy_value($3); }
           ;

value:            factor
     | value PLUS factor { $$ = n_add($1, $3); n_destroy_value($1); n_destroy_value($3); }
     | value DASH factor { $$ = n_sub($1, $3); n_destroy_value($1); n_destroy_value($3); }
     ;

factor:                term
      | factor AST     term   { $$ = n_mul($1, $3);        n_destroy_value($1); n_destroy_value($3); }
      | factor SLASH   term   { $$ = n_divide($1, $3);     n_destroy_value($1); n_destroy_value($3); }
      | factor DSLASH  term   { $$ = n_int_divide($1, $3); n_destroy_value($1); n_destroy_value($3); }
      | factor PERCENT term   { $$ = n_mod($1, $3);        n_destroy_value($1); n_destroy_value($3); }
      ;

term:      exp
    | PLUS exp      { $$ = n_unary_plus($2); n_destroy_value($2);  }
    | DASH exp      { $$ = n_unary_minus($2); n_destroy_value($2); }
    ;

exp: atom
   | atom DAST atom     { $$ = n_exp($1, $3); n_destroy_value($1); n_destroy_value($3); }
   ;

atom: INT
    | FLOAT
    | BOOL
    | N_NULL
    | STRING
    | NAME                  { $$ = n_copy_value(n_lookup(n_current_scope, $1)); free($1); }
    | LPAR expression RPAR  { $$ = $2; }
    ;

%%

void yyerror(const char *s)
{
    fprintf(stderr, "error: %s\n", s);
}