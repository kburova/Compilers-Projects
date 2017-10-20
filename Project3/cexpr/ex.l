%{
#include "y.tab.h"
%}
%%

[0-9]+       	 { sscanf(yytext, "%d", &yylval.num);
               	   return NUM; }

dump			 { return DUMP; }
clear			 { return CLEAR; }

[a-z]	         { yylval.num = yytext[0] - 'a'; 
			       return var; }

[+\-*/=()%~&^|]  { return yytext[0]; }

[ \t\r\n]+		   ; //ignore white space

.            	 { return yytext[0]; }		   	
%%