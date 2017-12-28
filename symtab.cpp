#include "errors.h"
#include "symtab.h"

#include <unordered_map>
#include <string>
#include <stdexcept>
#include <cassert>
using std::unordered_map;
using std::string;

    //{ "define", {DEFINE_TYPE,   1ll, 2} }, // special form: 1ll
    //{ "if",     {IF_TYPE,       1ll, 3} }, // special form: 1ll
unordered_map<string,Entry> symtab;  // global env.

// primitives.
Value add2(Value a, Value b)
{
    return a+b;
}


void* get_body(Value value)
{
    assert(cdr(value) != NULL);
    return (void*)( cdr(value)->value );
}

Node* get_arglist(Value value)
{
    assert(car(value) != NULL);
    return car(value);
}

void init_symtab(void)
{
    auto* arglist = pair("arg-list", PAIR, 0ll, 
                         atom("arg1", INT, 0ll),
                         pair("pair", PAIR, 0ll,
                              atom("arg2", INT, 0ll),
                              NULL));
    auto* node = pair("pair", PAIR, 0ll,
                      arglist, atom("func-ptr", NO_TYPE, (Value)add2));
    pretty_print(node,0);
    symtab = 
    {
        { "display",{FUNC, 0ll} },  // primitive func has C function ptr.
        { "add2",   {FUNC, (Value)node} }, 
        { "sub2",   {FUNC, 0ll} },
        { "mul2",   {FUNC, 0ll} },
        { "div2",   {FUNC, 0ll} },
        { "newline",{FUNC, 0ll} },
        { "=",      {FUNC, 0ll} }, 
    };
}

void deinit_symtab(void)
{
    symtab.clear();
}


// need to clear symtab..
int get_from_symtab(char* identifier, Type* rettype, Value* retval)
{
    assert(symtab.empty() == false);
    try{
        auto entry = symtab.at(string(identifier));
        if(rettype != NULL)     *rettype    = entry.type;
        if(retval != NULL)      *retval     = entry.value;
        return NO_ERR;
    }
    catch(const std::out_of_range& e){
        return UNBOUND_VARIABLE;
    }
}    

// need to entry structure...
int add_to_symtab(char* identifier, Type type, Value value)
{
    assert(symtab.empty() == false);
    //assert(type != FUNC && "use add_func_to_symtab instead."); //
    symtab[string(identifier)] = {type,value};
}
