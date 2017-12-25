#include <iostream>
extern "C"
{
#include "minscheme.tab.h"

extern FILE *yyin;
extern FILE *yyout;
extern void yyerror(char *);
extern int yylex(void);
}

// |    LPAREN INTEGER RPAREN   {;}
int main(void){
    //yyout = fopen("tmpres", "w");
    yyparse();
    return 0;
}
