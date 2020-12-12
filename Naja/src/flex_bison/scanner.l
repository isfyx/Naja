%option noyywrap
%{
#include "parser.h"
%}

%%

[0-9]+ { yylval = atoi(yytext); return NUMBER; }

%%