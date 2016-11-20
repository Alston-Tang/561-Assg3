//
// Created by tang on 11/18/16.
//

#ifndef INC_561ASSG3_AST_H
#define INC_561ASSG3_AST_H

#include <vector>
#include <string>

#define NEG 1
#define IPL 2
#define AND 3
#define OR 4

#define VAR 11
#define CON 12

struct Arg{
    int type;
    std::string name;
    Arg(int type, char* name);
};

struct ArgList{
    std::vector<Arg*> list;
};

struct Predicate{
    std::string name;
    ArgList *argList;
    Predicate(std::string name);
};

struct ASTNode{
    int op;
    ASTNode *left, *right;
    Predicate *p;
    ASTNode();
    ASTNode* deepCopy();
};

#endif //INC_561ASSG3_AST_H