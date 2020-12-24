#include "utils.h"

#include <stdlib.h>

#include "flex_bison/parser.h"


// Parser

void n_eval_stmt()
{}

// Scanner

size_t n_col;
size_t n_indent;
size_t n_line;

typedef struct n_IndentStack n_IndentStack;
struct n_IndentStack {
    size_t         indent;
    n_IndentStack* prev;
};

n_IndentStack* n_indent_stack;

void n_indent_pop()
{
    n_IndentStack* s = n_indent_stack;
    n_indent         = s->indent;
    n_indent_stack   = s->prev;
    free(s);
}

void n_indent_push()
{
    n_IndentStack* prev = malloc(sizeof(n_IndentStack));
    prev->indent   = n_indent;
    prev->prev     = n_indent_stack;
    n_indent_stack = prev;
    n_indent       = n_col;
}

// Other

void n_init()
{
    n_col          = 0;
    n_indent       = 0;
    n_indent_stack = 0;
    n_line         = 1;
}