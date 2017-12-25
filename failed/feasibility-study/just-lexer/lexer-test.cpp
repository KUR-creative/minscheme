#define CATCH_CONFIG_MAIN
#include "catch.hpp" 

#include <iostream>
//#include <string>
using std::cout;
using std::endl;
//using std::string;

extern "C"
{
#include "test-utils.h"
#include "minscheme.tab.h"

extern FILE *yyin;
extern void yyerror(char *);
extern int yylex(void);
}

// DO NOT call yylex anymore!
void finish_yyin_to_eof(void){
    while(yylex() != 0);
}

using Catch::Matchers::Equals;
TEST_CASE("mockfile test"){
    char    tmpstr[]= "test src";
    FILE*   mockfile= new_mockfile(tmpstr);
    char    actual[256];

    fgets(actual, 256, mockfile);
    del_mockfile(mockfile);

    REQUIRE_THAT( actual, Equals(tmpstr) );
    //cout << "actual: " << actual << endl;
    //cout << "expect: " << tmpstr << endl;
}

TEST_CASE("yyXXX"){
    char    tmpstr[]= "aa 1";
    FILE*   mockfile= new_mockfile(tmpstr);
    yyin = mockfile;

    int     id      = yylex();
    int     integer = yylex();

    del_mockfile(mockfile);
    REQUIRE(id == IDENTIFIER);
    REQUIRE(integer == INTEGER);
}

TEST_CASE("end read file: get eof"){
    char    tmpstr[]= "a 1 b 2 c asd e asdA";
    FILE*   mockfile= new_mockfile(tmpstr);
    yyin = mockfile;

    finish_yyin_to_eof();

    del_mockfile(mockfile);
}

SCENARIO("identifier"){
    SECTION("alphabets"){
        char    tmpstr[]= "abcd";
        FILE*   mockfile= new_mockfile(tmpstr);
        yyin = mockfile;

        int     id      = yylex();
        finish_yyin_to_eof();

        del_mockfile(mockfile);
        REQUIRE(id == IDENTIFIER);
    }

    SECTION("alphabets"){
        char    tmpstr[]= "ABCDEFGHIJKLMN";
        FILE*   mockfile= new_mockfile(tmpstr);
        yyin = mockfile;

        int     id      = yylex();
        finish_yyin_to_eof();

        del_mockfile(mockfile);
        REQUIRE(id == IDENTIFIER);
    }

    SECTION("parentheses"){
        char    tmpstr[]= "( ))( )";
        FILE*   mockfile= new_mockfile(tmpstr);
        yyin = mockfile;

        REQUIRE(yylex() == OPENPAREN);
        REQUIRE(yylex() == CLOSEPAREN);
        REQUIRE(yylex() == CLOSEPAREN);
        REQUIRE(yylex() == OPENPAREN);
        REQUIRE(yylex() == CLOSEPAREN);
        finish_yyin_to_eof();

        del_mockfile(mockfile);
    }
} 
