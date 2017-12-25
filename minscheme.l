%{
    #include <stdlib.h>
    #include "minscheme.tab.h"
    void yyerror(char *);
%}

%%
    
"disp"                  { return DISP; }
"define"                { return DEFINE; }
("#t"|"#f")             { return BOOL; }
    
[-+]?[0-9]+             { return INTEGER; }
[_a-zA-Z0-9?!+-/*=]+    { return IDENTIFIER; }

[()]                    { return *yytext; }
                         
[ \t\n]                 ; /* skip whitespace */

                        /* anything else is an error */
.                       yyerror("invalid character");

%%


void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
}

/*
%union
{
    char*   str_t;
    int     int_t;
}
%type<int_t>    INTEGER IDENTIFIER DISP DEFINE
[-+()=/*] {   return *yytext; }
*/
int yywrap(void) {
    return 1; // 왜 eof에서 1이 반환이 안 되지? 이건 그럼 뭔 의미..
}
