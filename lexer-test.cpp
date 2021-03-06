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
    REQUIRE(yylex() == IDtok);
    REQUIRE(yylex() == INTtok);

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
        REQUIRE(yylex() == IDtok);

        finish_yyin_to_eof();
        del_mockfile(mockfile);
    }

    SECTION("alphabets"){
        char    tmpstr[]= "ABCDEFGHIJKLMN";
        FILE*   mockfile= new_mockfile(tmpstr);

        yyin = mockfile;
        REQUIRE(yylex() == IDtok);

        finish_yyin_to_eof();
        del_mockfile(mockfile);
    }

    SECTION("special charaters"){
        char    tmpstr[]= "hypen-id +id";
        FILE*   mockfile= new_mockfile(tmpstr);

        yyin = mockfile;
        REQUIRE(yylex() == IDtok);
        REQUIRE(yylex() == IDtok);

        finish_yyin_to_eof();
        del_mockfile(mockfile);
    }

    SECTION("integers and IDtok differnce"){
        char    tmpstr[]= "134234 243-sde";
        FILE*   mockfile= new_mockfile(tmpstr);

        yyin = mockfile;
        REQUIRE(yylex() == INTtok);
        REQUIRE(yylex() == IDtok);

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
        REQUIRE(yylex() == INTtok);
        REQUIRE(yylex() == INTtok);
        REQUIRE(yylex() == ')');

        finish_yyin_to_eof();
        del_mockfile(mockfile);
    }
    SECTION("bool"){
        char    tmpstr[]= "#t #f";
        FILE*   mockfile= new_mockfile(tmpstr);

        yyin = mockfile;
        REQUIRE(yylex() == BOOLtok);
        REQUIRE(yylex() == BOOLtok);

        finish_yyin_to_eof();
        del_mockfile(mockfile);
    }

    SECTION("if"){
        char    tmpstr[]= "(if (= 1 1) 1 2)";
        FILE*   mockfile= new_mockfile(tmpstr);

        yyin = mockfile;
        REQUIRE(yylex() == '(');
        REQUIRE(yylex() == IFtok);
        REQUIRE(yylex() == '(');
        REQUIRE(yylex() == IDtok);
        REQUIRE(yylex() == INTtok);
        REQUIRE(yylex() == INTtok);

        finish_yyin_to_eof();
        del_mockfile(mockfile);
    }

    SECTION("float"){
        char    tmpstr[]= "(+ 12.49 -0.1 +0.3 1. .45)";
        FILE*   mockfile= new_mockfile(tmpstr);

        yyin = mockfile;
        CHECK(yylex() == '(');
        CHECK(yylex() == IDtok);
        CHECK(yylex() == FLOATtok);
        CHECK(yylex() == FLOATtok);
        CHECK(yylex() == FLOATtok);
        CHECK(yylex() == FLOATtok);
        CHECK(yylex() == FLOATtok);
        CHECK(yylex() == ')');

        finish_yyin_to_eof();
        del_mockfile(mockfile);
    }
} 
