#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

extern int yy_create_buffer();
extern int yy_switch_to_buffer();
extern int yy_delete_buffer();

extern int          yylineno;

n_FileStack* n_filestack = 0;

int n_FS_push(char* filename)
{
    FILE*        new_file  = fopen(filename, "r");
    n_FileStack* new_stack = malloc(sizeof(n_FileStack));

    if (!new_file) {
        perror(filename);
        return 0;
    }
    if (!new_stack) {
        perror("malloc");
        exit(1);
    }

    if (n_filestack)
        n_filestack->lineno = yylineno;

    new_stack->prev      = n_filestack;
    new_stack->buf_state = yy_create_buffer(new_file, N_FILE_BUF_SIZE);
    new_stack->file      = new_file;
    new_stack->filename  = filename;

    yy_switch_to_buffer(new_stack->buf_state);
    yylineno = 0;
    n_filestack = new_stack;
    
    return 1;
}

int n_FS_pop()
{
    if (!n_filestack)
        return 0;

    fclose(n_filestack->file);
    yy_delete_buffer(n_filestack->buf_state);

    n_FileStack* prev = n_filestack->prev;
    free(n_filestack);

    if (!prev)
        return 0;

    yy_switch_to_buffer(prev->buf_state);
    yylineno = prev->lineno;
    n_filestack = prev;

    return 1;
}