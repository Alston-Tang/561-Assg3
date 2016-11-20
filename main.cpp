#include <stdio.h>
#include <iostream>
#include <vector>
#include "ast.h"
int yyparse(void);

ASTNode *globalASTRoot = nullptr;
int globalNumQueries;
int globalNumSentences;
std::vector<ASTNode*> queries;
std::vector<ASTNode*> sentences;

void eliminateImply(ASTNode *node) {
    if (node->op == IPL) {
        ASTNode *newNode = new ASTNode();
        newNode->op = NEG;
        newNode->left = node->left;

        node->op = OR;
        node->left = newNode;
    }
    if (node->left) eliminateImply(node->left);
    if (node->right) eliminateImply(node->right);
}

void propagateNeg(ASTNode *node) {
    if (node->op == NEG) {
        if (node->left->op) {
            ASTNode *del = node->left;
            *node = *del;
            delete del;

            switch (node->op) {
                ASTNode *negR, *negL;
                case NEG:
                    del = node->left;
                    *node = *del;
                    delete del;
                    break;
                case AND:
                    negL = new ASTNode();
                    negL->op = NEG;
                    negL->left = node->left;
                    negR = new ASTNode();
                    negR->op = NEG;
                    negR->left = node->right;

                    node->op = OR;
                    node->left = negL;
                    node->right = negR;
                    break;
                case OR:
                    negL = new ASTNode();
                    negL->op = NEG;
                    negL->left = node->left;
                    negR = new ASTNode();
                    negR->op = NEG;
                    negR->left = node->right;

                    node->op = AND;
                    node->left = negL;
                    node->right = negR;
                    break;
                default:
                    std::cerr << "Error" << std::endl;
                    exit(-1);
            }
        }
    }
    if (node->left) propagateNeg(node->left);
    if (node->right) propagateNeg(node->right);
}

bool distributeOr(ASTNode *node) {
    bool changed = false;
    if (node->op == OR) {
        if (node->left->op == AND || node->right->op == AND) {
            changed = true;
            node->op = AND;
            if (node->left->op == AND) {
                node->left->op = OR;
                ASTNode *distRight = node->left->right;
                node->left->right = node->right->deepCopy();
                ASTNode *nOr = new ASTNode();
                nOr->left = distRight;
                nOr->right = node->right;
                nOr->op = OR;
                node->right = nOr;
            }
            else if(node->right->op == AND) {
                node->right->op = OR;
                ASTNode *distLeft = node->right->left;
                node->right->left = node->left->deepCopy();
                ASTNode *nOr = new ASTNode();
                nOr->left = node->left;
                nOr->right = distLeft;
                nOr->op = OR;
                node->left = nOr;
            }
        }
    }
    if (node->left) changed |= distributeOr(node->left);
    if (node->right) changed |= distributeOr(node->right);
    return changed;
}

void convertCnf() {
    for (ASTNode *root : sentences) {
        eliminateImply(root);
        propagateNeg(root);
        while (distributeOr(root));
    }
}


int main() {
    extern FILE* yyin;
    yyin = fopen("input.txt", "r");
    yyparse();
    // Convert To CNF Form
    convertCnf();
    return 0;
}
