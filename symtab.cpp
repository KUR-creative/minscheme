#include "errors.h"
#include "symtab.h"

#include <unordered_map>
#include <string>
#include <stdexcept>
#include <cassert>
using std::unordered_map;
using std::string;

extern "C"
{
extern FILE *yyout;
}

#include <string.h> 
inline static
bool streq(char* s1, char* s2){
    return (strcmp(s1, s2) == 0);
}
    //{ "define", {DEFINE_TYPE,   1ll, 2} }, // special form: 1ll
    //{ "if",     {IF_TYPE,       1ll, 3} }, // special form: 1ll
unordered_map<string,Entry> symtab;  // global env.

Value newline(void)
{
    //printf("\n");
    fprintf(yyout, "\n");
    return UNKNOWN_VAL;
}

// primitives.
Value add2(Value a, Value b)
{
    return a+b;
}

Value display(Node* node)
{
    auto type = node->type;
    switch(type){
    case INT:   
        //printf("%lld", node->value); 
        fprintf(yyout, "%lld", node->value);
        break;

    case FLOAT: 
        //printf("%f", Value_to_double(node->value));
        fprintf(yyout, "%f",  Value_to_double(node->value));
        break;

    case BOOL:  
        if(node->value){
            //printf("true"); 
            fprintf(yyout, "true");
        }else{
            //printf("false"); 
            fprintf(yyout, "false");
        }
        break;
    }
    return UNKNOWN_VAL;
}


void* get_body(Value value)
{
    assert(value != NULL);

    Node* end_pair = cdr(value);
    assert(end_pair != NULL);

    if(car(end_pair) == NULL){ // means primitive.
        return (void*)( end_pair->value ); // primitive function pointer.
    }
    else{ // means user-defined.
        return (void*)( car(end_pair) ); // defined function body.
    }
}

Node* get_arglist(Value value)
{
    assert(car(value) != NULL);
    return car(value);
}

void init_symtab(void)
{
    Node* nl_args   = atom("nil", PAIR, 0ll);
    Node* nl_node   = pair("pair", PAIR, 0ll,
                           nl_args,
                           atom("func-ptr", NO_TYPE, (Value)newline));

    Node* disp_args = pair("arg-list",PAIR, 0ll,
                           atom("arg1", GENERIC, 0ll),
                           NULL);
    Node* disp_node = pair("pair", PAIR, 0ll,
                           disp_args, 
                           atom("func-ptr", NO_TYPE, (Value)display));

    Node* add2args  = pair("arg-list", PAIR, 0ll, 
                           atom("arg1", INT, 0ll),
                           pair("pair", PAIR, 0ll,
                                atom("arg2", INT, 0ll),
                                NULL));
    Node* add2node  = pair("pair", PAIR, 0ll,
                           add2args,
                           atom("func-ptr", NO_TYPE, (Value)add2));


    //pretty_print(add2node,0);
    //pretty_print(disp_node,0);
    symtab = 
    {
        { "newline",{FUNC, (Value)nl_node} },
        { "display",{FUNC, (Value)disp_node} },  // primitive func has C function ptr.
        { "add2",   {FUNC, (Value)add2node} }, 
        { "sub2",   {FUNC, 0ll} },
        { "mul2",   {FUNC, 0ll} },
        { "div2",   {FUNC, 0ll} },
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
