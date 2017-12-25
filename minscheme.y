%token INTEGER IDENTIFIER DISP DEFINE BOOL IF

%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "errors.h"
    #include "symtab.h"
    #include "simp-tree.h"
    #include "syntax-tree.h"
    #include "interpreter.h"
    extern FILE* yyout;
    extern char* yytext;
    void yyerror(char *);
    int yylex(void);

    static char tmpstrbuf[32] = {0,};
    static char* ll2str(long long n){
        sprintf(tmpstrbuf, "%lld", n);
        return tmpstrbuf;
    }
    static int error_flag = NO_ERR;

%}

%union
{
    struct Node*    node_t;
    int             int_t;
}

%type<int_t>    INTEGER IDENTIFIER DISP DEFINE BOOL IF
%type<node_t>   expr pair

%%

prog: 
        pair            { syntax_tree = $1; }
    ;

expr:
        '(' pair ')'    { $$ = $2; }
    |   INTEGER         { long long val = strtoll(yytext, NULL, 10);
                          $$ = atom(yytext, INT, val, INT_ATOM); }
    |   IDENTIFIER      { $$ = atom(yytext, GENERIC, UNKNOWN_VAL, ID_ATOM); }
    |   DISP            { $$ = atom("disp", FUNC, UNKNOWN_VAL, ID_ATOM); }
    |   DEFINE          { $$ = atom("define", DEFINE_TYPE, UNKNOWN_VAL, DEFINE_ATOM); }
    |   BOOL            { long long val = ((yytext[1] == 't') ? 1 : 0);
                          $$ = atom(yytext, BOOL_TYPE, val, BOOL_ATOM); }
    |   IF              { $$ = atom("if", IF_TYPE, UNKNOWN_VAL, IF_ATOM); }
    ;

pair:
        expr            { $$ = pair("end-pair", GENERIC, UNKNOWN_VAL, END_PAIR, 
                                    $1, NULL);// NO_NAME 
                        }
    |   expr pair       { $$ = pair("pair", GENERIC, UNKNOWN_VAL, EXPR_PAIR,
                                    $1, $2); // NO_NAME
                        }
    ;

%%

