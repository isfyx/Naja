%option noyywrap

%x CLOSE

%{
#include "Naja/values.h"
#include "Naja/utils.h"

#include "parser.h"

extern n_Scope* n_current_scope;
extern int      n_lineno;
%}

number      [0-9]+
decimal     ({number}?\.?{number}|{number}\.)
exponent    ([eE]("+"|"-")?{number})

alpha           [a-zA-Z]
alphanumeric    ({alpha}|{number})
word            (_|{alpha})+(_|{alphanumeric})*

whitespace [ \t]

%%

<CLOSE><<EOF>> { yyterminate(); }

<<EOF>> {
    n_lineno++;
    BEGIN CLOSE;
    return END_OF_FILE;
}

{number} { // INT
    yylval.nval = n_new_int(atol(yytext));
    return INT;
}

{decimal}{exponent}? { // FLOAT
    yylval.nval = n_new_float(atof(yytext));
    return FLOAT;
}

"="     { return ASSIGN; }

"True"      { yylval.nval = n_new_boolean(1); return BOOL;   }
"False"     { yylval.nval = n_new_boolean(0); return BOOL;   }
"Null"      { yylval.nval = n_new_null(n_current_scope);       return N_NULL; }

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
"//"    { return DSLASH;  }
"%"     { return PERCENT; }
"**"    { return DAST;    }

"("     { return LPAR; }
")"     { return RPAR; }

{word} { // NAME
    yylval.nval = calloc(strlen(yytext)+1, sizeof(char));
    strcpy(yylval.nval, yytext);
    return NAME;
}

{whitespace}    { /* Ignore whitespace */ }
"#".*           { /* Ignore comments */ }
\n              { n_lineno++; return EOL; }
.               { printf("Unrecognized character '%c'\n", *yytext); }

%%