#pragma once

#define N_FILE_BUF_SIZE     16384
#define N_FILENAME          ((!n_filestack) ? "MAIN" : n_filestack->filename)

#include <stdio.h>

typedef struct n_FileStack n_FileStack;
struct n_FileStack
{
    n_FileStack* prev;
    int          buf_state;
    int          lineno;
    char*        filename;
    FILE*        file;
};

extern n_FileStack* n_filestack;

int n_FS_push(char* filename);
int n_FS_pop();