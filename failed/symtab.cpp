#include "errors.h"
#include "symtab.h"

#include <unordered_map>
#include <string>
#include <stdexcept>
#include <cassert>
using std::unordered_map;
using std::string;

unordered_map<string,Entry> symtab = 
{
    { "define",{DEFINE_TYPE, 1ll, 2} }, // special form: 1ll
    { "disp", {FUNC, 0ll, 1} },  // primitive func has NULL body ptr.
    { "add2", {FUNC, 0ll, 2} }, 
    { "sub2", {FUNC, 0ll, 2} },
    { "mul2", {FUNC, 0ll, 2} }, 
    { "div2", {FUNC, 0ll, 2} }, 
    { "newline",{FUNC, 0ll, 0} },
};

// need to clear symtab..
int get_from_symtab(char* identifier, 
                    Type* rettype, Value* retval, int* ret_argnum)
{
    try{
        auto entry = symtab.at(string(identifier));
        if(rettype != NULL)     *rettype = entry.type;
        if(retval != NULL)      *retval = entry.value;
        if(ret_argnum != NULL)  *ret_argnum = entry.argnum;
        return NO_ERR;
    }
    catch(const std::out_of_range& e){
        return UNBOUND_VARIABLE;
    }
}    

// need to entry structure...
int add_to_symtab(char* identifier, Type type, Value value)
{
    assert(type != FUNC && "use add_func_to_symtab instead."); //
    symtab[string(identifier)] = {type,value};
}
