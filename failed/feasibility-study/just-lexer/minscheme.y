%token INTEGER IDENTIFIER OPENPAREN CLOSEPAREN

%{
    #include <stdio.h>
    #include <stdlib.h>
    void yyerror(char *);
    int yylex(void);
%}

%%

prog: '+' {exit(1);}

%%

