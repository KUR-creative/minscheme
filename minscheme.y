%token INTtok IDtok DISP DEFtok BOOLtok IFtok FLOATtok LAMBDAtok

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

%type<int_t>    INTtok IDtok DISP DEFtok BOOLtok IFtok FLOATtok LAMBDAtok
%type<node_t>   expr pair

%%

prog: 
        pair            { syntax_tree = $1; }
    ;

expr:
        '(' pair ')'    { $$ = $2; }
    |   '(' ')'         { $$ = atom("nil", PAIR, UNKNOWN_VAL); }
    |   INTtok          { long long val = strtoll(yytext, NULL, 10);
                          $$ = atom(yytext, INT, val); }
    |   FLOATtok        { long long val;
                          double double_val = atof(yytext);
                          //val = *(long long*)(&double_val);// no cast!
                          val = double_to_Value(double_val);// no cast!
                          $$ = atom(yytext, INT, val); }
    |   IDtok           { $$ = atom(yytext, GENERIC, UNKNOWN_VAL); }
    |   BOOLtok         { long long val = ((yytext[1] == 't') ? 1 : 0);
                          $$ = atom(yytext, BOOL, val); }
    |   DEFtok          { $$ = atom("define", SPECIAL, UNKNOWN_VAL); }
    |   IFtok           { $$ = atom("if", SPECIAL, UNKNOWN_VAL); }
    |   LAMBDAtok       { $$ = atom("lambda", SPECIAL, UNKNOWN_VAL); }
    ;

pair:
        expr            { $$ = pair("end-pair", PAIR, UNKNOWN_VAL, 
                                    $1, NULL);// NO_NAME 
                        }
    |   expr pair       { $$ = pair("pair", PAIR, UNKNOWN_VAL,
                                    $1, $2); // NO_NAME
                        }
    ;

%%

