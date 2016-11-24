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
		    yylval.val = stoi(std::string(yytext));
		    return NUMBER;
		}
[A-Z][A-Za-z]* {
		    yylval.str = yytext;
		    return CONSTANT;
		}
[a-z]*  {
		    yylval.str = yytext;
		    return VARIABLE;
		}
\n		{
		    return ENDL;
		}
[()&|~=>,]	{
		    yylval.str = yytext;
		    return yytext[0];
		}

%%

int yywrap(void) {
    return 1;
}
