#pragma once

#include "values.h"

int n_is_comparable(n_Value* a, n_Value* b);
int n_is_divisible(n_Value* f1, n_Value* f2);
int n_is_integer(n_Value* v);
int n_is_number(n_Value* v);
int n_is_undefined(n_Value* v);
int n_is_zero(n_Value* v);