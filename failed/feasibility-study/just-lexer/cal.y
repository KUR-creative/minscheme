%token INTEGER VARIABLE FUNCTION
%left '+' '-'
%left '*' '/'

%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "simp-tree.h"
    void yyerror(char *);
    int yylex(void);
    int sym[26];
    int square(int x){ return x*x; }

    char buffer[32];
    char* int2str(int n){
        sprintf(buffer, "%d", n);
        return buffer;
    }
%}

%union
{
    Node*   node;
    int     int_t;
}

%type<node>     stmt stmt_list expr operator assign_op variable function 
%type<int_t>    INTEGER VARIABLE


%start program

%%

program:
        stmt_list '\n''\n'          { pretty_print($1,0); exit(0); }
        ;

stmt:
          expr                      { $$ = new_node1($1, "expr"); }
        | variable assign_op expr   { $$ = new_node3($1,$2,$3, "stmt"); }
        ;

stmt_list:
          stmt_list stmt '\n'       { $$ = new_node2($1,$2, "stmt_list"); }
        | stmt '\n'                 { $$ = new_node1($1, "stmt"); }
        ;

expr:
          function '(' expr ')'     { $$ = new_node4($1,NULL,$3,NULL, "expr"); }
        | INTEGER                   { $$ = new_node1(NULL, int2str($1)); }
        | variable                  { $$ = new_node1($1, "expr"); }
        | expr operator expr        { $$ = new_node3($1,$2,$3, "expr"); }
        | '(' expr ')'              { $$ = new_node1($2, "expr"); }
        ;

variable: VARIABLE  { char id = $1 + 'a';
                      char name[1]; name[0] = id;
                      $$ = new_node1(NULL, name); }
        ;

operator: '+'       { $$ = new_node1(NULL,"+"); } 
        | '-'       { $$ = new_node1(NULL,"-"); }
        | '*'       { $$ = new_node1(NULL,"*"); }
        | '/'       { $$ = new_node1(NULL,"/"); }
        ;

assign_op:'='       { $$ = new_node1(NULL,"="); }
        ;

function: FUNCTION  { $$ = new_node1(NULL, "function"); }


%%

void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
}

int main(void) {
    yyparse();
    return 0;
}
