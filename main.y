%{
#include <stdio.h>
#include <stdlib.h>
int yylex(void);
void yyerror(char*);
%}

%token NUMBER CONSTANT VARIABLE ENDL

%%
program:
       nq ENDL queries ENDL ns ENDL sentences ENDL {printf("Reduce Program\n");}
       ;

nq:
       NUMBER { printf("Nq -> Number\n"); }
       ;
ns:
       NUMBER
       ;

queries:
       query { printf("Queries -> Query\n"); }
       | queries ENDL query
       ;
query: constatom
       | '~' constatom
       ;
sentences:
	 sentence
         | sentences ENDL sentence
         ;
sentence:
	expression
        ;
expression:
	  expression '=' '>' term
          | term
          ;
term:
    term '&' item
    | term '|' item
    | item
    ;
item:
    element
    | '~' element
    ;
element:
       atom
       | '(' expression ')'
       ;
atom:
    CONSTANT '(' arglist ')'
    ;
constatom:
	CONSTANT '(' constarglist ')'
        ;
arglist:
       arg { printf("Arglist -> Arg\n"); }
       | arglist ',' arg
       ;
constarglist:
	CONSTANT
	| constarglist CONSTANT
	;
arg:
   CONSTANT { printf("Arg -> Constant\n"); }
   | VARIABLE { printf("Arg -> Variable\n"); }
   ;
%%


void yyerror(char *s) {
    printf("Error: %s\n", s);
    exit(-1);
}
