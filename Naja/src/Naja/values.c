#include "values.h"

#include <stdlib.h>
#include <string.h>

#include "errors.h"
#include "predicates.h"

extern int n_lineno;

// Scope

n_Scope* n_new_scope(n_Scope* parent)
{
    n_Scope* s = malloc(sizeof(n_Scope));

    if (!s) {
        perror("malloc");
        exit(1);
    }

    s->values = calloc(N_SCOPE_SIZE, sizeof(n_Value*));
    
    if (!s->values) {
        perror("malloc");
        exit(1);
    }

    s->parent       = parent;

    return s;
}

void n_destroy_scope(n_Scope* s)
{
    for (size_t i = 0; i < N_SCOPE_SIZE; i++) {
        do {
            s->values[i] = n_destroy_value(s->values[i]);
        } while (s->values[i]); 
    }

    free(s->values);
    free(s);
}

size_t n_scope_hash(char* c)
{
    // TEMPORARY SOLUTION

    if (*c == '_')
        return 0;
    if ('a' <= *c && *c <= 'z')
        return *c - 'a' + 1;
    if ('A' <= *c && *c <= 'Z')
        return *c - 'A' + 27;
    
    n_RuntimeError("Can't hash name", n_lineno);
    exit(1);
}

n_Value* n_assign_name(n_Scope* s, n_Value* v, char* n)
{
    size_t   idx  = n_scope_hash(n);
    n_Value* prev = s->values[idx];

    while (prev) {
        if (!strcmp(prev->name, n)) {
            n_RuntimeError("Name already in use", n_lineno);
            return n_new_boolean(0);
        }
        prev = prev->next;
    }

    n_Value* value = n_copy_value(v);
    char*    name  = calloc(strlen(n)+1, sizeof(char));

    strcpy(name, n);
    value->name = name;
    value->next = s->values[idx];
    s->values[idx] = value;
    return n_copy_value(v);
}

n_Value* n_lookup(n_Scope* s, char* n)
{
    n_Value* v = s->values[n_scope_hash(n)];

    while (v && strcmp(v->name, n)) v = v->next;

    if (!v) {
        if (s->parent) return n_lookup(s->parent, n);
        n_NameError("Name '%s' is not defined", n, n_lineno);
        exit (1);
    }

    return v;
}

// Value constructors

n_Value* n_new_boolean(char v)
{
    n_Value* value = calloc(1, sizeof(n_Value));
    
    value->type  = BOOL;
    value->size  = sizeof(char);
    value->value = malloc(sizeof(char));

    memcpy(value->value, &v, sizeof(char)); 

    return value;
}

n_Value* n_new_float(double v)
{
    n_Value* value = calloc(1, sizeof(n_Value));
    
    value->type  = FLOAT;
    value->size  = sizeof(double);
    value->value = malloc(sizeof(double));

    memcpy(value->value, &v, sizeof(double));

    return value;
}

n_Value* n_new_int(long v)
{
    n_Value* value = calloc(1, sizeof(n_Value));
    
    value->type  = INT;
    value->size  = sizeof(long);
    value->value = malloc(sizeof(long));

    memcpy(value->value, &v, sizeof(long)); 

    return value;
}

n_Value* n_new_null()
{
    n_Value* value = calloc(1, sizeof(n_Value));
    
    value->type  = N_NULL;

    return value;
}

n_Value* n_new_string(char* str, size_t len)
{
    n_Value* value = calloc(1, sizeof(n_Value));

    value->type  = STRING;
    value->size  = len ? len : strlen(str)+1;
    value->value = len ? str : malloc(value->size);

    if (!len) strcpy(value->value, str);

    ((char*)value->value)[value->size-1] = 0;

    return value;
}

n_Value* n_copy_value(n_Value* v)
{
    n_Value* value = malloc(sizeof(n_Value));

    value->type  = v->type;
    value->name  = 0;
    value->size  = v->size;
    value->value = malloc(v->size);
    value->next  = 0;

    memcpy(value->value, v->value, v->size);

    return value;
}

n_Value* n_destroy_value(n_Value* v)
{
    if (!v) return 0;
    n_Value* next = v->next;
    free(v->name);
    free(v->value);
    free(v);
    return next;
}

// Return values

char n_get_boolean_value(n_Value* v)
{
    if (n_is_undefined(v)) {
        n_ValueError("Can't get boolean value of an undefined value", n_lineno);
        exit(1);
    }

    switch (v->type) {
        case BOOL:
            return *((char*)v->value);
        case INT:
            return *((long*)v->value) != 0;
        case FLOAT:
            return *((double*)v->value) != 0;
        case N_NULL:
            return 0;
    }

    n_TypeError("Boolean value is not defined for this type", n_lineno);
    exit(1);
}

double n_get_float_value(n_Value* v)
{
    if (!n_is_number(v)) {
        n_ValueError("Can't get floating point value of non-numeric value", n_lineno);
        exit (1);
    }

    double value;

    switch (v->type) {
        case INT:
            value = (double)*((long*)v->value);
            break;
        case FLOAT:
            value = *((double*)v->value);
            break;
    }

    return value;
}

long n_get_integer_value(n_Value* v)
{
    if (!n_is_number(v)) {
        n_ValueError("Can't get integer value of non-numeric value", n_lineno);
        exit (1);
    }

    long value;

    switch (v->type) {
        case INT:
            value = *((long*)v->value);
            break;
        case FLOAT:
            value = (long)*((double*)v->value);
            break;
    }

    return value;
}

char* n_get_string_value(n_Value* v)
{
    if (n_is_undefined(v)) {
        n_ValueError("Can't get string value of undefined value", n_lineno);
        exit (1);
    }

    char* s = 0;

    switch (v->type) {
        case STRING:
            s = calloc(v->size, sizeof(char));
            strcpy(s, v->value);
            break;
        case INT:
            s = calloc(16, sizeof(char));
            sprintf(s, "%ld", n_get_integer_value(v));
            break;
        case FLOAT:
            s = calloc(32, sizeof(char));
            sprintf(s, "%f", n_get_float_value(v));
            break;
        case BOOL:
            s = malloc(6);
            strcpy(s, n_get_boolean_value(v) ? "True" : "False");
            break;
        case N_NULL:
            s = malloc(5);
            strcpy(s, "Null");
            break;
    }

    if (!s) {
        n_TypeError("String value not implemented for type", n_lineno);
        exit (1);
    }

    return s;
}
