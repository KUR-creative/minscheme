#include "interpreter.h"
#include "errors.h"
#include "symtab.h"
#include <iostream>
#include <cassert>
#include <string.h> 
#include <string>

Node* syntax_tree = NULL;

extern "C"
{
//#include "minscheme.tab.h"
extern FILE *yyout;
}

using std::string;
using std::cout;
using std::endl;

//static char tmpstrbuf[32] = {0,};
/*
static char* ll2str(long long n){
    sprintf(tmpstrbuf, "%lld", n);
    return tmpstrbuf;
}
*/
static 
void error_report(Node* node, ErrorKind kind, Inherit* state, int argnum = 0);
static
bool incorrect_argnum(Node* node, int* ret_argnum);

static
void type_check_sub2(Node* call_expr, Inherit* state,int car_eval_argnum){
    if(*state != ERROR_STATE){ 
        if(cadr(call_expr)->type != INT ||
           caddr(call_expr)->type != INT )// type mismatched!
        { 
            error_report(call_expr, TYPE_MISMATCH, state, car_eval_argnum);
        }
    }
}
static
void eval_sub2(Node* call_expr, Inherit* state){
    if(*state != ERROR_STATE){ 
        auto arg1val = cadr(call_expr)->value; 
        auto arg2val = caddr(call_expr)->value; 
        del_tree(&car(call_expr)); del_tree(&cdr(call_expr));
        call_expr->value = arg1val - arg2val;
        call_expr->type = INT;
    }
}

static
void type_check_add2(Node* call_expr, Inherit* state,int car_eval_argnum){
    if(*state != ERROR_STATE){ 
        if(cadr(call_expr)->type != INT ||
           caddr(call_expr)->type != INT )// type mismatched!
        { 
            error_report(call_expr, TYPE_MISMATCH, state, car_eval_argnum);
        }
    }
}
static
void eval_add2(Node* call_expr, Inherit* state){
    if(*state != ERROR_STATE){ 
        auto arg1val = cadr(call_expr)->value; 
        auto arg2val = caddr(call_expr)->value; 
        del_tree(&car(call_expr)); del_tree(&cdr(call_expr));
        call_expr->value = arg1val + arg2val;
        call_expr->type = INT;
    }
}

static
void type_check_mul2(Node* call_expr, Inherit* state,int car_eval_argnum){
    if(*state != ERROR_STATE){ 
        if(cadr(call_expr)->type != INT ||
           caddr(call_expr)->type != INT )// type mismatched!
        { 
            error_report(call_expr, TYPE_MISMATCH, state, car_eval_argnum);
        }
    }
}

static
void eval_mul2(Node* call_expr, Inherit* state){
    if(*state != ERROR_STATE){ 
        auto arg1val = cadr(call_expr)->value; 
        auto arg2val = caddr(call_expr)->value; 
        del_tree(&car(call_expr)); del_tree(&cdr(call_expr));
        call_expr->value = arg1val * arg2val;
        call_expr->type = INT;
    }
}

static
void type_check_div2(Node* call_expr, Inherit* state,int car_eval_argnum){
    if(*state != ERROR_STATE){ 
        if(cadr(call_expr)->type != INT ||
           caddr(call_expr)->type != INT )// type mismatched!
        { 
            error_report(call_expr, TYPE_MISMATCH, state, car_eval_argnum);
        }
    }
}

static
void eval_div2(Node* call_expr, Inherit* state){
    if(*state != ERROR_STATE){ 
        auto arg1val = cadr(call_expr)->value; 
        auto arg2val = caddr(call_expr)->value; 
        del_tree(&car(call_expr)); del_tree(&cdr(call_expr));
        call_expr->value = arg1val / arg2val;
        call_expr->type = INT;
    }
}


static 
void type_check_disp(Node* call_expr, Inherit* state, int car_eval_argnum){
    if(*state != ERROR_STATE){ 
        if( cadr(call_expr)->type == INT ||
            cadr(call_expr)->type == BOOL_TYPE){ // type matched!
        }else{
            error_report(call_expr, TYPE_MISMATCH, state, car_eval_argnum);
        }
    }
}
static 
void eval_disp(Node* call_expr, Inherit* state){
    if(*state != ERROR_STATE){ 
        if( cadr(call_expr)->type == INT){
            fprintf(yyout, "%lld", cadr(call_expr)->value); 
        }else if( cadr(call_expr)->type == BOOL_TYPE){ 
            if(cadr(call_expr)->value == 0){
                fprintf(yyout, "false");
            }else{
                fprintf(yyout, "true");
            }
        }
    }
}

static
void type_check_newline(Node* call_expr, Inherit* state,int car_eval_argnum){
}
static
void eval_newline(Node* call_expr, Inherit* state){
    if(*state != ERROR_STATE){ 
        fprintf(yyout, "\n"); 
    }
}


inline static
bool streq(char* s1, char* s2){
    return (strcmp(s1, s2) == 0);
}

inline static
bool is_disp_expr(Node* node){
    assert( car(node) != NULL );
    //cout << "isdisp???:" << car(node)->name << '\n';
    return streq(car(node)->name, "disp"); // it will be replaced to predefined function.
    /*
    auto* child0 = node->child[0];
    return (child0 != NULL &&
            child0->name != NULL &&
            streq(child0->name, "disp"));
            */
}

inline static
bool is_define_expr(Node* node){
    return true;
    /*
    auto* child0 = node->child[0];
    return (child0 != NULL &&
            child0->name != NULL &&
            streq(child0->name, "define"));
            */
}

/*
inline static
bool is_add2_call(Node* node){
    assert( car(node) != NULL );
    //cout << "is add2 ???:" << car(node)->name << '\n';
    return streq(car(node)->name, "add2"); // it will be replaced to predefined function.
}

inline static
bool is_sub2_call(Node* node){
    assert( car(node) != NULL );
    //cout << "is add2 ???:" << car(node)->name << '\n';
    return streq(car(node)->name, "sub2"); // it will be replaced to predefined function.
}
*/

static
bool incorrect_argnum(Node* node, int* ret_argnum)
{
    char* func_name = car(node)->name;
    int argnum;
    auto error_flag = get_from_symtab(func_name, 
                                      NULL, NULL, &argnum);
    *ret_argnum = argnum;
    return (list_len(node) != (1 + argnum)); // func + argnum
}

static
void error_report(Node* node, ErrorKind kind, Inherit* state, int argnum)
{
    switch(kind){
    case INCORRECT_ARGNUM:{
        string errmsg = string(car(node)->name) 
                        + string(incorrect_argnum_errmsg);
        fprintf(yyout, "%s", errmsg.c_str());
        *state = ERROR_STATE;
        return;
        }
    case TYPE_MISMATCH:{
        string errmsg = string("(") 
                        + string(car(node)->name)
                        + string(" ")
                        + string(cadr(node)->name)
                        + string(")")
                        + string(type_mismatch_errmsg);
        fprintf(yyout, "%s", errmsg.c_str());
        *state = ERROR_STATE;
        return;
        }
    case UNBOUND_VARIABLE:{
        string errmsg = string(node->name) 
                        + string(unbound_variable_errmsg);
        fprintf(yyout, "%s", errmsg.c_str());
        return;
        }
    case ILL_FORMED_SPECIAL_FORM:{//TODO: generalize!
        string errmsg = string("(define newid 12 43)")
                        +string(ill_formed_special_form_errmsg);
        fprintf(yyout, "%s", errmsg.c_str());
        return;
        }
    }
}

//inline static
//bool 
//TODO: remove NOT_APPLICABLE error.
//      LL LR RL.. 왜 에러 나는가?
//      분석해서, int가 eval되지 않게 하라.

// move car
// set state
// move cdr 
// --- if no more move ---
// do proper job on this node(or tree): evaluation...
// return hint
Type interpret(Node* node, Inherit state, PrevEdges prevXX) {
    Type car_type = NULL_NODE;
    if( car(node) != NULL ){
        if(state == DEFINING){
            car_type = interpret(car(node), DEFINING, push_car(prevXX));
        }else{
            car_type = interpret(car(node), NORMAL_NODE, push_car(prevXX));
        }
    }

    int car_eval_argnum;
    if( car_type == FUNC ){
        state = FUNC_CALL; 
        if( incorrect_argnum(node, &car_eval_argnum) ) { 
            error_report(node, INCORRECT_ARGNUM, &state);
        }
    }
    else if( car_type == DEFINE_TYPE ){ 
        state = DEFINING;
        if( incorrect_argnum(node, &car_eval_argnum) ) { 
            error_report(node, ILL_FORMED_SPECIAL_FORM, &state);
        }
    }

    Type cdr_type = NULL_NODE;
    if( cdr(node) != NULL ){
        // TODO: refactor?
        Inherit next_state;
        if(state == TOP_LIST || state == DEFINING){
            next_state = state;
        }
        else{
            if( car_type == FUNC ){ 
                next_state = FUNC_CALL;
            }else{
                next_state = NORMAL_NODE;
            }
        }
        interpret(cdr(node), next_state, push_cdr(prevXX));
    }

    // type은 evaluation이 끝나고 나온다...
    // 이 시점에서 함수 호출의 길이는 이미 체크되어 있음.
    if( car_type == FUNC ){ 
        char* func_name = car(node)->name;
        if(streq(func_name,"disp")){
            type_check_disp(node, &state, car_eval_argnum);
            eval_disp(node, &state);
        }
        else if(streq(func_name,"add2")){
            type_check_add2(node, &state, car_eval_argnum);
            eval_add2(node, &state);
        }
        else if(streq(func_name,"sub2")){
            type_check_sub2(node, &state, car_eval_argnum);
            eval_sub2(node, &state);
        }
        else if(streq(func_name,"mul2")){
            type_check_mul2(node, &state, car_eval_argnum);
            eval_mul2(node, &state);
        }
        else if(streq(func_name,"div2")){
            type_check_div2(node, &state, car_eval_argnum);
            eval_div2(node, &state);
        }
        else if(streq(func_name,"newline")){
            //type_check_newline(node, &state, car_eval_argnum);
            eval_newline(node, &state);
        }
    }


    if( node->hint == ID_ATOM ){
        if( state != DEFINING ){ // if defining, then skip!
            Value value;
            Type type;
            auto error_flag = get_from_symtab(node->name, 
                                              &type, &value, NULL);

            if(error_flag == UNBOUND_VARIABLE) {
                error_report(node, UNBOUND_VARIABLE, &state);
            }else{
                node->type = type; 
                node->value = value; 
            }
        }
    }

    //cout << '[' << node->name << '|' << state << '|' << prevXX << "] \n"; 

    return node->type;
    /*
    bool is_func_position = false;
    Hint car_hint;
    if( state == DEFINING ){
        car_hint = safe_interpret(car(node), DEFINING);
    }else{
        car_hint = safe_interpret(car(node), CAR_DOWN);
    }

    if( car_hint == DEFINE_ATOM ){ 
        state = DEFINING;
    }
    else if( state == CAR_DOWN )   // This node is FIRST node of list.
    {
        is_func_position = true;
        if( car_hint == INT_ATOM ){// INT_ATOM is not applicable. 
            string errmsg = string(car(node)->name) + string(not_applicable_errmsg);
            fprintf(yyout, "%s", errmsg.c_str());
        }
    }

    if( state != DEFINING ){
        state = CDR_DOWN;
    }
    Hint cdr_hint = safe_interpret(cdr(node), state);

    //cout << node->hint << ':' << node->name << '[' 
         //<< car_hint << ',' << cdr_hint << "]\n";
    if( node->hint == ID_ATOM ){
        if( state != DEFINING ){ // if defining, then skip!
            Value value;
            Type type;
            int argnum;
            auto error_flag = get_from_symtab(node->name, 
                                              &type, &value, &argnum);

            if(error_flag == UNBOUND_VARIABLE) {
                string errmsg = string(node->name) 
                                + string(unbound_variable_errmsg);
                fprintf(yyout, "%s", errmsg.c_str());
            }else{
                node->value = value; //get value from symtab!
            }
        }
    }
    else if( car_hint == ID_ATOM ){ // 이거 함수 인자 개수 설정 되겠는데?
        if( is_add2_call(node) ){
            if( list_len(node) != (1 + 2) ){ // 3: add2, arg1, arg2
                string errmsg = string(car(node)->name) 
                                + string(incorrect_argnum_errmsg);
                fprintf(yyout, "%s", errmsg.c_str());
            }else{
                if(is_func_position){
                    assert(cadr(node) != NULL); // if cadr is null, then..??
                    assert(caddr(node) != NULL);
                    auto arg1val = cadr(node)->value; 
                    auto arg2val = caddr(node)->value; 
                    // evaluation!: int x int -> int
                    del_tree(&car(node)); del_tree(&cdr(node));
                    node->value = arg1val + arg2val;
                    node->hint = INT_ATOM;
                }
            }
        }
        else if( is_sub2_call(node) ){ // check name
            if( list_len(node) != (1 + 2) ){ // 3: add2, arg1, arg2
                string errmsg = string(car(node)->name) 
                                + string(incorrect_argnum_errmsg);
                fprintf(yyout, "%s", errmsg.c_str());
            }else{
                if(is_func_position){
                    assert(cadr(node) != NULL);
                    assert(caddr(node) != NULL);
                    auto arg1val = cadr(node)->value; 
                    auto arg2val = caddr(node)->value; 
                    // evaluation!: int x int -> int
                    del_tree(&car(node)); del_tree(&cdr(node));
                    node->value = arg1val - arg2val;
                    node->hint = INT_ATOM;
                }
            }
        }
        else if(is_disp_expr(node)){
            //cout << '<' << cadr(node)->value << '>';
            if(is_func_position){
                fprintf(yyout, "%lld", cadr(node)->value); 
            }
        }
    }
    else if( car_hint == DEFINE_ATOM && cdr_hint == EXPR_PAIR ){ // list len 뽑는 함수 만들 것.
        if( is_define_expr(node) ) {
            auto    name    = cadr(node)->name;
            Value   value   = caddr(node)->value;
            Type    type    = caddr(node)->type; 
            add_to_symtab(name, type, value);
        }
    }

    */
}

        /*
        if( car_hint != ID_ATOM ){// Only ID_ATOM could be a function.
            if( car_hint != NO_CHILD ){
                string errmsg = string(car(node)->name) + string(not_applicable_errmsg);
                fprintf(yyout, "%s", errmsg.c_str());
            }
        }
        */
