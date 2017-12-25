#ifndef INTERPRETER_H
#define INTERPRETER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "simp-tree.h"

typedef enum Inherit {
    TOP_LIST,
    NORMAL_NODE,
    FUNC_CALL,
    ERROR_STATE,
    DEFINING,
} Inherit;

Type interpret(Node* parse_tree, Inherit state, PrevEdges prevs);
extern Node* syntax_tree;

#ifdef __cplusplus
}
#endif

#endif //INTERPRETER_H
