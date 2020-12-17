#pragma once

#include <stdlib.h>

#include "flex_bison/parser.h"

typedef struct n_value {
    int    type;
    size_t size;
    void*  value;
} n_value;

n_value n_new_int();