#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "test-utils.h"
#include "syscalls.h"

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
