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
    REQUIRE(yylex() == IDENTIFIER);
    REQUIRE(yylex() == INTEGER);

    del_mockfile(mockfile);
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
        REQUIRE(yylex() == IDENTIFIER);

        finish_yyin_to_eof();
        del_mockfile(mockfile);
    }

    SECTION("alphabets"){
        char    tmpstr[]= "ABCDEFGHIJKLMN";
        FILE*   mockfile= new_mockfile(tmpstr);

        yyin = mockfile;
        REQUIRE(yylex() == IDENTIFIER);

        finish_yyin_to_eof();
        del_mockfile(mockfile);
    }

    SECTION("special charaters"){
        char    tmpstr[]= "hypen-id +id";
        FILE*   mockfile= new_mockfile(tmpstr);

        yyin = mockfile;
        REQUIRE(yylex() == IDENTIFIER);
        REQUIRE(yylex() == IDENTIFIER);

        finish_yyin_to_eof();
        del_mockfile(mockfile);
    }

    SECTION("integers and identifier differnce"){
        char    tmpstr[]= "134234 243-sde";
        FILE*   mockfile= new_mockfile(tmpstr);

        yyin = mockfile;
        REQUIRE(yylex() == INTEGER);
        REQUIRE(yylex() == IDENTIFIER);

        finish_yyin_to_eof();
        del_mockfile(mockfile);
    }

    SECTION("parentheses"){
        char    tmpstr[]= "( ))( )";
        FILE*   mockfile= new_mockfile(tmpstr);

        yyin = mockfile;
        REQUIRE(yylex() == '(');
        REQUIRE(yylex() == ')');
        REQUIRE(yylex() == ')');
        REQUIRE(yylex() == '(');
        REQUIRE(yylex() == ')');

        finish_yyin_to_eof();
        del_mockfile(mockfile);
    }
    SECTION("break?"){
        char    tmpstr[]= "(1 2)";
        FILE*   mockfile= new_mockfile(tmpstr);

        yyin = mockfile;
        REQUIRE(yylex() == '(');
        REQUIRE(yylex() == INTEGER);
        REQUIRE(yylex() == INTEGER);
        REQUIRE(yylex() == ')');

        finish_yyin_to_eof();
        del_mockfile(mockfile);
    }
} 
