#define CATCH_CONFIG_MAIN
#include "catch.hpp" 
#include <string>
using std::string;
using std::cout;
using std::endl;

extern "C"
{
#include "simp-tree.h"
}

TEST_CASE("del_tree must be set deleted root NULL")
{
    Node* node = atom("name", INT, 1ll, 1ll);
    del_tree(&node);
    REQUIRE(node == NULL);
}

TEST_CASE("list len"){
    Node* li = NULL;
    REQUIRE(list_len(li) == 0);
    
    li = new_node2(NULL,NULL,"root");
    REQUIRE(list_len(li) == 1);
    del_tree(&li);
}

TEST_CASE("list len = 2"){
    Node* root = 
        new_node2(new_node2(NULL,NULL, "car"),
                  new_node2(new_node2(NULL,NULL,"cadr"), 
                            NULL, "cdr"),
                  "roor");
    pretty_print(root,0);
    REQUIRE(list_len(root) == 2);
    del_tree(&root);
}

TEST_CASE("list len = 4"){
    Node* root = 
        new_node2(new_node2(NULL,NULL, "car"),
                  new_node2(new_node2(NULL,NULL, "cadr"),
                            new_node2(new_node2(NULL,NULL, "caddr"),
                                      new_node2(new_node2(NULL,NULL,"cadddr"), 
                                                NULL, "cdddr"),
                                      "cddr"),
                            "cdr"),
                  "root");
    pretty_print(root,0);
    REQUIRE(list_len(root) == 4);
    del_tree(&root);
}

TEST_CASE("LL LR RL RR"){
    SECTION("RR"){
        REQUIRE(push_car(RR) == RL);
        REQUIRE(push_cdr(RR) == RR);
    }
    SECTION("RL"){
        REQUIRE(push_car(RL) == LL);
        REQUIRE(push_cdr(RL) == LR);
    }
    SECTION("LL"){
        REQUIRE(push_car(LL) == LL);
        REQUIRE(push_cdr(LL) == LR);
    }
    SECTION("LR"){
        REQUIRE(push_car(LR) == RL);
        REQUIRE(push_cdr(LR) == RR);
    }
    //cout << RR << endl;
    //cout << RL << endl;
    //cout << LL << endl;
    //cout << LR << endl;
}
