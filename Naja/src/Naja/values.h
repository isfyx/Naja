#pragma once

#define N_SCOPE_SIZE    64

#include <stdio.h>

#include "flex_bison/parser.h"

// Value

typedef struct n_Value n_Value;
struct n_Value {
    int      type;
    char*    name;
    size_t   size;
    void*    value;
    n_Value* next;
};

// Scope

typedef struct n_Scope n_Scope;
struct n_Scope {
    n_Scope*   parent;
    n_Value**  values;
};

extern n_Scope* n_global_scope;

n_Scope* n_new_scope(n_Scope* parent);
void     n_destroy_scope(n_Scope* s);
size_t   n_scope_hash(char* c);
n_Value* n_assign_name(n_Scope* s, n_Value* v, char* n);
n_Value* n_lookup(n_Scope* s, char* n);

// Value constructors

n_Value* n_new_boolean(char v);
n_Value* n_new_float(double v);
n_Value* n_new_int(long v);
n_Value* n_new_null();
n_Value* n_new_string(char* str, size_t len);

n_Value* n_copy_value(n_Value* v);
n_Value* n_destroy_value(n_Value* v);

// Return values

char   n_get_boolean_value(n_Value* v);
double n_get_float_value(n_Value* v);
long   n_get_integer_value(n_Value* v);
char*  n_get_string_value(n_Value* v);