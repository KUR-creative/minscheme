#include "test-utils.h"
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void    unix_error(char* msg) 
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(0);
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

FILE*   new_mockfile(char* str)
{
    char*   tmpname = TMPFILE_NAME;
    FILE*   tmpfile = Fopen(tmpname, "w");
    fprintf(tmpfile, "%s", str);
    Fclose(tmpfile);
    return Fopen(tmpname, "r");
}

void    del_mockfile(FILE* mockfile)
{
    Fclose(mockfile);
    Remove(TMPFILE_NAME);
}

FILE*   new_mockfile_n(char* str, char* filename)
{
    char*   tmpname = filename;
    FILE*   tmpfile = Fopen(tmpname, "w");
    fprintf(tmpfile, "%s", str);
    Fclose(tmpfile);
    return Fopen(tmpname, "r");
}

void    del_mockfile_n(FILE* mockfile, char* filename)
{
    Fclose(mockfile);
    Remove(filename);
}
