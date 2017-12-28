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

    auto fp = (ValxVal_Val)get_body(val);
    Value result = fp(1ll,2ll);
    REQUIRE(result == 3ll);

    auto arglist = get_arglist(val);
    REQUIRE(car(arglist)->type == INT);
    REQUIRE(cadr(arglist)->type == INT);
    deinit_symtab();
}
