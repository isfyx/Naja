%option noyywrap

%x CLOSE

%{
#include "Naja/utils.h"

#include "parser.h"

extern n_FileStack* n_fs;
%}

number      [0-9]+
decimal     ({number}?\.?{number}|{number}\.)
exponent    ([eE]("+"|"-")?{number})

alpha           [a-zA-Z]
alphanumeric    [{alpha}{number}]
word            [_{alpha}]+[_{alphanumeric}]*

whitespace [ \t]

%%

<CLOSE><<EOF>> { yyterminate(); }

<<EOF>> {
    yylineno++;
    BEGIN CLOSE;
    return END_OF_FILE;
}

{number} {
    yylval.nval.type  = INT;
    yylval.nval.size  = sizeof(long int);
    yylval.nval.value = malloc(yylval.nval.size);
    *(yylval.nval.value) = atol(yytext); 
    return INT;
}

{decimal}{exponent}? {
    yylval.dval = atof(yytext);
    return FLOAT;
}

"or"    { return OR;  }
"and"   { return AND; }
"not"   { return NOT; }

"=="    { return EQ;  }
">="    { return GEQ; }
"<="    { return LEQ; }
"!="    { return NEQ; }
">"     { return GT;  }
"<"     { return LT;  }

"+"     { return PLUS;    }
"-"     { return DASH;    }
"*"     { return AST;     }
"/"     { return SLASH;   }
"%"     { return PERCENT; }
"**"    { return DAST;    }

"("     { return LPAR; }
")"     { return RPAR; }

{whitespace}    { /* Ignore whitespace */ }
"#".*           { /* Ignore comments */ }
\n              { yylineno++; return EOL; }
.               { printf("Unrecognized character '%c'\n", *yytext); }

%%