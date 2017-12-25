#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "syscalls.h"

void    unix_error(char* msg)  
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(0);
}

void*   Malloc(size_t size) 
{
    void*   p;
    if ((p = malloc(size)) == NULL)
        unix_error("Malloc error");
    return p;
}

void*   Calloc(size_t num, size_t size) 
{
    void*   p;
    if ((p = calloc(num,size)) == NULL)
        unix_error("Calloc error");
    return p;
}

FILE*   Fopen(const char* filename, const char* mode) 
{
    FILE*   tmpfile = fopen(filename, mode);

    if(tmpfile == NULL) 
        unix_error("Fopen error");

    return tmpfile;
}

void    Fclose(FILE* stream)
{
    if(fclose(stream) != 0)
        unix_error("Fclose error");
}

void    Remove(char* filename)
{
    if(remove(filename) != 0){
        perror("Remove error!");
        exit(EXIT_FAILURE);
    }
}

