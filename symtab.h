#ifndef SYMTAB_H
#define SYMTAB_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "simp-tree.h"

typedef struct Entry {
    Type    type;
    Value   value;
    int     argnum;
} Entry;

// return value is error code.
int get_from_symtab(char* key, Type* rettype, Value* retval, int* ret_argnum); 
int add_to_symtab(char* key, Type type, Value value); 
//int get_func_from_symtab(char* key, Type* rettype, Value* retval); 
int add_func_to_symtab(char* key, Value bodyptr, int argnum);

#ifdef __cplusplus
}
#endif

#endif //SYMTAB_H
