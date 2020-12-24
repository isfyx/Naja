#pragma once

#define N_TAB_WIDTH 4

#include <stdio.h>

extern size_t n_col;
extern size_t n_indent;
extern size_t n_line;

// Parser
void n_eval_stmt();

// Scanner
void n_indent_pop();
void n_indent_push();

// Other
void n_init();