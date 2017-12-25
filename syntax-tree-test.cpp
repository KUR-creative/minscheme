#define CATCH_CONFIG_MAIN
#include "catch.hpp" 
#include <iostream>
#include <string>
using std::string;
using std::cout;
using std::endl;

#include "simp-tree.h"
#include "syntax-tree.h"

using Catch::Matchers::Equals;
TEST_CASE("root 1"){
    GIVEN("root"){
        Node root = {"name", (Type)ROOT_EXPR, -1};
    WHEN("get syntax tree"){
        auto* tree = static_cast<Expr*>( new_syntax_tree(&root) );
    THEN("check correctness"){
        REQUIRE_THAT( tree->name, Equals("name") );
        REQUIRE( tree->type == ROOT_EXPR );
        REQUIRE( tree->value == -1 );
    }}}
}

TEST_CASE(""){
    GIVEN("parse tree"){
        Node root = {"name", (Type)ROOT_EXPR, -1};
        Node c1 = {"c1", (Type)1, 1};
        Node c2 = {"c2", (Type)2, 2};
        Node c3 = {"c3", (Type)3, 3};
        Node c4 = {"c4", (Type)4, 4};
    WHEN("get syntax tree"){
    THEN("check correctness"){
    }}}
}

/*
TEST_CASE(""){
    GIVEN("root"){
    WHEN("get syntax tree"){
    THEN("check correctness"){
    }}}
}
    */
