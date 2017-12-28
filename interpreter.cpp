#include "interpreter.h"
#include "errors.h"
#include "symtab.h"
#include <iostream>
#include <cassert>
#include <string.h> 
#include <string>
#include <stack>

Node* syntax_tree = NULL;

extern "C"
{
extern FILE *yyout;
}

using std::stack;
using std::string;
using std::cout;
using std::endl;


Type interpret(Node* parse_tree, State state)
{
}

//static char tmpstrbuf[32] = {0,};
/*
static char* ll2str(long long n){
    sprintf(tmpstrbuf, "%lld", n);
    return tmpstrbuf;
}
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

static
void type_check_equal(Node* call_expr, Inherit* state,int car_eval_argnum){
    if(*state != ERROR_STATE){ 
        if(cadr(call_expr)->type != INT ||
           caddr(call_expr)->type != INT )// type mismatched!
        { 
            error_report(call_expr, TYPE_MISMATCH, state, car_eval_argnum);
        }
    }
}

static
void eval_equal(Node* call_expr, Inherit* state){
    if(*state != ERROR_STATE){ 
        auto arg1val = cadr(call_expr)->value; 
        auto arg2val = caddr(call_expr)->value; 
        del_tree(&car(call_expr)); del_tree(&cdr(call_expr));
        call_expr->value = (arg1val == arg2val);
        call_expr->type = BOOL_TYPE;
    }
}

// car_eval_argnum is useless...!
// warning: type_check_if must called in eval:
static
void type_check_if(Node* expr, Inherit* state,int car_eval_argnum){
    if(*state != ERROR_STATE){ 
        if(cadr(expr)->type != BOOL_TYPE )// type mismatched!
        { 
            error_report(expr, TYPE_MISMATCH, state, car_eval_argnum);
        }
    }
}
static
void eval_if(Node* expr, Inherit* state){
    auto* _cond = cadr(expr); 
    auto* _then = caddr(expr); 
    auto* _else = cadddr(expr); 

    interpret(_cond, SELECTION, LR);
    type_check_if(expr, state, 1); // cond.type = bool ?

    if(*state != ERROR_STATE){ 
        if(_cond->value == 1ll){
            interpret(_then,SELECTION,RL);
            expr->value = _then->value;
            expr->type = _then->type;
        }else{
            interpret(_else,SELECTION,RL);
            expr->value = _else->value;
            expr->type = _else->type;
        }
    }
    del_tree(&car(expr)); del_tree(&cdr(expr));
    // TODO: func도 스테이트로 abort하지 말고 if처럼 del_tree로 abort한다.
}

static 
void eval_define(Node* expr, Inherit* state){ // li0 = define
    // case 1: (define id value) 
    if( cadr(expr)->hint == ID_ATOM     &&
        caddr(expr)->hint != EXPR_PAIR  &&
        caddr(expr)->hint != END_PAIR)
    {
        auto type   = caddr(expr)->type;
        auto value  = caddr(expr)->value;
        auto argnum = caddr(expr)->argnum;
        add_to_symtab(cadr(expr)->name, type, value, argnum);
    }
    else if(cadr(expr)->hint == EXPR_PAIR){ // case 2: function.
        Node* prototype = cadr(expr);
        char* func_name = car(prototype)->name;
        Node* body_ptr = caddr(expr);
        auto  argnum = list_len(prototype) - 1;// 2?
        add_to_symtab(func_name, FUNC, (long long)body_ptr, argnum);
        //cout << '[' << car(body_ptr)->name <<']' <<  endl;
        //cout << argnum <<  endl;
    }
    else{
        // type mismatched.. ill_formed_special_form...
    }
}

inline static
bool streq(char* s1, char* s2){
    return (strcmp(s1, s2) == 0);
}

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
        //string errmsg = string("(") 
                        //+ string(car(node)->name)
                        //+ string(" ")
                        //+ string(cadr(node)->name)
                        //+ string(")")
                        //+ string(type_mismatch_errmsg);
        string errmsg = string(car(node)->name) 
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
        string errmsg = string(car(node)->name) 
                        + string(ill_formed_special_form_errmsg);
        fprintf(yyout, "%s", errmsg.c_str());
        return;
        }
    }
}

void    set_actual_param(Node* node, char* formal_name, Value actual_value)
{   
    if(node != NULL){
                //pretty_print(node,0);
                //cout << "------------------------" << endl;
        if( streq(node->name, formal_name) &&
            node->hint == ID_ATOM)
        {
            //cout << node->name << ',' << formal_name << ',' << node->value << endl;
            node->value = actual_value;
            node->type = INT;
            //cout << node->name << ',' << formal_name << ',' << node->value << endl;
        }
        set_actual_param(car(node), formal_name, actual_value);
        set_actual_param(cdr(node), formal_name, actual_value);
    }
}

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
    if( car_type == FUNC && state != DEFINING){ // before eval 
        state = FUNC_CALL; 
        if( incorrect_argnum(node, &car_eval_argnum) ) { 
            error_report(node, INCORRECT_ARGNUM, &state);
        }
        Type type;
        Value value;
        int argnum;
        auto error_flag = get_from_symtab(car(node)->name, 
                                          &type, &value, &argnum);
        if(error_flag == UNBOUND_VARIABLE) {
            //error_report(node, UNBOUND_VARIABLE, &state);
        }else{
            if(value != 0ll){ // not primitive.
                //cout << '{' << car(node)->name << '}' << endl;
                auto* origin = new_node2(car(node), cdr(node),"pair");
                //pretty_print(origin,0);
                
                auto* copyed = copy_tree((Node*)value);
                car(node) = car(copyed);
                cdr(node) = cdr(copyed);
                //cout << "--node--" << endl;
                //pretty_print(node,0);
                //pretty_print(origin,0);
                //pretty_print(syntax_tree,0);

                Node* formal = node;
                Node* actual = origin;
                while(formal != NULL && actual != NULL ){
                    if( car(formal) != NULL &&
                        car(actual) != NULL)
                    {
                        set_actual_param(node, car(formal)->name, car(actual)->value);
                    }
                    formal = cdr(formal);
                    actual = cdr(actual); 
                }
                interpret(node, state, prevXX);
                //pretty_print(node,0);

                del_tree(&origin);
            }
        }
    }
    else if( car_type == DEFINE_TYPE ){ 
        state = DEFINING;
        if( incorrect_argnum(node, &car_eval_argnum) ) { 
            error_report(node, ILL_FORMED_SPECIAL_FORM, &state);
        }
        eval_define(node, &state);
    }
    else if( car_type == IF_TYPE ){
        //state = SELECTION;
        if(state != DEFINING){
            if( incorrect_argnum(node, &car_eval_argnum) ) { 
                error_report(node, ILL_FORMED_SPECIAL_FORM, &state);
            }
            //cout << '[' << car_eval_argnum << ']' << endl;
            eval_if(node, &state);
        }
    }

    Type cdr_type = NULL_NODE;
    if( cdr(node) != NULL ){
        // TODO: refactor?
        Inherit next_state;
        if( state == TOP_LIST || 
            state == DEFINING ||
            state == SELECTION)
        {
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
    if( car_type == FUNC && car(node) != NULL){ 
        if( state != DEFINING ){ // if defining, then skip!
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
            else if(streq(func_name,"=")){
                type_check_equal(node, &state, car_eval_argnum);
                eval_equal(node, &state);
            }
            else{ // call other function. CAN't.
            }
        }
    }


    if( node->hint == ID_ATOM ){
        if( state != DEFINING && state != FUNC_CALL){ // if defining, then skip!
            Value value;
            Type type;
            auto error_flag = get_from_symtab(node->name, 
                                              &type, &value, NULL);

            if(error_flag == UNBOUND_VARIABLE) {
                //error_report(node, UNBOUND_VARIABLE, &state);
            }else{
                node->type = type; 
                node->value = value; 
            }
        }
    }

    //cout << '[' << node->name << '|' << state << '|' << prevXX << "] \n"; 

    return node->type;
}
*/
