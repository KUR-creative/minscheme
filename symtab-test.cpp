#define CATCH_CONFIG_MAIN
#include "catch.hpp" 
#include <string>
using std::string;
using std::cout;
using std::endl;

#include "errors.h"
#include "symtab.h"

// add symbol(entry) 
// update symbol 
// get symbol info(what?)
TEST_CASE("get unbound symbol -> set error flag"){
    init_symtab();

    int errflag = get_from_symtab("asd", NULL, NULL);
    REQUIRE(errflag == UNBOUND_VARIABLE);

    deinit_symtab();
}

TEST_CASE("add symbol"){
    init_symtab();

    Value expected = 14;
    char name[] = "as-df";
    add_to_symtab(name, INT, expected);

    Type type;
    Value actual = 0;
    int errflag = get_from_symtab(name, &type, &actual);
    REQUIRE(errflag == NO_ERR);
    REQUIRE(actual == expected);

    deinit_symtab();
}

TEST_CASE("update symbol: re-binding ocurred."){
    init_symtab();

    Value old = 1;
    Value expected = 14;
    char name[] = "new";

    add_to_symtab(name, INT, old);
    add_to_symtab(name, INT, expected);

    Type type;
    Value actual = 0;
    int errflag = get_from_symtab(name, &type, &actual);
    REQUIRE(errflag == NO_ERR);
    REQUIRE(actual == expected);

    deinit_symtab();
}

TEST_CASE("init symtab"){
    SECTION("if didn't init, then abort."){
        //add_to_symtab("1", (Type)1,(Value)1);
    }

    init_symtab();

    int err = get_from_symtab("add2", NULL, NULL);
    REQUIRE(err != UNBOUND_VARIABLE);

    deinit_symtab();
}

TEST_CASE("run with function pointer"){
    init_symtab();

    Value val;
    get_from_symtab("add2", NULL, &val);

    auto arglist = get_arglist(val);
    REQUIRE(car(arglist)->type == INT);
    REQUIRE(cadr(arglist)->type == INT);

    auto fp = (ValxVal_Val)get_body(val);
    Value result = fp(1ll,2ll);
    REQUIRE(result == 3ll);

    deinit_symtab();
}

TEST_CASE("display"){
    init_symtab();

    Value val;
    get_from_symtab("display", NULL, &val);

    auto arglist = get_arglist(val);
    REQUIRE(car(arglist)->type == GENERIC);

    auto fp = (pNode_Val)get_body(val);

    Node* integer = atom("int", INT, 123ll);
    Node* boolean = atom("bool", BOOL, 0l);
    double tmpfloat = 3254.6752;
    Node* floatnum = atom("float", FLOAT, double_to_Value(tmpfloat));
    //fp(integer);
    //fp(boolean);
    //fp(floatnum);
    deinit_symtab();
}

TEST_CASE("newline"){
    init_symtab();

    Value val;
    get_from_symtab("newline", NULL, &val);

    auto arglist = get_arglist(val);

    auto fp = (none_Val)get_body(val);
    //printf("1");
    //fp();
    //printf("2");
    deinit_symtab();
}

TEST_CASE("user-defined"){
    init_symtab();

    Node* arglist = pair("end-pair", PAIR, 0ll,
                        atom("x",GENERIC,0ll),
                        NULL);
    Node* body = pair("pair",PAIR,0ll,
                     atom("add2",FUNC,0ll), // val은 0이 아니게 될 것.
                     pair("pair",PAIR,0ll,
                         atom("x",GENERIC,0ll),
                         pair("pair",PAIR,0ll,
                             atom("1",INT,1ll),
                             NULL)));

    Node* func_val = 
        pair("lambda-expr", PAIR, 0ll, // name must be lambda-expr.
                   atom("lambda",SPECIAL,UNKNOWN_VAL),
                   pair("pair", PAIR, 0ll,
                       arglist,
                       pair("end-pair",PAIR,0ll,
                           body,
                           NULL)));
    add_to_symtab("user-defined",FUNC,(Value)func_val);
    Value val;
    get_from_symtab("user-defined", NULL, &val);

    auto obtained_arglist = get_arglist(val);
    CHECK(obtained_arglist == arglist);

    auto fp = (Node*)get_body(val);
    REQUIRE(fp == func_val);
    //printf("1");
    //fp();
    //printf("2");
    deinit_symtab();
}
