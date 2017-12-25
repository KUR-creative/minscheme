%{
    #include <stdlib.h>
    #include "simp-tree.h"
    #include "cal.tab.h"
    void yyerror(char *);
%}

%%

"square"    {
                return FUNCTION;
            }

    
[a-z]       {   /* variables */
                yylval.int_t = *yytext - 'a';
                return VARIABLE;
            }

    
[0-9]+      {   /* integers */
                yylval.int_t = atoi(yytext);
                return INTEGER;
            }

    
[-+()=/*\n] { return *yytext; /* operators */}

    
[ \t]       ;   /* skip whitespace */

                /* anything else is an error */
.               yyerror("invalid character");

%%

int yywrap(void) {
    return 1;
}
