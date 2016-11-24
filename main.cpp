#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <string>
#include <ctime>
#include "ast.h"

int yyparse(void);
extern FILE* yyin;


namespace KB {
    class Arg {
    public:
        std::string name;
        virtual bool isConstant() {
            return false;
        }
        virtual bool isVariable() {
            return false;
        }
        Arg(std::string &_name) {
            name = _name;
        }
    };
    class Constant : public Arg {
    public:
        Constant(std::string _name) : Arg(_name) {}
        virtual bool isConstant() {
            return true;
        }
    };
    class Variable : public Arg {
    public:
        Variable(std::string _name) : Arg(_name) {

        }
        virtual bool isVariable() {
            return true;
        }
    };
    class Predicate {
    public:
        std::string name;
        bool pos;
        Predicate *opp;
        unsigned long numArgs;
    public:
        Predicate(std::string &_name, bool _pos, unsigned long _numArgs) {
            name = _name;
            pos = _pos;
            numArgs = _numArgs;
        }
    };
    class PredicateSet {
        std::unordered_map<std::string, std::pair<Predicate*, Predicate*>> con;
    public:
        Predicate* getCreatePredicate(std::string name, bool pos, unsigned long numArgs) {
            if (!con.count(name)) {
                std::pair<Predicate *, Predicate *> pr(nullptr, nullptr);
                Predicate *posP = new Predicate(name, true, numArgs);
                Predicate *negP = new Predicate(name, false, numArgs);
                posP->opp = negP;
                negP->opp = posP;
                pr.first = posP;
                pr.second = negP;
                con[name] = pr;
            }
            if (pos) return con[name].first;
            else return con[name].second;
        }
    };
    struct PredicateInst {
        Predicate *p;
        std::vector<Arg*> args;
        PredicateInst(Predicate *_p) {
            p = _p;
        }
        void insertArg(Arg *arg) {
            args.push_back(arg);
        }
        bool unify(PredicateInst *p2, std::unordered_map<Arg*, Arg*> &argMap) {
            unsigned long len = args.size();
            if (len != p2->args.size()) return false;
            for (int i = 0; i < args.size(); i++) {
                Arg *arg1 = args[i];
                Arg *arg2 = p2->args[i];
                if (arg1->isVariable()) {
                    Arg *arg1Mapped = argMap.count(arg1) ? argMap[arg1] : nullptr;
                    if (arg2->isVariable()) {
                        Arg *arg2Mapped = argMap.count(arg2) ? argMap[arg2] : nullptr;
                        if (arg1Mapped && arg2Mapped) {
                            if (arg1Mapped != arg2Mapped) return false;
                        }
                        else if(arg1Mapped) {
                            argMap[arg2] = arg1Mapped;
                        }
                        else if(arg2Mapped) {
                            argMap[arg1] = arg2Mapped;
                        }
                        else {
                            Variable *newVar = new Variable("");
                            argMap[arg1] = newVar;
                            argMap[arg2] = newVar;
                        }
                    }
                    else {
                        if (arg1Mapped) {
                            if (arg1Mapped->name != arg2->name) return false;
                        }
                        else {
                            argMap[arg1] = arg2;
                        }
                    }
                }
                else if(arg2->isVariable()) {
                    Arg *arg2Mapped = argMap.count(arg2) ? argMap[arg2] : nullptr;
                    if (arg2Mapped) {
                        if (arg2Mapped->name != arg1->name) return false;
                    }
                    else {
                        argMap[arg2] = arg1;
                    }
                }
                else {
                    if (arg1->name != arg2->name) return false;
                }
            }
            return true;
        }
    };
    struct PredicateLink {
        PredicateLink *prev;
        PredicateInst *con;
        PredicateLink *next;
        ~PredicateLink(){
            if (con) {
                delete con;
            }
        }
    };
    class Sentence {
    public:
        PredicateLink *head;
        PredicateLink *tail;
        std::unordered_map<std::string, Arg*> argSet;
        bool tautology;
        Sentence() {
            head = new PredicateLink();
            tail = new PredicateLink();
            head->prev = nullptr;
            head->con = nullptr;
            head->next = tail;
            tail->prev = head;
            tail->next = nullptr;
            tail->con = nullptr;
            tautology = false;
        }
        bool operator == (const Sentence &s2) const {
            std::unordered_map<Arg*, Arg*> argMap;
            PredicateLink *curS1 = head->next;
            PredicateLink *curS2 = s2.head->next;
            while (curS1 != tail && curS2 != s2.tail) {
                for (int i = 0; i < curS1->con->args.size(); i++) {
                    Arg* arg1 = curS1->con->args[i];
                    Arg* arg2 = curS2->con->args[i];
                    if (arg1->isConstant() && arg2->isVariable()) return false;
                    if (arg1->isVariable() && arg2->isConstant()) return false;
                    if (arg1->isConstant()) {
                        if (arg1->name != arg2->name) return false;
                    }
                    else {
                        if (argMap.count(arg1)) {
                            if (argMap[arg1] != arg2) return false;
                        }
                        else {
                            argMap[arg1] = arg2;
                        }
                    }
                }
                curS1 = curS1->next;
                curS2 = curS2->next;
            }
            return true;
        }
        void insertPredicate(Predicate *p, std::vector<Arg*> &args) {
            PredicateLink *cur = this->head;
            for (; cur->next != tail && cur->next->con->p->name < p->name; cur = cur->next);
            while (cur->next != tail && cur->next->con->p->name == p->name) {
                auto nextArg = cur->next->con->args;
                auto pArg = args;
                bool same = true;
                for (int i = 0; i < nextArg.size(); i++) {
                    if (nextArg[i]->isVariable() && pArg[i]->isVariable()) {
                        same = same && nextArg[i] == pArg[i];
                    }
                    else if(nextArg[i]->isConstant() && pArg[i]->isConstant()) {
                        same = same && nextArg[i]->name == pArg[i]->name;
                    }
                    else {
                        same = false;
                    }
                }
                if (same) {
                    if (cur->next->con->p->pos != p->pos) {
                        tautology = true;
                    }
                    else return;
                }
                cur = cur->next;
            }
            PredicateLink *lnk = new PredicateLink();
            //
            PredicateInst *pIns = new PredicateInst(p);
            for (auto arg : args) {
                pIns->insertArg(arg);
            }
            lnk->con = pIns;
            //
            lnk->prev = cur;
            lnk->next = cur->next;
            cur->next->prev = lnk;
            cur->next = lnk;
        }
        void insertPredicate(Predicate *p, ArgList *argList) {

            PredicateLink *cur = this->head;
            for (; cur->next != tail && cur->next->con->p->name < p->name; cur = cur->next);
            while (cur->next != tail && cur->next->con->p->name == p->name) {
                if (cur->next->con->p->pos == p->pos) {
                    auto nextArg = cur->next->con->args;
                    auto pArg = argList->list;
                    bool same = true;
                    for (int i = 0; i < nextArg.size(); i++) {
                        if (nextArg[i]->name != pArg[i]->name) {
                            same = false;
                            break;
                        }
                    }
                    if (same) {
                        return;
                    }
                }
                cur = cur->next;
            }
            PredicateLink *lnk = new PredicateLink();
            //
            PredicateInst *pIns = new PredicateInst(p);
            for (auto arg : argList->list) {
                if (!argSet.count(arg->name)) {
                    if (arg->type == VAR) {
                        argSet[arg->name] = new Variable(arg->name);
                    }
                    else {
                        argSet[arg->name] = new Constant(arg->name);
                    }
                }
                pIns->insertArg(argSet[arg->name]);
            }
            lnk->con = pIns;
            //
            lnk->prev = cur;
            lnk->next = cur->next;
            cur->next->prev = lnk;
            cur->next = lnk;
        }
        std::string format() {
            std::string rv;
            PredicateLink *cur = head->next;
            while (cur != tail) {
                Predicate *p = cur->con->p;
                if (!p->pos) rv += '~';
                rv += p->name;
                rv += '(';
                bool first = true;
                for (Arg* arg : cur->con->args) {
                    if (!first) rv += ", ";
                    first = false;
                    if (arg->isConstant()) rv += arg->name;
                    else rv += std::to_string((unsigned long long)arg);
                }
                rv += ')';
                if (cur->next != tail) rv += " | ";
                cur = cur->next;
            }
            return rv;
        }
    };
    struct PredicateTree {
        std::unordered_map<Predicate*, PredicateTree*> next;
        std::vector<Sentence*> term;
        static bool checkExist(PredicateTree *root, Sentence *s) {
            PredicateTree *cur = root;
            PredicateLink *lnk = s->head->next;
            while (lnk != s->tail) {
                if (!cur->next.count(lnk->con->p)) {
                    return false;
                }
                cur = cur->next[lnk->con->p];
                lnk = lnk->next;
            }
            for (Sentence *existS : cur->term) {
                if (*existS == *s) return true;
            }
            return false;
        }
        static void insertToTree(PredicateTree *root, Sentence *s) {
            PredicateTree *cur = root;
            PredicateLink *lnk = s->head->next;
            while (lnk != s->tail) {
                if (!cur->next.count(lnk->con->p)) {
                    cur->next[lnk->con->p] = new PredicateTree();
                }
                cur = cur->next[lnk->con->p];
                lnk = lnk->next;
            }
            cur->term.push_back(s);
        }

    };
    class KB {
    public:
        std::vector<Sentence*> sentences;
        PredicateSet *pSet;
        KB() {
            pSet = new PredicateSet();
        }
        static bool insertToRemainder(Sentence *s, std::list<Sentence*> &remainder,
                                      std::unordered_map<Predicate*, std::unordered_set<Sentence*>> &predicateCheck,
                                      PredicateTree *pTreeRoot)
        {
            if (s->tautology) return false;
            if (PredicateTree::checkExist(pTreeRoot, s)) {
                return false;
            }
            PredicateLink *cur = s->head->next;
            while (cur != s->tail) {
                predicateCheck[cur->con->p].insert(s);
                cur = cur->next;
            }
            remainder.push_back(s);
            PredicateTree::insertToTree(pTreeRoot, s);
            return true;
        }
        static Sentence* getRemoveFromRemainder(std::list<Sentence*> &remainder,
                                                std::unordered_map<Predicate*, std::unordered_set<Sentence*>> &predicateCheck,
                                                PredicateTree *pTreeRoot)
        {
            Sentence *rv = remainder.front();
            PredicateLink *cur = rv->head->next;
            while (cur != rv->tail) {
                predicateCheck[cur->con->p].erase(rv);
                cur = cur->next;
            }

            remainder.pop_front();
            return rv;
        }
        static Sentence* resolve(Sentence *s1, Sentence *s2, Predicate *predicateS1) {
            Sentence *rv = nullptr;
            PredicateLink *s1p, *s2p;
            for (s1p = s1->head->next; s1p != s1->tail && s1p->con->p != predicateS1; s1p = s1p->next);
            for (s2p = s2->head->next; s2p != s2->tail && s2p->con->p != predicateS1->opp; s2p = s2p->next);
            std::unordered_map<Arg*, Arg*> argMap;
            bool res = s1p->con->unify(s2p->con, argMap);
            if (!res) return rv;
            rv = new Sentence();
            PredicateLink *cur = s1->head->next;
            while (cur != s1->tail) {
                if (cur != s1p) {
                    std::vector<Arg*> newArgList;
                    for (Arg *arg : cur->con->args) {
                        if (arg->isConstant()) newArgList.push_back(arg);
                        else {
                            if (!argMap.count(arg)) {
                                argMap[arg] = new Variable("");
                            }
                            newArgList.push_back(argMap[arg]);
                        }
                    }
                    rv->insertPredicate(cur->con->p, newArgList);
                }
                cur = cur->next;
            }
            cur = s2->head->next;
            while (cur != s2->tail) {
                if (cur != s2p) {
                    std::vector<Arg*> newArgList;
                    for (Arg *arg : cur->con->args) {
                        if (arg->isConstant()) newArgList.push_back(arg);
                        else {
                            if (!argMap.count(arg)) {
                                argMap[arg] = new Variable("");
                            }
                            newArgList.push_back(argMap[arg]);
                        }
                    }
                    rv->insertPredicate(cur->con->p, newArgList);
                }
                cur = cur->next;
            }
            return rv;
        }
        bool query(Sentence &q) {
            std::clock_t start = std::clock();
            std::vector<Sentence*> toClean;
            PredicateTree *pTreeRoot = new PredicateTree();
            std::unordered_map<Predicate*, std::unordered_set<Sentence*>> predicateCheck;
            // Copy sentences' pointer to remainder list
            std::list<Sentence*> remainder;
            for (Sentence *s : sentences) {
                insertToRemainder(s, remainder, predicateCheck, pTreeRoot);
            }
            // Neg query sentence
            Predicate *p = q.head->next->con->p;
            q.head->next->con->p = pSet->getCreatePredicate(p->name, !p->pos, p->numArgs);
            // Push to remainder
            insertToRemainder(&q, remainder, predicateCheck, pTreeRoot);
            while (!remainder.empty()) {
                double duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
                if (duration > 20) return false;
                Sentence *resolveSen = getRemoveFromRemainder(remainder, predicateCheck, pTreeRoot);
                PredicateLink *cur = resolveSen->head->next;
                while (cur != resolveSen->tail) {
                    for (Sentence *resSen : predicateCheck[cur->con->p->opp]) {
                        //std::cout << "From" << std::endl << resolveSen->format() << std::endl << resSen->format() << std::endl;
                        Sentence *newSentence = resolve(resolveSen, resSen, cur->con->p);
                        if (newSentence) {
                            //std::cout << "New Sentence" << std::endl << newSentence->format() << std::endl;
                            toClean.push_back(newSentence);
                            if (newSentence->head->next == newSentence->tail) {
                                return true;
                            }
                            else {
                                insertToRemainder(newSentence, remainder, predicateCheck, pTreeRoot);
                            }
                        }
                    }
                    cur = cur->next;
                }
            }
            return false;
        }
        void insertSentence(Sentence *s) {
            sentences.push_back(s);
        }
    };
}

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

void convertToCnf() {
    for (ASTNode *root : sentences) {
        eliminateImply(root);
        propagateNeg(root);
        while (distributeOr(root));
    }
}

KB::Sentence* generateSentence(ASTNode *node, KB::Sentence *s, KB::KB &kb) {
    ASTNode *cur = node;
    bool pos = true;
    if (!node->op || node->op == NEG) {
        if (node->op == NEG) {
            pos = false;
            cur = node->left;
        }
        s->insertPredicate(kb.pSet->getCreatePredicate(cur->p->name, pos, cur->p->argList->list.size()), cur->p->argList);
        return s;
    }
    else if (node->op == OR) {
        generateSentence(node->left, s, kb);
        generateSentence(node->right, s, kb);
    }
    else {
        std::cerr << "Error" << std::endl;
        exit(-1);
    }
    return s;
}

void splitCNF(ASTNode *node, KB::KB &kb) {
    KB::Sentence *s;
    if (node->op == AND) {
        splitCNF(node->left, kb);
        splitCNF(node->right, kb);
    }
    else {
        kb.insertSentence(generateSentence(node, new KB::Sentence(), kb));
    }
}

void insertToKB(KB::KB &kb) {
    for (ASTNode *root :sentences) {
        splitCNF(root, kb);
    }
}

void convertQuery(std::vector<KB::Sentence*> &qs, KB::KB &kb) {
    for (ASTNode *root : queries) {
        ASTNode *cur = root;
        bool pos = true;
        if (cur->op == NEG) {
            cur = cur->left;
            pos = false;
        }
        KB::Sentence *s = new KB::Sentence();
        KB::Predicate *p = kb.pSet->getCreatePredicate(cur->p->name, pos, cur->p->argList->list.size());
        s->insertPredicate(p, cur->p->argList);
        qs.push_back(s);
    }
}


int main() {
    yyin = fopen("input.txt", "r");
    yyparse();
    // Convert To CNF Form
    convertToCnf();
    // Insert To KB
    KB::KB kb;
    insertToKB(kb);
    // Convert query
    std::vector<KB::Sentence*> qs;
    convertQuery(qs, kb);
    std::ofstream out("output.txt");
    for (auto query : qs) {
        bool res = kb.query(*query);
        if (res) out << "TRUE" << std::endl;
        else out << "FALSE" << std::endl;
    }
    return 0;
}
