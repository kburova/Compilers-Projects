%{
// Project 3. CS461. Regexes for parsing.
// Written by: Ksenia Burova
#include "y.tab.h"
%}
%%

[0-9]+       	 { sscanf(yytext, "%d", &yylval.num);
               	   return NUM; }

dump			 { return DUMP; }
clear			 { return CLEAR; }

[a-z]	         { yylval.num = yytext[0] - 'a'; 
			       return var; }
(\+\=)			 { return AADD; }
(-=)			 { return ASUB; }
(\*\=)			 { return AMULT; }
(\/=)			 { return ADIV; }
(%=)			 { return AMOD; }
(<<=)			 { return ALSHIFT; }
(>>=)			 { return ARSHIFT; }
(&=)			 { return AAND; }
(^=)			 { return AXOR; }
(\|\=)			 { return AOR; }	

(<<)             { return LSHIFT; }
(>>)			 { return RSHIFT;}

[+\-*\/=()%~&^|]  { return yytext[0]; }

[ \t\r\n]+		   ; //ignore white space

.            	 { return yytext[0]; }		   	
%%
