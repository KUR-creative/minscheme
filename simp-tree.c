#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "simp-tree.h"
#include "syscalls.h"

const Value UNKNOWN_VAL = -1000000000; 
const void* NO_NAME = NULL;

Node*   new_node1(Node* node1, char* str1)
{
    Node*   ret = Calloc(1,sizeof(Node));
    ret->child[0] = node1;
    ret->name = strdup(str1);
    return ret;
}
Node*   new_node2(Node* node1, Node* node2, char* str1)
{
    Node*   ret = Calloc(1,sizeof(Node));
    ret->child[0] = node1;
    ret->child[1] = node2;
    ret->name = strdup(str1);
    return ret;
}

/*
Node*   expr_node(Node* node1, char* str1, Type type)
{
    Node*   ret = Calloc(1,sizeof(Node));
    ret->child[0] = node1;
    ret->name = strdup(str1);
    ret->type = type;
    return ret;
}
Node*   expr_list_node(Node* node1, Node* node2, char* str1, Type type)
{
    Node*   ret = Calloc(1,sizeof(Node));
    ret->child[0] = node1;
    ret->child[1] = node2;
    ret->name = strdup(str1);
    ret->type = type;
    return ret;
}
Node*   new_node3(Node* node1, Node* node2, Node* node3, 
                  char* str1)
{
    Node*   ret = Calloc(1,sizeof(Node));
    ret->child[0] = node1;
    ret->child[1] = node2;
    ret->child[2] = node3;
    ret->name = strdup(str1);
    return ret;
}
Node*   new_node4(Node* node1, Node* node2, Node* node3, Node* node4,
                  char* str1)
{
    Node*   ret = Calloc(1,sizeof(Node));
    ret->child[0] = node1;
    ret->child[1] = node2;
    ret->child[2] = node3;
    ret->child[3] = node4;
    ret->name = strdup(str1);
    return ret;
}
*/

// INT has no name = NULL
// GENERIC has name != NULL
Node*   atom(char* name, Type type, Value value, Value auxval)
{
    Node*   ret = Calloc(1,sizeof(Node));
    if(name != NO_NAME) {
        ret->name = strdup(name);
    }
    ret->type = type;
    ret->value = value;
    ret->auxval = auxval;
    return ret;
}

Node*   pair(char* name, Type type, Value value, Value auxval, 
             Node* car, Node* cdr)
{
    Node*   ret = Calloc(1,sizeof(Node));
    ret->child[0] = car;
    ret->child[1] = cdr;
    if(name != NO_NAME) {
        ret->name = strdup(name);
    }
    ret->type = type;
    ret->value = value;
    ret->auxval = auxval;
    return ret;
}

void    pretty_print(Node* tree, int depth)
{
    for(int i = 0; i < depth; i++){
        printf("\t");
    }
    //printf("%s|%d|%lld \n", tree->name, tree->type, tree->value);
    //printf("%c%c|%d| \n", tree->name[0], tree->name[1], tree->type);
    printf("%s|%d| \n", tree->name, tree->type);
    //printf("%s \n", tree->name);
    for(int i = 0; i < NUM_CHILD; i++){
        Node* child = tree->child[i];
        if(child != NULL){
            pretty_print(child, depth + 1);
        }
    }
}

static
void    del_node(Node** pnode)
{
    Node* node = *pnode;
    for(int i = 0; i < NUM_CHILD; i++){
        node->child[i] = NULL;
        if(node->name != NULL){
            free(node->name);
            node->name = NULL;
        }
    }

    free(node);
}

// it set proot NULL.
void    del_tree(Node** proot)
{
    Node* root = *proot;
    for(int i = 0; i < NUM_CHILD; i++){
        Node* child = root->child[i];
        if(child != NULL){
            del_tree(&child);
        }
    }
    del_node(&root);
    *proot = NULL;
}

int     list_len(Node* list)
{
    int len = 0;
    while(list != NULL){
        len++;
        list = cdr(list); 
    }
    return len;
}

State   push_car(State edges)
{
    switch(edges){
    case RR: return RL;
    case RL: return LL;
    case LL: return LL;
    case LR: return RL;
    }
}

State   push_cdr(State edges)
{
    switch(edges){
    case RR: return RR;
    case RL: return LR;
    case LL: return LR;
    case LR: return RR;
    }
}

Node*   copy_tree(Node* tree)
{   
    Node* ret_tree = NULL;
    if(tree != NULL){
        ret_tree = new_node2(NULL, NULL, tree->name);
        ret_tree->auxval = tree->auxval;
        ret_tree->type = tree->type;
        ret_tree->value = tree->value;
        //ret_tree->argnum = tree->argnum;
        car(ret_tree) = copy_tree(car(tree));
        cdr(ret_tree) = copy_tree(cdr(tree));
    }
    return ret_tree;
}

