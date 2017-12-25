#ifndef SYNTAX_TREE_H
#define SYNTAX_TREE_H


#ifdef __cplusplus

#include <vector>
#include <string>
using std::vector;
using std::string;

typedef long long Value;
enum ExprType { // it is NOT the type of data!
    ROOT_EXPR = -1,
    FUNC_EXPR = 0,
    DEFINE_EXPR,
    IF_EXPR,
    AND_EXPR,
    OR_EXPR,
    NOT_EXPR,
};

struct Expr {
    string          name;
    ExprType        type;
    Value           value;
    vector<Expr>    child;
};
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#include "simp-tree.h"

void* new_syntax_tree(Node* node);

#ifdef __cplusplus
}
#endif

#endif //SYNTAX_TREE_H
