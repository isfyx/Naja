#include "errors.h"

#include <stdio.h>

void n_NameError(const char* e, const char* n, int lineno)
{
    fprintf(stderr, "Type Error on line %d: %s\n", lineno, n, e);
}

void n_RuntimeError(const char* e, int lineno)
{
    fprintf(stderr, "Runtime error encountered on line %d: %s\n", lineno, e);
}

void n_TypeError(const char* e, int lineno)
{
    fprintf(stderr, "Type Error on line %d: %s\n", lineno, e);
}

void n_ValueError(const char* e, int lineno)
{
    fprintf(stderr, "Value Error on line %d: %s\n", lineno, e);
}