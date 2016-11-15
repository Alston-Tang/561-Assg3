%{
#include <stdio.h>
#include <stdlib.h>
int yylex(void);
void yyerror(char*);
%}

%token NUMBER CONSTANT VARIABLE ENDL

%%
program:
       nq ENDL queries ENDL ns ENDL sentences {printf("Reduce Program\n");}
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
query: atom
       | '~' atom
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
arglist:
       arg { printf("Arglist -> Arg\n"); }
       | arglist ',' arg
       ;
arg:
   CONSTANT { printf("Arg -> Constant\n"); }
   | VARIABLE { printf("Arg -> Variable\n"); }
   ;
%%

int main(void) {
    yyparse();
    return 0;
}

void yyerror(char *s) {
    printf("Error: %s\n", s);
    exit(-1);
}
