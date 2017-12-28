%token INTtok IDtok DISP DEFtok BOOLtok IFtok FLOATtok

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

%type<int_t>    INTtok IDtok DISP DEFtok BOOLtok IFtok FLOATtok
%type<node_t>   expr pair

%%

prog: 
        pair            { syntax_tree = $1; }
    ;

expr:
        '(' pair ')'    { $$ = $2; }
    |   INTtok          { long long val = strtoll(yytext, NULL, 10);
                          $$ = atom(yytext, INT, val, 0); }
    |   IDtok           { $$ = atom(yytext, GENERIC, UNKNOWN_VAL, 0); }
    |   DEFtok          { $$ = atom("define", SPECIAL, UNKNOWN_VAL, 0); }
    |   BOOLtok         { long long val = ((yytext[1] == 't') ? 1 : 0);
                          $$ = atom(yytext, BOOL, val, 0); }
    |   IFtok           { $$ = atom("if", SPECIAL, UNKNOWN_VAL, 0); }
    ;

pair:
        expr            { $$ = pair("end-pair", GENERIC, UNKNOWN_VAL, NULL, 
                                    $1, NULL);// NO_NAME 
                        }
    |   expr pair       { $$ = pair("pair", GENERIC, UNKNOWN_VAL, NULL,
                                    $1, $2); // NO_NAME
                        }
    ;

%%

