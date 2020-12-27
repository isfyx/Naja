%{
#include "parser.h"
#include "Naja/utils.h"

#define N_TOKEN_LOG(...)    printf(__VA_ARGS__)

void store_line();

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

float           [+-]?({num}*\.{num}+|{num}+\.?)([eE][+-]?{num}+)?
int             [+-]?{num}+
name            (_|{alpha})(_|{alphanumeric})*[!?]?
string          \"([^"\\]|{string_content})*\"|\'([^'\\]|{string_content})*\'

%%

    /* Handle start and end of file */

<SC_EOF><<EOF>> {
    while (n_indent) {
        n_indent_pop();
        N_TOKEN_LOG("DEDENT ");
        return DEDENT;
    }
    yyterminate();
}
<<EOF>> { BEGIN SC_EOF; N_TOKEN_LOG("EOL\n"); return EOL; }

^.* {
    for (size_t i = strlen(yytext); i > 0; i--)
        unput(yytext[i-1]);
    store_line();
}

    /* Handle keywords */

"pass"  { N_TOKEN_LOG("PASS "); return PASS; }
"def"   { N_TOKEN_LOG("DEF ");  return DEF;  }
"if"    { N_TOKEN_LOG("IF ");   return IF;   }

    /* Handle symbols */

":" { N_TOKEN_LOG(": "); return COL;  }
"(" { N_TOKEN_LOG("( "); return LPAR; }
")" { N_TOKEN_LOG(") "); return RPAR; }

    /* Handle values */

{int} {
    n_col += strlen(yytext);
    yylval = malloc(sizeof(long));
    *(long*)yylval = atol(yytext);
    N_TOKEN_LOG("INT ");
    return INT; 
}

{float} {
    n_col += strlen(yytext);
    yylval = malloc(sizeof(double));
    *(double*)yylval = atof(yytext);
    N_TOKEN_LOG("FLOAT ");
    return FLOAT; 
}

{name} {
    size_t len = strlen(yytext);
    n_col += len;
    yylval = malloc(len+1);
    strcpy(yylval, yytext);
    N_TOKEN_LOG("NAME ");
    return NAME;
}

{string} {
    size_t len = strlen(yytext);
    n_col += len;
    yylval = malloc(len+1);
    strcpy(yylval, yytext);
    N_TOKEN_LOG("STRING ");
    return STRING;
}

    /* Handle linebreaks and indentation */

{newline} {
    char c = input();
    if (c) unput(c);
    // Skip ahead if the next line is empty or a comment
    if (c && c != '#' && c != '\n') {
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

        n_lineno++;
        n_col = 0;
        store_line();
        N_TOKEN_LOG("EOL\n");
        return EOL;
    }
    n_lineno++;
    n_col = 0;
}

<SC_INDENT>[ ]* {
    n_col = strlen(yytext);
    for (size_t i = 0; i < n_col; i++)
        unput(' ');
    n_indent_push();
    n_col = 0;
    BEGIN INITIAL;
    N_TOKEN_LOG("INDENT ");
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
    N_TOKEN_LOG("DEDENT ");
    return DEDENT;
}

    /* Strip comments and whitespace */

<*># {
    char c;
    while ((c = input()) && c != '\n');
    unput(c);
}

[ \t] { n_col += *yytext == '\t' ? N_TAB_WIDTH : 1; }

    /* Handle unchaught characters */

. {
    if (!*yytext) {
        BEGIN SC_EOF;
        N_TOKEN_LOG("EOL\n");
        return EOL;
    }
    n_col++;
    N_TOKEN_LOG("0x%02x ", *yytext);
    return *yytext;
}

%%

void store_line()
{
    if (!n_line)
        n_line = malloc(YYLMAX);
    
    char c   = input();
    size_t i = 0;
    while(c && c != '\n') {
        n_line[i++] = c;
        c = input();
    }   n_line[i] = 0;
        unput(c);

    for (i = strlen(n_line); i > 0; i--)
        unput(n_line[i-1]);

    unput(input()); // Prevent matching ^
}