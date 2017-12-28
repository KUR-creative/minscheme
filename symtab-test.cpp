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
    int errflag = get_from_symtab("asd", NULL, NULL, NULL);
    REQUIRE(errflag == UNBOUND_VARIABLE);
}

TEST_CASE("add symbol"){
    Value expected = 14;
    char name[] = "as-df";
    add_to_symtab(name, INT, expected, 0);

    Type type;
    Value actual = 0;
    int errflag = get_from_symtab(name, &type, &actual, NULL);
    REQUIRE(errflag == NO_ERR);
    REQUIRE(actual == expected);
}

TEST_CASE("update symbol: re-binding ocurred."){
    Value old = 1;
    Value expected = 14;
    char name[] = "new";

    add_to_symtab(name, INT, old, 0);
    add_to_symtab(name, INT, expected, 0);

    Type type;
    Value actual = 0;
    int errflag = get_from_symtab(name, &type, &actual, NULL);
    REQUIRE(errflag == NO_ERR);
    REQUIRE(actual == expected);
}
