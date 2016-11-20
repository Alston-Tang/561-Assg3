//
// Created by tang on 11/18/16.
//

#include "ast.h"
#include <string>

Arg::Arg(int type, char *name) {
    this->type = type;
    this->name = std::string(name);
}

ASTNode::ASTNode() {
    op = 0;
    p = nullptr;
    left = nullptr;
    right = nullptr;
}

ASTNode* ASTNode::deepCopy() {
    ASTNode *rv = new ASTNode();
    *rv = *this;
    if (rv->left) rv->left = rv->left->deepCopy();
    if (rv->right) rv->right = rv->right->deepCopy();
    return rv;
}

Predicate::Predicate(std::string name) {
    this->name = name;
}