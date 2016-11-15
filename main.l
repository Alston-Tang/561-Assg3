%{
#include <stdio.h>
#include "y.tab.h"
%}

%%
[1-9][0-9]*	{
		    printf("Match Number\n");
		    return NUMBER;    
		}
[A-Z][A-Za-z]*  {
		    printf("Match Constant\n");
		    return CONSTANT;
		}
[a-z][A-Za-z]*  {
		    printf("Match Variable\n");
		    return VARIABLE;
		}
\n		{
		    printf("Match Newline\n");
		    return ENDL;
		}
[()&|~=>]		{
		    printf("Match %s\n", yytext);
		    return yytext[0];
		}

%%



