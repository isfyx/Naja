%{
#include "parser.h"
#include "Naja/utils.h"
%}
%option noyywrap
%array
%x SC_INDENT SC_DEDENT

num             [0-9]
alpha           [a-zA-Z]

alphanumeric    {alpha}|{num}

float   ({num}*\.{num}+|{num}+\.?)([eE][+-]?{num}+)?
name    (_|{alpha})(_|{alphanumeric})*
newline \n[ \t]*

%%

<<EOF>> {
    while (n_indent) {
        n_indent_pop();
        return DEDENT;
    }
    yyterminate();
}

"="     { n_col++; return ASSIGN; }

{num}+ {
    printf("Scanning INT\n");
    n_col += strlen(yytext);
    yylval = malloc(sizeof(long));
    *(long*)yylval = atol(yytext);
    return INT; 
}

{float} {
    printf("Scanning FLOAT\n");
    n_col += strlen(yytext);
    yylval = malloc(sizeof(double));
    *(double*)yylval = atof(yytext);
    return FLOAT; 
}

{name} {
    printf("Scanning NAME\n");
    size_t len = strlen(yytext);
    n_col += len;
    yylval = malloc(len+1);
    strcpy(yylval, yytext);
    return NAME;
}

{newline} {
    char c = input();
    // Skip ahead if the next line is empty or a comment
    if (c != '#' && c != '\n') {
        unput(c);

        n_col = strlen(yytext)-1;

        // Expand tabs
        for (size_t i = n_col; i > 0; i--)
            if (yytext[i] == '\t')
                n_col += N_TAB_WIDTH-1;

        //
        if (n_col != n_indent) {
            for (size_t i = 0; i < n_col; i++)
                unput(' ');
            BEGIN (n_col > n_indent ? SC_INDENT : SC_DEDENT);
        }

        n_line++;
        return EOL;
    }
    unput(c);
    n_line++;
}

<SC_INDENT>" "*. {
    printf("Scanning INDENT\n");
    n_indent_push();
    printf("'%s'%d\n", yytext, strlen(yytext));
    for (size_t i = strlen(yytext); i > 0; i--)
        unput(yytext[i-1]);
    n_col = 0;
    BEGIN INITIAL;
    return INDENT;
}

<SC_DEDENT>" "*. {
    printf("Scanning DEDENT\n");
    n_indent_pop();
    for (size_t i = strlen(yytext); i > 0; i--)
        unput(yytext[i-1]);
    if (n_col > n_indent)
        BEGIN SC_INDENT;
    else if (n_col == n_indent) {
        n_col = 0;
        BEGIN INITIAL;
    }
    return DEDENT;
}

<*># { // Strip comments
    printf("Scanning comment\n");
    char c;
    while ((c = input()) && c != '\n');
    unput(c);
}

[ \t] { n_col++; /* Ignore whitespace */ }

. { // UNRECOGNIZED
    fprintf( stderr
           , (' ' <= *yytext && *yytext <= '~')
           ? "%s%lu,%lu%s'%c'\n" : "%s%lu,%lu%s0x%02x\n"
           , "Scanner error ("
           , n_line, n_col
           , "): Unrecognized character "
           , *yytext
    );
    n_col++;
}

%%