#pragma once

void n_NameError(const char* e, const char* n, int lineno);
void n_RuntimeError(const char* e, int lineno);
void n_TypeError(const char* e, int lineno);
void n_ValueError(const char* e, int lineno);