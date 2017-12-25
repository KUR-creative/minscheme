#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdio.h>

void    unix_error(char* msg);
void*   Malloc(size_t size); 
void*   Calloc(size_t num, size_t size);

FILE*   Fopen(const char* filename, const char* mode);
void    Fclose(FILE* stream);
void    Remove(char* filename);

#endif
