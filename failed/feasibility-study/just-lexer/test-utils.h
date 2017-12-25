#pragma once
#ifndef _TEST_UTILS_H_
#define _TEST_UTILS_H_

#include <stdio.h>

#define TMPFILE_NAME   "__temporary__" 

void    unix_error(char* msg);
FILE*   Fopen(const char* filename, const char* mode);
void    Fclose(FILE* stream);
void    Remove(char* filename);

FILE*   new_mockfile(char* str);
void    del_mockfile(FILE* mockfile);
FILE*   new_mockfile_n(char* str, char* filename);
void    del_mockfile_n(FILE* mockfile, char* filename);

#endif
