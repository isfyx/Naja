%{
#include "parser.h"
#include "Naja/utils.h"
%}
%option noyywrap
%array
%x SC_INDENT SC_DEDENT SC_EOF

alpha   [a-zA-Z]
newline \n[ \t]*
num     [0-9]
oct     [0-7]

alphanumeric    {alpha}|{num}
hex             {num}|[a-fA-F]
string_content  \\['"?\\abfnrtv]|\\{oct}{1,3}|\\[Xx]{hex}+|{ucn}
ucn             \\u{hex}{4}|\\U{hex}{8}

float           ({num}*\.{num}+|{num}+\.?)([eE][+-]?{num}+)?
int             {num}+
name            (_|{alpha})(_|{alphanumeric})*[!?]?
string          \"([^"\\]|{string_content})*\"|\'([^'\\]|{string_content})*\'

%%

<SC_EOF><<EOF>> {
    while (n_indent) {
        n_indent_pop();
        return DEDENT;
    }
    yyterminate();
}
<<EOF>> { BEGIN SC_EOF; return EOL; }

{int} {
    n_col += strlen(yytext);
    yylval = malloc(sizeof(long));
    *(long*)yylval = atol(yytext);
    return INT; 
}

{float} {
    n_col += strlen(yytext);
    yylval = malloc(sizeof(double));
    *(double*)yylval = atof(yytext);
    return FLOAT; 
}

{name} {
    size_t len = strlen(yytext);
    n_col += len;
    yylval = malloc(len+1);
    strcpy(yylval, yytext);
    return NAME;
}

{string} {
    size_t len = strlen(yytext);
    n_col += len;
    yylval = malloc(len+1);
    strcpy(yylval, yytext);
    return STRING;
}

{newline} {
    char c = input();
    unput(c);
    // Skip ahead if the next line is empty or a comment
    if (c != '#' && c != '\n') {
        n_col = strlen(yytext)-1;

        // Expand tabs
        for (size_t i = n_col; i > 0; i--)
            if (yytext[i] == '\t')
                n_col += N_TAB_WIDTH-1;

        // Unput whitespace
        if (n_col != n_indent) {
            for (size_t i = 0; i < n_col; i++)
                unput(' ');
            BEGIN (n_col > n_indent ? SC_INDENT : SC_DEDENT);
        }

        n_line++;
        n_col = 0;
        return EOL;
    }
    n_line++;
    n_col = 0;
}

<SC_INDENT>[ ]* {
    n_col = strlen(yytext);
    for (size_t i = 0; i < n_col; i++)
        unput(' ');
    n_indent_push();
    n_col = 0;
    BEGIN INITIAL;
    return INDENT;
}

<SC_DEDENT>[ ]* {
    n_col = strlen(yytext);
    for (size_t i = 0; i < n_col; i++)
        unput(' ');
    n_indent_pop();
    if (n_col > n_indent)
        BEGIN SC_INDENT;
    else if (n_col == n_indent) {
        n_col = 0;
        BEGIN INITIAL;
    }
    return DEDENT;
}

<*># { // Strip comments
    char c;
    while ((c = input()) && c != '\n');
    unput(c);
}

[ \t] { n_col += *yytext == '\t' ? N_TAB_WIDTH : 1; }

. {
    n_col++;
    return *yytext;
}

%%