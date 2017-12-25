#define CATCH_CONFIG_MAIN
#include "catch.hpp" 
#include <string>
using std::string;
using std::cout;
using std::endl;

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

// DO NOT call yylex anymore!
void finish_yyin_to_eof(void){
    while(yylex() != 0);
}

class IOFixture {
    FILE*   prog_outfile;
    bool    actual_called;
public:
    IOFixture(): actual_called(false){;}
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
        interpret(syntax_tree, TOP_LIST, RR);

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
        if( ! actual_called )
            actual();
        Fclose(prog_outfile);
        Remove("out");
        del_tree(&syntax_tree);
        syntax_tree = NULL;
    }
};

/*
TEST_CASE_METHOD(IOFixture, "get factorial definition tree"){
    set_prog_src("(define (fac n) (if (= n 1) 1 (* n (fac (- n 1))))) (fac 4) (disp (fac 5))");
    // just look and feel: pretty_print
}
TEST_CASE_METHOD(IOFixture, "look"){
    set_prog_src("((lambda (a b) (+ a b)) 1 2)");
    // just look and feel: pretty_print
}
TEST_CASE_METHOD(IOFixture, "get factorial call tree"){
    set_prog_src("(disp (fac 5))\n(disp 3)(disp (fac 4))");
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
    set_prog_src("(define (add2 a b) (+ a b)) (disp (add2 1 6))");
    // just look and feel: pretty_print
}

TEST_CASE("pair-[disp,p]"){
    Node* node = pair("pair", GENERIC, UNKNOWN_VAL, EXPR_PAIR,
                     atom("disp", DISP_NODE, UNKNOWN_VAL, ID_ATOM),
                     pair("end-pair", GENERIC, UNKNOWN_VAL, END_PAIR, 
                         atom("10", INT, 10, INT_ATOM), NULL));
    Hint hint = interpret(node);
    //REQUIRE(hint = 
}
*/

using Catch::Matchers::Equals;
TEST_CASE_METHOD(IOFixture, "check hints 1","[.]"){ // it is maybe useless.
    set_prog_src("(disp 1)");
    auto root = syntax_tree;
    REQUIRE( root->hint == END_PAIR );
    REQUIRE( car(root)->hint == EXPR_PAIR );
    REQUIRE( car(car(root))->hint == ID_ATOM);
}
TEST_CASE_METHOD(IOFixture, "disp 1, disp is function."){
    set_prog_src("(disp 1)");
    auto root = syntax_tree;
    REQUIRE_THAT( actual(), Equals("1") );
}
TEST_CASE_METHOD(IOFixture, "disp other"){
    set_prog_src("(disp 264)");
    REQUIRE_THAT( actual(), Equals("264") );
}
TEST_CASE_METHOD(IOFixture, "disp minus"){
    set_prog_src("(disp -523)");
    REQUIRE_THAT( actual(), Equals("-523") );
}

TEST_CASE_METHOD(IOFixture, "multiple disp"){
    set_prog_src("(disp 1)(disp 234)");
    REQUIRE_THAT( actual(), Equals("1234") );
}
TEST_CASE_METHOD(IOFixture, "multiline disp"){
    set_prog_src("(disp 451)\n(disp 452)");
    REQUIRE_THAT( actual(), Equals("451452") );
}

TEST_CASE_METHOD(IOFixture, "unbound variable error"){
    // it should be parsed. it is SEMANTIC error.
    set_prog_src("unbound-id");
    REQUIRE_THAT( actual(), Equals(string("unbound-id")
                                  +string(unbound_variable_errmsg)));
}
/* 
TEST_CASE_METHOD(IOFixture, "unbound variable error2"){
    // it should be parsed. it is SEMANTIC error.
    set_prog_src("(unbound-id2 unde3)");
    REQUIRE_THAT( actual(), Equals(string("unbound-id2")
                                  +string(unbound_variable_errmsg)
                                  +string("unde3")
                                  +string(unbound_variable_errmsg)));
}
*/
TEST_CASE_METHOD(IOFixture, "define: must be used with 2 expressions."){
    set_prog_src("(define newid 12 43)");
    REQUIRE_THAT( actual(), Equals(string("define")
                                  +string(ill_formed_special_form_errmsg)));
}

TEST_CASE_METHOD(IOFixture, "define: id is unbound variable, but no error."){
    set_prog_src("(define newid 12)");
    REQUIRE_THAT( actual(), Equals("") );
}
TEST_CASE_METHOD(IOFixture, "define: add new symbol to symtab","[.]"){
    set_prog_src("(define id1 12)");
    Value retval = 0;
    Type type;
    int argnum;
    get_from_symtab("id1", &type, &retval, &argnum);
    REQUIRE(retval == 12);
}
TEST_CASE_METHOD(IOFixture, "define: enviroment, and use id","[.]"){
    set_prog_src("(define identifier 846)(disp identifier)");

    REQUIRE_THAT( actual(), Equals("846") );

    Value retval = 0;
    Type type;
    int argnum;
    get_from_symtab("identifier", &type, &retval, &argnum);
    REQUIRE(retval == 846);
}
TEST_CASE_METHOD(IOFixture, "define: is type saved in symtab correctly?","[.]"){
    set_prog_src("(define newid 12)");
    Value retval = 0;
    Type type;
    int argnum;
    get_from_symtab("identifier", &type, &retval, &argnum);
    REQUIRE(type == INT);
}


//(if #f (disp 1)(disp 0))
TEST_CASE_METHOD(IOFixture, "if: selection"){
    set_prog_src("(if #t (disp 1)(disp 0)) ");
    REQUIRE_THAT( actual(), Equals("1") );
}
TEST_CASE_METHOD(IOFixture, "if: selection - false"){
    set_prog_src("(if #f (disp 1)(disp 0)) ",true);
    REQUIRE_THAT( actual(), Equals("0") );
}
TEST_CASE_METHOD(IOFixture, "if: condition must be bool type."){
    set_prog_src("(if 1 (disp 1)(disp 0))",true);
    REQUIRE_THAT( actual(), Equals(string("if")
                                  +string(type_mismatch_errmsg)) );
}
TEST_CASE_METHOD(IOFixture, "if: result can be any type."){
    set_prog_src("(disp (if #t 1 #f)) (disp (if #f 1 #f))",true);
    REQUIRE_THAT( actual(), Equals("1false") );
}
TEST_CASE_METHOD(IOFixture, "if: recursive evaluation."){
    set_prog_src("(disp (if #t (add2 3 4) #f))",true);
    REQUIRE_THAT( actual(), Equals("7") );
}
TEST_CASE_METHOD(IOFixture, "if: recursive evaluation2.","[.]"){
    set_prog_src("(disp (if (= 4 4) (add2 3 4) #f))",true);
    REQUIRE_THAT( actual(), Equals("7") );
}

//primitives             
TEST_CASE_METHOD(IOFixture, "=: int x int -> bool"){
    set_prog_src("(disp (= 1 1))",true);
    REQUIRE_THAT( actual(), Equals("true") );
}
TEST_CASE_METHOD(IOFixture, "bool: #t"){
    set_prog_src("#t",true);
    REQUIRE_THAT( actual(), Equals("") );
}
TEST_CASE_METHOD(IOFixture, "disp bool: #t"){
    set_prog_src("(disp #t)(newline)(disp #f)");
    REQUIRE_THAT( actual(), Equals("true\nfalse") );
}
TEST_CASE_METHOD(IOFixture, "=2: ","[.]"){
    set_prog_src("");
    REQUIRE_THAT( actual(), Equals("") );
}

TEST_CASE_METHOD(IOFixture, "add2 = arg1 + arg2"){
    set_prog_src("(disp (add2 2 4))");
    REQUIRE_THAT( actual(), Equals("6") );
}
TEST_CASE_METHOD(IOFixture, "sub2 = arg1 - arg2"){
    set_prog_src("(disp (sub2 2 40))");
    REQUIRE_THAT( actual(), Equals("-38") );
}
TEST_CASE_METHOD(IOFixture, "recursive calculation 1"){
    set_prog_src("(disp (add2 1 (add2 2 3)))");
    REQUIRE_THAT( actual(), Equals("6") );
}
TEST_CASE_METHOD(IOFixture, "recursive calculation 2"){
    set_prog_src("(disp (add2 (add2 10 40) (add2 (sub2 4 8) 3)))");
    REQUIRE_THAT( actual(), Equals("49") );
}
TEST_CASE_METHOD(IOFixture, "newline: () -> notype: output func"){
    set_prog_src("(newline)");
    REQUIRE_THAT( actual(), Equals("\n") );
}

// not applicable
TEST_CASE_METHOD(IOFixture, "interger is not applicable","[.]"){
    set_prog_src("(141 2)");
    REQUIRE_THAT( actual(), Equals(string("141")
                                  +string(not_applicable_errmsg)) );
}
TEST_CASE_METHOD(IOFixture, "interger is not applicable 2","[.]"){
    set_prog_src("(141 2 5 4)");
    REQUIRE_THAT( actual(), Equals(string("141")
                                  +string(not_applicable_errmsg)) );
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
    set_prog_src("(disp (mul2 2 4))");
    REQUIRE_THAT( actual(), Equals("8") );
}
TEST_CASE_METHOD(IOFixture, "div2 = arg1 / arg2"){
    set_prog_src("(disp (div2 8 4))");
    REQUIRE_THAT( actual(), Equals("2") );
}

TEST_CASE_METHOD(IOFixture, "disp must be called 1 arguments."){
    set_prog_src("(disp 1 2)");
    REQUIRE_THAT( actual(), Equals(string("disp")
                                  +string(incorrect_argnum_errmsg)) );
}
// primitive function checks the type equivalence.

TEST_CASE_METHOD(IOFixture, "disp: arg type is int"){
    set_prog_src("(disp disp)");
    pretty_print(syntax_tree,0);
    // skip this case. just no segfault.
    //REQUIRE_THAT( actual(), Equals(string("(disp disp)")
                                  //+string(type_mismatch_errmsg)) );
}
/* // TODO:
TEST_CASE_METHOD(IOFixture, "add2:int x int -> int"){
    set_prog_src("(add2 sub2 sub2)");
    pretty_print(syntax_tree,0);
    REQUIRE_THAT( actual(), Equals(string("(add2 sub2 sub2)")
                                  +string(type_mismatch_errmsg)) );
}
TEST_CASE_METHOD(IOFixture, "error aborts the evaluation."){
    set_prog_src("(add2 add2 4)");
    pretty_print(syntax_tree,0);
    REQUIRE_THAT( actual(), Equals(string("(add2 add2 4)")
                                  +string(type_mismatch_errmsg)) );
}
*/
TEST_CASE_METHOD(IOFixture, "if func is arg, then don't eval the func.","[.]"){
    set_prog_src("(add2 disp 4)", true);
    REQUIRE_THAT( actual(), Equals(string("(add2 disp 4)")
                                  +string(type_mismatch_errmsg)) );
}

// later..
TEST_CASE_METHOD(IOFixture, "critical error aborts the execution","[.]"){
    set_prog_src("unbound-id (disp 3) (disp -5)");
    REQUIRE_THAT( actual(), Equals(string("unbound-id")
                                  +string(unbound_variable_errmsg)) );
}


