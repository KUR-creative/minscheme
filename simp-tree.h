#ifndef SIMP_TREE_H
#define SIMP_TREE_H

#include <stdlib.h>

#define NUM_CHILD   2
// unsafe!
#define car(p)      ( ((Node*)p)->child[0])  // list[0]
#define cadr(p)     ( ((Node*)p)->child[1]->child[0]) // list[1]
#define caddr(p)    ( ((Node*)p)->child[1]->child[1]->child[0]) // list[2]
#define cadddr(p)   ( ((Node*)p)->child[1]->child[1]->child[1]->child[0]) // list[3]
#define cdr(p)      ( ((Node*)p)->child[1])  // unsafe!
#define caadr(p)    ( ((Node*)p)->child[1]->child[0]->child[0]) 


#define double_to_Value(double_lval) *(long long*)(&(double_lval))
#define Value_to_double(lvalue)      *(double*)(&(lvalue))

typedef long long Value;
extern const Value UNKNOWN_VAL; // UNKNOWN_VALUE is conflict with yacc! 
extern const void* NO_NAME;

typedef enum State { // L: car, R: cdr.
    RR = 0, 
    RL, 
    LL,
    LR,
    LAMBDAstate,
    DEFstate,
    IFstate,
    NOstate, // for initialization.
} State;

typedef enum Type{
    INT = 0,
    FLOAT,
    FUNC,   // APPLICABLE 1!
    BOOL,
    PAIR,
    NUMBER,
    GENERIC,
    SPECIAL,// APPLICABLE 2!
    NO_TYPE,
} Type;

extern const Value UNKNOWN_VAL;

typedef struct Node {
    char*           name;
    Type            type;
    Value           value;
    //int             argnum; // 최적화..
    struct Node*    child[NUM_CHILD];
} Node;

Node*   new_node1(Node* node1, char* str1);
Node*   new_node2(Node* node1, Node* node2, char* str1);
//Node*   new_node3(Node* node1, Node* node2, Node* node3, char* str1);
//Node*   new_node4(Node* node1, Node* node2, Node* node3, Node* node4, char* str1);

Node*   atom(char* name, Type type, Value value);
Node*   pair(char* name, Type type, Value value, 
             Node* car, Node* cdr);

void    pretty_print(Node* tree, int depth);
void    del_tree(Node** root);

int     list_len(Node* list);

State   push_car(State edges);
State   push_cdr(State edges);

Node*   copy_tree(Node* tree);
#endif

