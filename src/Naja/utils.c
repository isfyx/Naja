#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errors.h"
#include "predicates.h"

extern int yy_create_buffer();
extern int yy_switch_to_buffer();
extern int yy_delete_buffer();

extern int n_lineno;

extern n_Scope* n_current_scope;

void n_trace(int lineno, n_Value* value)
{
    char *s = n_get_string_value(value);
    printf("[%04d] -> %s\n", lineno, s);
    free(s);
}

// Logic

n_Value* n_disjunction(n_Value* a, n_Value* b)
{
    if (n_is_undefined(a) || n_is_undefined(b)) {
        n_ValueError("Can't resolve disjunction (or) of undefined value", n_lineno);
        return 0;
    }

    return n_copy_value(n_get_boolean_value(a) ? a : b);
}

n_Value* n_conjunction(n_Value* a, n_Value* b)
{
    if (n_is_undefined(a) || n_is_undefined(b)) {
        n_ValueError("Can't resolve conjunction (and) of undefined value", n_lineno);
        return 0;
    }

    return n_copy_value(n_get_boolean_value(a) ? b : a);
}

n_Value* n_negate(n_Value* v)
{
    if (n_is_undefined(v)) {
        n_ValueError("Can't negate (not) undefined value", n_lineno);
        return 0;
    }

    return n_new_boolean(!n_get_boolean_value(v));
}

// Comparrators

n_Value* n_compare_eq(n_Value* a, n_Value* b)
{
    if (!n_is_comparable(a, b)) {
        n_ValueError("Can't compare values", n_lineno);
        return 0;
    }

    if (n_is_integer(a) && n_is_integer(b))
        return n_new_boolean(n_get_integer_value(a) == n_get_integer_value(b));
    else if (n_is_number(a) && n_is_number(b))
        return n_new_boolean(n_get_float_value(a) == n_get_float_value(b));

    n_RuntimeError("Comparrison (==) failed", n_lineno);
    return 0;
}

n_Value* n_compare_geq(n_Value* a, n_Value* b)
{
    if (!n_is_comparable(a, b)) {
        n_ValueError("Can't compare values", n_lineno);
        return 0;
    }

    if (n_is_integer(a) && n_is_integer(b))
        return n_new_boolean(n_get_integer_value(a) >= n_get_integer_value(b));
    else if (n_is_number(a) && n_is_number(b))
        return n_new_boolean(n_get_float_value(a) >= n_get_float_value(b));

    n_RuntimeError("Comparrison (>=) failed", n_lineno);
    return 0;
}

n_Value* n_compare_leq(n_Value* a, n_Value* b)
{
    if (!n_is_comparable(a, b)) {
        n_TypeError("Can't compare values", n_lineno);
        return 0;
    }

    if (n_is_integer(a) && n_is_integer(b))
        return n_new_boolean(n_get_integer_value(a) <= n_get_integer_value(b));
    else if (n_is_number(a) && n_is_number(b))
        return n_new_boolean(n_get_float_value(a) <= n_get_float_value(b));

    n_RuntimeError("Comparrison (<=) failed", n_lineno);
    return 0;
}

n_Value* n_compare_neq(n_Value* a, n_Value* b)
{
    if (!n_is_comparable(a, b)) {
        n_TypeError("Can't compare values", n_lineno);
        return 0;
    }

    if (n_is_integer(a) && n_is_integer(b))
        return n_new_boolean(n_get_integer_value(a) != n_get_integer_value(b));
    else if (n_is_number(a) && n_is_number(b))
        return n_new_boolean(n_get_float_value(a) != n_get_float_value(b));

    n_RuntimeError("Comparrison (!=) failed", n_lineno);
    return 0;
}

n_Value* n_compare_gt(n_Value* a, n_Value* b)
{
    if (!n_is_comparable(a, b)) {
        n_TypeError("Can't compare values", n_lineno);
        return 0;
    }

    if (n_is_integer(a) && n_is_integer(b))
        return n_new_boolean(n_get_integer_value(a) > n_get_integer_value(b));
    else if (n_is_number(a) && n_is_number(b))
        return n_new_boolean(n_get_float_value(a) > n_get_float_value(b));

    n_RuntimeError("Comparrison (>) failed", n_lineno);
    return 0;
}

n_Value* n_compare_lt(n_Value* a, n_Value* b)
{
    if (!n_is_comparable(a, b)) {
        n_TypeError("Can't compare values", n_lineno);
        return 0;
    }

    if (n_is_integer(a) && n_is_integer(b))
        return n_new_boolean(n_get_integer_value(a) < n_get_integer_value(b));
    else if (n_is_number(a) && n_is_number(b))
        return n_new_boolean(n_get_float_value(a) < n_get_float_value(b));

    n_RuntimeError("Comparrison (<) failed", n_lineno);
    return 0;
}

// Math

n_Value* n_add(n_Value* a, n_Value* b)
{
    if (!n_is_number(a) || !n_is_number(b)) {
        n_ValueError("Can't preform addition on non-numeric value", n_lineno);
        return 0;
    }

    if (n_is_integer(a) && n_is_integer(b))
        return n_new_int(n_get_integer_value(a) + n_get_integer_value(b));

    return n_new_float(n_get_float_value(a) + n_get_float_value(b));
}

n_Value* n_sub(n_Value* a, n_Value* b)
{
    if (!n_is_number(a)) {
        n_ValueError("Can't subtract from non-numeric value", n_lineno);
        return 0;
    }
    if (!n_is_number(b)) {
        n_ValueError("Can't subtract non-numeric value", n_lineno);
        return 0;
    }

    if (n_is_integer(a) && n_is_integer(b))
        return n_new_int(n_get_integer_value(a) - n_get_integer_value(b));

    return n_new_float(n_get_float_value(a) - n_get_float_value(b));
}

n_Value* n_mul(n_Value* f1, n_Value* f2)
{
    if(!n_is_number(f1) || !n_is_number(f2)) {
        n_ValueError("Can't multiply non-numeric value", n_lineno);
        return 0;
    }

    if (n_is_integer(f1) && n_is_integer(f2))
        return n_new_int(n_get_integer_value(f1) * n_get_integer_value(f2));

    return n_new_float(n_get_float_value(f1) * n_get_float_value(f2));
}

n_Value* n_divide(n_Value* f1, n_Value* f2)
{
    if (!n_is_divisible(f1, f2))
        return 0;

    if (n_is_integer(f1) && n_is_integer(f2)) {
        long ff1 = n_get_integer_value(f1);
        long ff2 = n_get_integer_value(f2);
        if (ff1 % ff2 == 0)
            return n_new_int(ff1/ff2);
    }

    double ff1 = n_get_float_value(f1);
    double ff2 = n_get_float_value(f2);

    return n_new_float(ff1/ff2);
}

n_Value* n_int_divide(n_Value* f1, n_Value* f2)
{
    if (!n_is_divisible(f1, f2))
        return 0;

    return n_new_int(n_get_integer_value(f1) / n_get_integer_value(f2));
}

n_Value* n_mod(n_Value* f1, n_Value* f2)
{
    if (!n_is_divisible(f1, f2))
        return 0;

    if (n_is_integer(f1) && n_is_integer(f2)) {
        long ff1 = n_get_integer_value(f1);
        long ff2 = n_get_integer_value(f2);
        
        return n_new_int(ff1%ff2);
    }

    double ff1 = n_get_float_value(f1);
    double ff2 = n_get_float_value(f2);
    double s   = ff1 < 0 ? -1 : 1;

    ff1 = ff1 < 0 ? -ff1 : ff1;
    ff2 = ff2 < 0 ? -ff2 : ff2;

    while (ff1>ff2)
        ff1 -= ff2;

    return n_new_float(s*ff1);
}

n_Value* n_unary_plus(n_Value* v)
{
    if (!n_is_number(v)) {
        n_ValueError("Can't apply unary '+' to non-numeric value", n_lineno);
        return 0;
    }

    switch (v->type) {
        case INT:
            return n_new_int(+n_get_integer_value(v));
        case FLOAT:
            return n_new_float(+n_get_float_value(v));
    }

    return 0;
}

n_Value* n_unary_minus(n_Value* v)
{
    if (!n_is_number(v)) {
        n_ValueError("Can't apply unary '-' to non-numeric value", n_lineno);
        return 0;
    }

    switch (v->type) {
        case INT:
            return n_new_int(-n_get_integer_value(v));
        case FLOAT:
            return n_new_float(-n_get_float_value(v));
    }

    return 0;
}

n_Value* n_exp(n_Value* base, n_Value* exp)
{
    if (!n_is_number(base)) {
        n_ValueError("Base must be a number", n_lineno);
        return 0;
    }

    if (!n_is_integer(exp)) {
        n_ValueError("Exponent must be an integer value", n_lineno);
        return 0;
    }

    long e = n_get_integer_value(exp);

    if (e < 0)
        n_ValueError("Exponent can't be less than zero", n_lineno);

    double b = n_get_float_value(base);
    double v = 1;

    for (long i = 0; i < e; i++) v *= b;

    if (n_is_integer(base))
        return n_new_int(v);

    return n_new_float(v);
}
