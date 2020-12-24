%{
#include "parser.h"
#include "Naja/utils.h"
%}
%option noyywrap
%array
%x SC_INDENT SC_DEDENT

newline \n[ \t]*

%%

{newline} {{
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
}}

<SC_INDENT>" "*. {
    printf("Scanning INDENT\n");
    n_indent_push();
    printf("'%s'%d\n", yytext, strlen(yytext));
    for (size_t i = strlen(yytext); i > 0; i--)
        unput(yytext[i-1]);
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
    else if (n_col == n_indent)
        BEGIN INITIAL;
    return DEDENT;
}

<*># {{ // Strip comments
    printf("Scanning comment\n");
    char c;
    while ((c = input()) && c != '\n');
    unput(c);
}}


. { // UNRECOGNIZED
        printf( "Scanner error (%lu,%lu): Unrecognized character '%s'\n"
               , n_line, n_col, yytext
        );
        n_col++;
    }

%%