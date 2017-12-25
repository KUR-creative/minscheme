#include <iostream>
extern "C"
{
#include "minscheme.tab.h"
#include "syscalls.h"
#include "simp-tree.h"
#include "interpreter.h" 

extern FILE *yyin;
extern FILE *yyout;
extern void yyerror(char *);
extern int yylex(void);
}

int main(int argc, char* argv[]){
    yyin = Fopen(argv[1], "r");
    if(argc == 3){
        yyout = Fopen(argv[2], "w");
    }
    yyparse();
    interpret(syntax_tree, TOP_LIST, RR);
    return 0;
}

