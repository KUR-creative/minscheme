#ifndef SIMP_TREE_H
#define SIMP_TREE_H

#include <stdlib.h>

#define NUM_CHILD   2
// unsafe!
#define car(p) ((p)->child[0])  // list[0]
#define cadr(p) ((p)->child[1]->child[0]) // list[1]
#define caddr(p) ((p)->child[1]->child[1]->child[0]) // list[2]
#define cadddr(p) ((p)->child[1]->child[1]->child[1]->child[0]) // list[3]
#define cdr(p) ((p)->child[1])  // unsafe!
#define caadr(p) ((p)->child[1]->child[0]->child[0]) 

typedef long long Value;
extern const Value UNKNOWN_VAL; // UNKNOWN_VALUE is conflict with yacc! 
extern const void* NO_NAME;

typedef enum PrevEdges { // L: car, R: cdr.
    RR = 0, 
    RL, 
    LL,
    LR,
} PrevEdges;

typedef enum Hint { // it is NOT the type of data!
    NO_CHILD,   //0
    DEFINE_ATOM,
    IF_ATOM,
    INT_ATOM,
    ID_ATOM,
    BOOL_ATOM,
    END_PAIR, // pair: [car,nil]
    EXPR_PAIR,// pair: [car,cdr]
} Hint;

typedef enum Type{
    INT = 0,
    GENERIC,
    EXPR,
    FUNC, // only one APPLICABLE!
    EXPR_LIST,
    DISP_NODE,
    DEFINE_TYPE,
    IF_TYPE,
    BOOL_TYPE,
    NULL_NODE,
} Type;

typedef struct Node {
    char*           name;
    Hint            hint;
    Type            type;
    Value           value;
    int             argnum; // 최적화..
    struct Node*    child[NUM_CHILD];
} Node;

Node*   new_node1(Node* node1, char* str1);
Node*   new_node2(Node* node1, Node* node2, char* str1);
//Node*   new_node3(Node* node1, Node* node2, Node* node3, char* str1);
//Node*   new_node4(Node* node1, Node* node2, Node* node3, Node* node4, char* str1);

Node*   atom(char* name, Type type, Value value, Hint hint);
Node*   pair(char* name, Type type, Value value, Hint hint, 
             Node* car, Node* cdr);

void    pretty_print(Node* tree, int depth);
void    del_tree(Node** root);

int     list_len(Node* list);

PrevEdges push_car(PrevEdges edges);
PrevEdges push_cdr(PrevEdges edges);

Node*   copy_tree(Node* tree);
#endif

