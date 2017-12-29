#include "interpreter.h"
#include "errors.h"
#include "symtab.h"
#include <iostream>
#include <cassert>
#include <string.h> 
#include <string>
#include <stack>
#include <tuple>

Node* syntax_tree = NULL;

extern "C"
{
extern FILE *yyout;
}

using std::stack;
using std::tuple;
using std::make_tuple;
using std::get;
using std::string;
using std::cout;
using std::endl;

inline static
bool streq(char* s1, char* s2){
    return (strcmp(s1, s2) == 0);
}

inline static
void view_stack(Node* now_node, stack<tuple<Node,int,int>>& s){
    if(now_node != NULL){
        cout << now_node->name << ' ';
    }
    if(! s.empty()){
        cout << '[' << get<0>(s.top()).name 
             << ',' << get<0>(s.top()).type 
             << ',' << get<0>(s.top()).value 
             << '|' << get<1>(s.top()) 
             << '|' << get<2>(s.top()) << ']';
    }
}

inline static
bool is_terminal(Node* node){
    return (node->type != PAIR);
}

stack<tuple<Node,int,int>> run_stack;
stack<tuple<Node,int,int>> tmp_stack;

void clear_stack(void)
{
    run_stack = stack<tuple<Node,int,int>>();
    tmp_stack = stack<tuple<Node,int,int>>();
}

State interpret(Node* node, State state)
{
    State car_state = NOstate;
    if( car(node) != NULL ){
        car_state = interpret(car(node), push_car(state));
    }
    if( car_state == ERRORstate ){
        return ERRORstate;
    }
    if(car_state == LAMBDAstate){
        free(node->name); // change name to 'lambda' to apply this node.
        node->name = strdup("lambda");

        Node*   func_node   = cdr(node); 
        Node    saving_node = *func_node; // copy!
        free(saving_node.name); // just for dbg
        saving_node.name = strdup("lambda");

        Node*   arglist         = cadr(node); 
        int     max_argnum      = list_len(arglist);
        cout << '(' << max_argnum << ')' << endl;

        run_stack.push( make_tuple(saving_node,max_argnum,0) );
        //view_stack(node, run_stack); cout << endl;
        return LL;// means "don't evaluate sub-tree of node."
                  // do not move to cdr!
    }

    State cdr_state = NOstate;
    if( cdr(node) != NULL ){
        cdr_state = interpret(cdr(node), push_cdr(state));
    }
    if( cdr_state == ERRORstate ){
        return ERRORstate;
    }



    //cout << '[' << node->name << '|' << state << "] \n"; 
    if(node->type == SPECIAL){
        if(state != LL){
            string errmsg = string(node->name) 
                            + string(ill_formed_special_form_errmsg);
            fprintf(yyout, "%s", errmsg.c_str());
            return ERRORstate;
        }
        if(streq(node->name,"lambda")){
            return LAMBDAstate;
        }
    }

    if(is_terminal(node)){ 
        char* name = node->name;
        Type type;
        Value val;
        if(node->type == GENERIC){
            int err = get_from_symtab(name, &type, &val);
            if(err == UNBOUND_VARIABLE){
                string errmsg = string(node->name) 
                                + string(unbound_variable_errmsg);
                fprintf(yyout, "%s", errmsg.c_str());
                return ERRORstate;
            }
            // set actual type/value.
            node->type = type; 
            node->value = val;
        }

        if(state == LL){ // evaluation! - push func node to stack.
            if(node->type != FUNC){
                string errmsg = string(node->name) 
                                + string(not_applicable_errmsg);
                fprintf(yyout, "%s", errmsg.c_str());
                return ERRORstate;
            }
            Node*   arglist = get_arglist(val); 
            int     argnum  = list_len(arglist);
            run_stack.push( make_tuple(*node,argnum,0) );
        }
        else{ 
            if(! run_stack.empty()){
                auto    top             = run_stack.top();
                auto    max_argnum      = get<1>(top);
                auto    accepted_argnum = get<2>(top) + 1;
                run_stack.push( make_tuple(*node,max_argnum,accepted_argnum) );
            }
        }
    }

    if(car_state == LL){ // application! - pop func and calculate!
        auto    top         = run_stack.top();
        auto    max_argnum  = get<1>(top);
        auto    accepted_num= get<2>(top); // no push. no +1

        cout << '(' << max_argnum << ',' << accepted_num << ')' << endl;
        if(max_argnum == accepted_num){ 
            char* car_name = car(node)->name; 
            Node* ret_node;
            if(streq(car_name,"lambda")){
                cout << "!!!!!!!!!!!!!!!" << endl;
                // modify ret_node.
            }
            else if(streq(car_name,"display")){
                // get arguments
                Node arg        = get<0>(run_stack.top());
                run_stack.pop();
                Value func_ptr  = get<0>(run_stack.top()).value;
                run_stack.pop();

                auto func       = (pNode_Val)get_body(func_ptr);
                ret_node = atom("no_type", NO_TYPE, func(&arg));
            }
            else if(streq(car_name, "newline")){
                Value func_ptr  = get<0>(run_stack.top()).value;
                run_stack.pop();

                auto func       = (none_Val)get_body(func_ptr);
                ret_node = atom("no_type", NO_TYPE, func());
            }
            else if(streq(car_name, "add2")){
                Node arg2       = get<0>(run_stack.top());
                run_stack.pop();
                Node arg1       = get<0>(run_stack.top());
                run_stack.pop();
                Value func_ptr  = get<0>(run_stack.top()).value;
                run_stack.pop();

                auto func       = (ValxVal_Val)get_body(func_ptr);
                ret_node = atom("int", INT, func(arg1.value, arg2.value));
            }

            // return value will be argument of func that was early called.
            if(! run_stack.empty()){
                auto    top         = run_stack.top();
                auto    max_argnum  = get<1>(top);
                auto    accepted_num= get<2>(top) + 1; 
                run_stack.push( make_tuple(*ret_node,max_argnum,accepted_num) );
                //del_node(&ret_node); // segfault???
            }
        }
        else{
            string errmsg = string(car(node)->name) 
                            + string(incorrect_argnum_errmsg);
            fprintf(yyout, "%s", errmsg.c_str());
            return ERRORstate;
        }
    }
    
    // watch stack.
    //view_stack(node, run_stack); cout << endl;
    return state;
}

            /*
            for(int i = 0; i < max_argnum + 1; i++){ // pop function too!
                tmp_stack.push(run_stack.top());
                run_stack.pop();

                view_stack(NULL, run_stack);
                view_stack(NULL, tmp_stack);
                cout << endl;
            }

            Value disp_ptr  = get<0>(tmp_stack.top()).value;
            tmp_stack.pop();
            Node arg        = get<0>(tmp_stack.top());
            tmp_stack.pop();

            auto func       = (pNode_Val)get_body(disp_ptr);
            func(&arg); // 이거 어째야 되는데? 커링인가 뭔가 해야되나..
            // arity를 같이 주는 방법이 있다..
            */

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
