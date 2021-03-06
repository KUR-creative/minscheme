#define CATCH_CONFIG_MAIN
#include "catch.hpp" 
#include <string>
using std::string;
using std::cout;
using std::endl;
using Catch::Matchers::Equals;

extern "C"
{
#include "syscalls.h"
#include "test-utils.h"
#include "minscheme.tab.h"
#include "symtab.h"
#include "simp-tree.h"
#include "interpreter.h"
#include "errors.h"

extern FILE *yyin;
extern FILE *yyout;
extern void yyerror(char *);
extern int yylex(void);
}



/*
*/
TEST_CASE("get function pointer(body) of primitive function."){
    init_symtab();
    Type type;
    Value value;
    int err = get_from_symtab("add2", &type, &value);
    REQUIRE(err != UNBOUND_VARIABLE);

    auto fp = (ValxVal_Val)get_body(value);
    REQUIRE(fp(1ll,2ll) == 3ll);
    //ValxVal_Val fp = add2;
    //printf("[%lld]", fp(1ll, 2ll));
    deinit_symtab();
}

class IOFixture {
    FILE*   prog_outfile;
    bool    actual_called;
public:
    IOFixture(): actual_called(false) {
        init_symtab();
    }
    void set_prog_src(char* prog_src, bool print = false){
        FILE*   mockfile= new_mockfile(prog_src);
        FILE*   outfile = Fopen("out", "w");
        yyin = mockfile;
        yyout = outfile;

        yyparse();
        if(print){
            pretty_print(syntax_tree,0);
            cout << prog_src << endl;
            cout << "--------------------------------\n";
        }
        interpret(syntax_tree, RR);

        del_mockfile(mockfile);
        Fclose(outfile);
    }

    string actual(){
        prog_outfile = Fopen("out","r");
        char ret_buf[256] = {'\0'};
        fread(ret_buf, sizeof ret_buf[0], 256, prog_outfile);
        return string(ret_buf);
    }

    ~IOFixture() {
        deinit_symtab();
        if( ! actual_called )
            actual();
        Fclose(prog_outfile);
        Remove("out");
        clear_stack();
        del_tree(&syntax_tree);
        syntax_tree = NULL;
    }
};

// primitives: display
TEST_CASE_METHOD(IOFixture, "display 1, display is function."){
    set_prog_src("(display 123)",true);
    auto root = syntax_tree;
    REQUIRE_THAT( actual(), Equals("123") );
}
TEST_CASE_METHOD(IOFixture, "display other"){
    set_prog_src("(display 264)");
    REQUIRE_THAT( actual(), Equals("264") );
}
TEST_CASE_METHOD(IOFixture, "multiple display"){
    set_prog_src("(display 1)(display 234)");
    REQUIRE_THAT( actual(), Equals("1234") );
}
TEST_CASE_METHOD(IOFixture, "multiline display"){
    set_prog_src("(display 451)\n(display 452)");
    REQUIRE_THAT( actual(), Equals("451452") );
}

//errors
// arg num
TEST_CASE_METHOD(IOFixture, "more: display must be called 1 arguments."){
    set_prog_src("(display 1 2)");
    REQUIRE_THAT( actual(), Equals(string("display")
                                  +string(incorrect_argnum_errmsg)) );
}
TEST_CASE_METHOD(IOFixture, "less: display must be called 1 arguments."){
    set_prog_src("(display)");
    REQUIRE_THAT( actual(), Equals(string("display")
                                  +string(incorrect_argnum_errmsg)) );
}
// unbound variable
TEST_CASE_METHOD(IOFixture, "unbound variable error in terminal"){
    // it should be parsed. it is SEMANTIC error.
    set_prog_src("unbound-id");
    REQUIRE_THAT( actual(), Equals(string("unbound-id")
                                  +string(unbound_variable_errmsg)));
}
TEST_CASE_METHOD(IOFixture, "unbound variable error2"){
    // it should be parsed. it is SEMANTIC error.
    set_prog_src("(display unde3)");
    REQUIRE_THAT( actual(), Equals(string("unde3")
                                  +string(unbound_variable_errmsg)));
}
TEST_CASE_METHOD(IOFixture, "critical error aborts the execution"){
    set_prog_src("unbound-id (display 3) (display -5)");
    REQUIRE_THAT( actual(), Equals(string("unbound-id")
                                  +string(unbound_variable_errmsg)) );
}

TEST_CASE_METHOD(IOFixture, "unbound variable error in LL terminal"){
    // it should be parsed. it is SEMANTIC error.
    set_prog_src("(unbound-id)");
    REQUIRE_THAT( actual(), Equals(string("unbound-id")
                                  +string(unbound_variable_errmsg)));
}

// not applicable
TEST_CASE_METHOD(IOFixture, "interger is not applicable"){
    set_prog_src("(141 2)");
    REQUIRE_THAT( actual(), Equals(string("141")
                                  +string(not_applicable_errmsg)) );
}
TEST_CASE_METHOD(IOFixture, "interger is not applicable 2"){
    set_prog_src("(141 2 5 4)");
    REQUIRE_THAT( actual(), Equals(string("141")
                                  +string(not_applicable_errmsg)) );
}

TEST_CASE_METHOD(IOFixture, "newline: () -> notype: output func"){
    set_prog_src("(newline)");
    REQUIRE_THAT( actual(), Equals("\n") );
}

TEST_CASE_METHOD(IOFixture, "bool: #t"){
    set_prog_src("#t");
    REQUIRE_THAT( actual(), Equals("") );
}
TEST_CASE_METHOD(IOFixture, "display bool: #t"){
    set_prog_src("(display #t)(display #f)");
    REQUIRE_THAT( actual(), Equals("truefalse") );
}

// add2
TEST_CASE_METHOD(IOFixture, "add2 = arg1 + arg2"){
    set_prog_src("(display (add2 2 4))");
    REQUIRE_THAT( actual(), Equals("6") );
}
TEST_CASE_METHOD(IOFixture, "recursive calculation 1"){
    set_prog_src("(display (add2 1 (add2 2 3)))");
    REQUIRE_THAT( actual(), Equals("6") );
}
TEST_CASE_METHOD(IOFixture, "recursive calculation 2"){
    set_prog_src("(display (add2 (add2 10 40) (add2 (add2 4 8) 3)))");
    REQUIRE_THAT( actual(), Equals("65") );
}


TEST_CASE_METHOD(IOFixture, "ill-formed special form"){
    set_prog_src("(display lambda)",true);
    REQUIRE_THAT( actual(), Equals(string("lambda")
                                  +string(ill_formed_special_form_errmsg)));
}
// lambda
TEST_CASE_METHOD(IOFixture, "lambda: get user-defined function"){
    set_prog_src("(display ((lambda () 5)))",true);
    REQUIRE_THAT( actual(), Equals("5") );
}
TEST_CASE_METHOD(IOFixture, "lambda: get user-defined function arg1"){
    set_prog_src("(display ((lambda (x) 5) 3))",true);
    REQUIRE_THAT( actual(), Equals("5") );
}
TEST_CASE_METHOD(IOFixture, "lambda: get arglist and body"){
    set_prog_src("(lambda (x) 5)",true);
    Node* func_node = cdr(car(syntax_tree)); // arglist <- funcnode -> p -> body
    CHECK( car(car(func_node))->type == GENERIC );

    Node* arglist   = car(func_node); 
    Node* body      = cadr(func_node);
    //cout << arglist->name << '|' << get_arglist((Value)func_node)->name << endl;
    //cout << body->name << '|' << ((Node*)get_body((Value)func_node))->name << endl;
    CHECK( arglist == get_arglist((Value)func_node) );
    CHECK( body == (Node*)get_body((Value)func_node) );
}
/*
TEST_CASE_METHOD(IOFixture, "=: int x int -> bool"){
    set_prog_src("(display (= 1 1))");
    REQUIRE_THAT( actual(), Equals("true") );
}

TEST_CASE_METHOD(IOFixture, "sub2 = arg1 - arg2"){
    set_prog_src("(display (sub2 2 40))");
    REQUIRE_THAT( actual(), Equals("-38") );
}


// arg num tests.
TEST_CASE_METHOD(IOFixture, "add2 must be called 2 arguments."){
    set_prog_src("(add2 1 2)");
    REQUIRE_THAT( actual(), Equals("") );
}
TEST_CASE_METHOD(IOFixture, "add2 must be called 2 arguments. not 1"){
    set_prog_src("(add2 2)");
    REQUIRE_THAT( actual(), Equals(string("add2")
                                  +string(incorrect_argnum_errmsg)) );
}
TEST_CASE_METHOD(IOFixture, "add2 must be called 2 arguments. not 0"){
    set_prog_src("(add2)");
    REQUIRE_THAT( actual(), Equals(string("add2")
                                  +string(incorrect_argnum_errmsg)) );
}
TEST_CASE_METHOD(IOFixture, "sub2 must be called 2 arguments. not 1"){
    set_prog_src("(sub2 2 2 4 3)");
    REQUIRE_THAT( actual(), Equals(string("sub2")
                                  +string(incorrect_argnum_errmsg)) );
}
TEST_CASE_METHOD(IOFixture, "sub2 must be called 2 arguments. not 0"){
    set_prog_src("(sub2)");
    REQUIRE_THAT( actual(), Equals(string("sub2")
                                  +string(incorrect_argnum_errmsg)) );
}
TEST_CASE_METHOD(IOFixture, "mul2 = arg1 * arg2"){
    set_prog_src("(display (mul2 2 4))");
    REQUIRE_THAT( actual(), Equals("8") );
}
TEST_CASE_METHOD(IOFixture, "div2 = arg1 / arg2"){
    set_prog_src("(display (div2 8 4))");
    REQUIRE_THAT( actual(), Equals("2") );
}

TEST_CASE_METHOD(IOFixture, "define function"){
    set_prog_src("(define (f arg) arg) (display (f 3))",true);
    //pretty_print(syntax_tree,0);
    REQUIRE_THAT( actual(), Equals("3") );
}
TEST_CASE_METHOD(IOFixture, "define if"){
    set_prog_src("(define (f arg) (if arg 3 -4))",true);
    //pretty_print(syntax_tree,0);
    REQUIRE_THAT( actual(), Equals("") );
}
TEST_CASE_METHOD(IOFixture, "define if, call","[.]"){
    set_prog_src("(define (f arg) (if arg 3 -4)) (display (f #t))",true);
    //pretty_print(syntax_tree,0);
    REQUIRE_THAT( actual(), Equals("3") );
}

// 치환은 eval과 함께 이루어져야 한다.....
TEST_CASE_METHOD(IOFixture, "get factorial definition tree"){
    set_prog_src("(define (fac n) (if (= n 1) 1 (mul2 n (fac (sub2 n 1))))) (fac 4) (display (fac 5))",true);
    cout << "after interpret" << endl;
    //pretty_print(syntax_tree,0);
    cout << "--------------" << endl;
    // just look and feel: pretty_print
}
TEST_CASE_METHOD(IOFixture, "look"){
    set_prog_src("((lambda (a b) (+ a b)) 1 2)");
    // just look and feel: pretty_print
}
TEST_CASE_METHOD(IOFixture, "get factorial call tree"){
    set_prog_src("(display (fac 5))\n(display 3)(display (fac 4))");
    // just look and feel: pretty_print
}
TEST_CASE_METHOD(IOFixture, "get parse tree 1"){
    set_prog_src("(1 2 (3 4 5 6) (7 8) 9)");
    // just look and feel: pretty_print
}
TEST_CASE_METHOD(IOFixture, "get parse tree 2"){
    set_prog_src("(1 2 3 4)");
    // just look and feel: pretty_print
}
TEST_CASE_METHOD(IOFixture, "get parse tree 3"){
    set_prog_src("(0 (1 2) (3 4 5) 6)");
    // just look and feel: pretty_print
}

TEST_CASE_METHOD(IOFixture, "add2"){
    set_prog_src("(define (add2 a b) (+ a b)) (display (add2 1 6))");
    // just look and feel: pretty_print
}

TEST_CASE("pair-[display,p]"){
    Node* node = pair("pair", GENERIC, UNKNOWN_VAL, EXPR_PAIR,
                     atom("display", DISP_NODE, UNKNOWN_VAL, ID_ATOM),
                     pair("end-pair", GENERIC, UNKNOWN_VAL, END_PAIR, 
                         atom("10", INT, 10, INT_ATOM), NULL));
    Hint hint = interpret(node);
    //REQUIRE(hint = 
}


TEST_CASE_METHOD(IOFixture, "define: must be used with 2 expressions."){
    set_prog_src("(define newid 12 43)");
    REQUIRE_THAT( actual(), Equals(string("define")
                                  +string(ill_formed_special_form_errmsg)));
}

TEST_CASE_METHOD(IOFixture, "define: id is unbound variable, but no error."){
    set_prog_src("(define newid 12)");
    REQUIRE_THAT( actual(), Equals("") );
}

TEST_CASE_METHOD(IOFixture, "define: add new symbol to symtab"){
    set_prog_src("(define id1 12)");
    Value retval = 0;
    Type type;
    int argnum;
    get_from_symtab("id1", &type, &retval, &argnum);
    REQUIRE(retval == 12);
}
TEST_CASE_METHOD(IOFixture, "define: enviroment, and use id"){
    set_prog_src("(define identifier 846)(display identifier)");

    REQUIRE_THAT( actual(), Equals("846") );

    Value retval = 0;
    Type type;
    int argnum;
    get_from_symtab("identifier", &type, &retval, &argnum);
    REQUIRE(retval == 846);
}
TEST_CASE_METHOD(IOFixture, "define: is type saved in symtab correctly?"){
    set_prog_src("(define newid 12)");
    Value retval = 0;
    Type type;
    int argnum;
    get_from_symtab("identifier", &type, &retval, &argnum);
    REQUIRE(type == INT);
}

TEST_CASE_METHOD(IOFixture, "caadr"){
    set_prog_src("(define (func a b) (add2 a b))");
    auto def_expr = car(syntax_tree);
    REQUIRE_THAT( car(def_expr)->name, Equals("define") );
    REQUIRE_THAT( caadr(def_expr)->name, Equals("func") );
}

// why don't eval.. ?
TEST_CASE_METHOD(IOFixture, "define: function"){
    set_prog_src("(define (func a b) (add2 a b))");
    //pretty_print(syntax_tree,0);
    Value retval = 0;
    Type type;
    int argnum;
    auto err = get_from_symtab("func", &type, &retval, &argnum);
    REQUIRE( err != UNBOUND_VARIABLE );
    REQUIRE( type == FUNC );
    REQUIRE( argnum == 2 );
    auto* body_ptr = (Node*)retval;
    REQUIRE_THAT( car(body_ptr)->name , Equals("add2")); 
}
TEST_CASE_METHOD(IOFixture, "define: function, and call"){
    set_prog_src("(define (func a b) (add2 a b)) (display (func 3 4))", true);
    cout << "===== after interpret =====" << endl;
    //pretty_print(syntax_tree,0);
    Value retval = 0;
    Type type;
    int argnum;
    auto err = get_from_symtab("func", &type, &retval, &argnum);   
    cout << '[' << type << '|' << retval << '|' << argnum << ']' << endl;
    REQUIRE( err != UNBOUND_VARIABLE );
    REQUIRE_THAT( actual(), Equals("7") );
}
// 모든 arg의 이름이 다른가?
// arg로 다른 스페셜폼을 쓰지는 않았나?
// 등등... 예외는 많다..
//
TEST_CASE_METHOD(IOFixture, "copy tree"){
    set_prog_src("(1 2 (3 4) (5 6))");
    auto* copyed = copy_tree(syntax_tree);
    //pretty_print(copyed,0);
    //cout << "!!!! !!! !!! !!!! !!!!!" << endl;
    // copyed! maybe...
}

//(if #f (display 1)(display 0))
TEST_CASE_METHOD(IOFixture, "if: selection"){
    set_prog_src("(if #t (display 1)(display 0)) ");
    REQUIRE_THAT( actual(), Equals("1") );
}
TEST_CASE_METHOD(IOFixture, "if: selection - false"){
    set_prog_src("(if #f (display 1)(display 0)) ");
    REQUIRE_THAT( actual(), Equals("0") );
}
TEST_CASE_METHOD(IOFixture, "if: condition must be bool type."){
    set_prog_src("(if 1 (display 1)(display 0))");
    REQUIRE_THAT( actual(), Equals(string("if")
                                  +string(type_mismatch_errmsg)) );
}
TEST_CASE_METHOD(IOFixture, "if: result can be any type."){
    set_prog_src("(display (if #t 1 #f)) (display (if #f 1 #f))");
    REQUIRE_THAT( actual(), Equals("1false") );
}
TEST_CASE_METHOD(IOFixture, "if: recursive evaluation."){
    set_prog_src("(display (if #t (add2 3 4) #f))");
    REQUIRE_THAT( actual(), Equals("7") );
}
TEST_CASE_METHOD(IOFixture, "if: recursive evaluation2."){
    set_prog_src("(display (if (= 4 4) (add2 3 4) #f))");
    REQUIRE_THAT( actual(), Equals("7") );
}


TEST_CASE_METHOD(IOFixture, "display must be called 1 arguments."){
    set_prog_src("(display 1 2)");
    REQUIRE_THAT( actual(), Equals(string("display")
                                  +string(incorrect_argnum_errmsg)) );
}
// primitive function checks the type equivalence.

TEST_CASE_METHOD(IOFixture, "display: arg type is int"){
    set_prog_src("(display display)");
    //pretty_print(syntax_tree,0);
    // skip this case. just no segfault.
    //REQUIRE_THAT( actual(), Equals(string("(display display)")
                                  //+string(type_mismatch_errmsg)) );
}
// TODO:
TEST_CASE_METHOD(IOFixture, "add2:int x int -> int"){
    set_prog_src("(add2 sub2 sub2)");
    //pretty_print(syntax_tree,0);
    REQUIRE_THAT( actual(), Equals(string("(add2 sub2 sub2)")
                                  +string(type_mismatch_errmsg)) );
}
TEST_CASE_METHOD(IOFixture, "error aborts the evaluation."){
    set_prog_src("(add2 add2 4)");
    //pretty_print(syntax_tree,0);
    REQUIRE_THAT( actual(), Equals(string("(add2 add2 4)")
                                  +string(type_mismatch_errmsg)) );
}

TEST_CASE_METHOD(IOFixture, "if func is arg, then don't eval the func.","[.]"){
    set_prog_src("(add2 display 4)");
    REQUIRE_THAT( actual(), Equals(string("(add2 display 4)")
                                  +string(type_mismatch_errmsg)) );
}

// later..
TEST_CASE_METHOD(IOFixture, "critical error aborts the execution","[.]"){
    set_prog_src("unbound-id (display 3) (display -5)");
    REQUIRE_THAT( actual(), Equals(string("unbound-id")
                                  +string(unbound_variable_errmsg)) );
}

// calculation
TEST_CASE_METHOD(IOFixture, "calculation"){
    set_prog_src("(define a 12)     \
                  (define b 24)     \
                  (define c  2)     \
                  (display (add2 a b)) \
                  (newline)         \
                  (display (sub2 b c))");
    REQUIRE_THAT( actual(), Equals("36\n22") );
}

TEST_CASE_METHOD(IOFixture, "calculation2"){
    set_prog_src("(define a 12)     \
                  (define b 24)     \
                  (define c  2)     \
                  (display (add2 a (add2 b c))) \
                  (newline)         \
                  (display (sub2 b (sub2 c a)))");
    REQUIRE_THAT( actual(), Equals("38\n34") );
}

*/
