#include <iostream>
#include <string.h>
#include "syntax-tree.h"

void* new_syntax_tree(Node* node)
{
    auto* expr = new Expr;
    expr->name = node->str;
    expr->type = (ExprType)node->type;
    expr->value = node->value;
    return expr;
}
