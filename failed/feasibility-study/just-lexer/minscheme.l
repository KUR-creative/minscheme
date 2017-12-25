%{
    #include <stdlib.h>
    #include "minscheme.tab.h"
    void yyerror(char *);
%}

%%
    
[_a-zA-Z]*  {   /* identifier */
                //printf(yytext);
                //printf("->ID: %d \n", IDENTIFIER);
                return IDENTIFIER;
            }

    
[0-9]+      {   /* integers */
                //printf("->INT: %d \n", INTEGER);
                return INTEGER;
            }

\(          {   return OPENPAREN;   }
\)          {   return CLOSEPAREN;  }

    
[-+()=/*\n] {   return *yytext; /* operators */}

    
[ \t]       ;   /* skip whitespace */

                /* anything else is an error */
.               yyerror("invalid character");

%%


void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
}

int yywrap(void) {
    return 1; // 왜 eof에서 1이 반환이 안 되지? 이건 그럼 뭔 의미..
}
