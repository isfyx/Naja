#include "predicates.h"

#include "errors.h"

extern int n_lineno;

int n_is_comparable(n_Value* a, n_Value* b)
{
    return n_is_number(a) && n_is_number(b);
}

int n_is_divisible(n_Value* f1, n_Value* f2)
{
    if (!n_is_number(f1)) {
        n_ValueError("Can't divide non-numeric value", n_lineno);
        return 0;
    }
    if (!n_is_number(f2)) {
        n_ValueError("Can't divide by non-numeric value", n_lineno);
        return 0;
    }
    if (n_is_zero(f2)) {
        n_ValueError("Can't divide by zero", n_lineno);
        return 0;
    }

    return 1;
}

int n_is_integer(n_Value* v)
{
    if (n_is_undefined(v)) {
        n_ValueError("Undefined value", n_lineno);
        return 0;
    }

    return v->type == INT;
}

int n_is_number(n_Value* v)
{
    if (n_is_undefined(v)) {
        n_ValueError("Undefined value", n_lineno);
        return 0;
    }

    switch (v->type) {
        case INT:
        case FLOAT:
            return 1;
    }

    return 0;
}

int n_is_undefined(n_Value* v)
{
    return !(v > 0);
}

int n_is_zero(n_Value* v)
{
    if (!n_is_number(v))
        return 0;

    switch (v->type) {
        case INT:
            return n_get_integer_value(v) == 0;
        case FLOAT:
            return n_get_float_value(v) == 0;
    }

    return 0;
}