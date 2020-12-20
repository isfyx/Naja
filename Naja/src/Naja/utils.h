#pragma once

#include <stdio.h>

#include "values.h"

void n_trace(int lineno, n_Value* value);

// Logic
n_Value* n_disjunction(n_Value* a, n_Value* b);
n_Value* n_conjunction(n_Value* a, n_Value* b);
n_Value* n_negate(n_Value* v);

// Comparators
n_Value* n_compare_eq(n_Value* a, n_Value* b);
n_Value* n_compare_geq(n_Value* a, n_Value* b);
n_Value* n_compare_leq(n_Value* a, n_Value* b);
n_Value* n_compare_neq(n_Value* a, n_Value* b);
n_Value* n_compare_gt(n_Value* a, n_Value* b);
n_Value* n_compare_lt(n_Value* a, n_Value* b);

// Math
n_Value* n_add(n_Value* a, n_Value* b);
n_Value* n_sub(n_Value* a, n_Value* b);
n_Value* n_mul(n_Value* f1, n_Value* f2);
n_Value* n_divide(n_Value* f1, n_Value* f2);
n_Value* n_int_divide(n_Value* f1, n_Value* f2);
n_Value* n_mod(n_Value* f1, n_Value* f2);
n_Value* n_unary_plus(n_Value* v);
n_Value* n_unary_minus(n_Value* v);
n_Value* n_exp(n_Value* base, n_Value* exp);

