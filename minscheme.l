%{
    #include <stdlib.h>
    #include "minscheme.tab.h"
    void yyerror(char *);
%}

%%
    
"disp"                  { return DISP; }
"define"                { return DEFtok; }
("#t"|"#f")             { return BOOLtok; }
"if"                    { return IFtok; }
    
\.                      yyerror("invalid character"); // float error case.
[-+]?[0-9]*\.[0-9]*     { return FLOATtok; }
[-+]?[0-9]+             { return INTtok; }
[_a-zA-Z0-9?!+-/*=]+    { return IDtok; }

[()]                    { return *yytext; }
                         
[ \t\n]                 ; /* skip whitespace */

                        /* anything else is an error */
.                       yyerror("invalid character");

%%


void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
}

int yywrap(void) {
    return 1; 
}
