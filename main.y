%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "ast.h"
#include "global.h"
int yylex(void);
void yyerror(char*);
%}

%union {
    ASTNode *node;
    ArgList *argList;
    Arg *arg;
    int val;
    char* str;
}

%type<node> query constatom atom sentence expression term item element
%type<argList> arglist constarglist
%type<val> nq ns NUMBER
%type<str> CONSTANT VARIABLE '(' ')'
%type<arg> arg

%token NUMBER CONSTANT VARIABLE ENDL

%%
program:
       nq ENDL queries ENDL ns ENDL sentences ENDL
       ;

nq:
       NUMBER { globalNumQueries = $1 }
       ;
ns:
       NUMBER { globalNumSentences = $1 }
       ;

queries:
       query { queries.push_back($1); }
       | queries ENDL query { queries.push_back($3); }
       ;
query: constatom {$$ = $1;}
       | '~' constatom {$$ = new ASTNode(); $$->op = NEG; $$->left = $2;}
       ;
sentences:
	 sentence { sentences.push_back($1); }
         | sentences ENDL sentence { sentences.push_back($3); }
         ;
sentence:
	expression { $$ = $1; }
        ;
expression:
	  expression '=' '>' term { $$ = new ASTNode(); $$->op = IPL; $$->left = $1; $$->right = $4; }
          | term { $$ = $1; }
          ;
term:
    term '&' item { $$ = new ASTNode(); $$->op = AND; $$->left = $1; $$->right = $3; }
    | term '|' item { $$ = new ASTNode(); $$->op = OR; $$->left = $1; $$->right = $3; }
    | item { $$ = $1; }
    ;
item:
    element { $$ = $1; }
    | '~' element {$$ = new ASTNode(); $$->op = NEG; $$->left = $2;}
    ;
element:
       atom { $$ = $1; }
       | '(' expression ')' { $$ = $2; }
       ;
atom:
    CONSTANT '(' arglist ')' { $$ = new ASTNode(); $$->p = new Predicate(std::string($1).substr(0, strlen($1) - strlen($2))); $$->p->argList = $3; }
    ;
constatom:
	CONSTANT '(' constarglist ')' { $$ = new ASTNode(); $$->p = new Predicate(std::string($1).substr(0, strlen($1) - strlen($2))); $$->p->argList = $3; }
        ;
arglist:
       arg { $$ = new ArgList(); $$->list.push_back($1); }
       | arglist ',' arg  {$$ = $1; $$->list.push_back($3);}
       ;
constarglist:
	CONSTANT {$$ = new ArgList(); $$->list.push_back(new Arg(CON, $1));}
	| constarglist ',' CONSTANT {$$ = $1; $$->list.push_back(new Arg(CON, $3));}
	;
arg:
   CONSTANT { $$ = new Arg(CON, $1); }
   | VARIABLE { $$ = new Arg(VAR, $1); }
   ;
%%


void yyerror(char *s) {
    printf("Error: %s\n", s);
    exit(-1);
}
