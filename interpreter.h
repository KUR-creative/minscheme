#ifndef INTERPRETER_H
#define INTERPRETER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "simp-tree.h"

extern Node* syntax_tree;
State interpret(Node* parse_tree, State state);
void clear_stack(void);

#ifdef __cplusplus
}
#endif

#endif //INTERPRETER_H
