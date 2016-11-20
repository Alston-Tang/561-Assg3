//
// Created by tang on 11/18/16.
//

#ifndef INC_561ASSG3_GLOBAL_H
#define INC_561ASSG3_GLOBAL_H

#include "ast.h"
#include <vector>

extern ASTNode *globalASTRoot;
extern int globalNumQueries;
extern int globalNumSentences;
extern std::vector<ASTNode*> queries;
extern std::vector<ASTNode*> sentences;

#endif //INC_561ASSG3_GLOBAL_H
