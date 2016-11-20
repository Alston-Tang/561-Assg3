%{
#include <stdio.h>
#include <string>
#include <string.h>
#include "ast.h"
#include "y.tab.h"
extern YYSTYPE yylval;
%}

%%
[1-9][0-9]*	{
		    printf("Match Number\n");
		    yylval.val = stoi(std::string(yytext));
		    return NUMBER;
		}
[A-Z][A-Za-z]* {
		    printf("Match Constant\n");
		    yylval.str = yytext;
		    return CONSTANT;
		}
[a-z]*  {
		    printf("Match Variable\n");
		    yylval.str = yytext;
		    return VARIABLE;
		}
\n		{
		    printf("Match Newline\n");
		    return ENDL;
		}
[()&|~=>,]	{
		    printf("Match %s\n", yytext);
		    yylval.str = yytext;
		    return yytext[0];
		}

%%

int yywrap(void) {
    return 1;
}
