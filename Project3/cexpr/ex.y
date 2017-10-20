/*
 * This file defines an example yacc grammar for simple C expressions.
 */

%{
#include <stdio.h>

// 0-25 corresponds to a-z
int vals[26];
int i;

int yyerror( const char *s){
   fprintf(stderr, "%s\n", s);
   return 0;
}

%}

%union {
  int num;
}

%token var '+' '-' '*' '/' '=' '(' ')' DUMP CLEAR 

%token <num> NUM

%type <num> expr term factor equals assign

%%
commands:
	|	commands command
	;

command	: expr ';'       { printf("%d\n", $1); }
        | assign ';'     { printf("%d\n", $1); }
        | DUMP ';'       { for ( i = 0; i < 26; i++) printf("%c: %d\n", i+'a', vals[i]); }
        | CLEAR ';'      { for ( i = 0; i < 26; i++) vals[i] = 0; }
	;

 assign   : var '+' equals { vals[$<num>1] += $3; $$ = vals[$<num>1]; }
          | var '-' equals { vals[$<num>1] -= $3; $$ = vals[$<num>1]; }
          | var equals     { vals[$<num>1] = $2; $$ = vals[$<num>1]; }
          ;
          
 equals : '=' equals    { $$ = $2; }
        | '=' expr      { $$ = $2; }
        ;
 expr   : term              {$$ = $1;}
        | expr '+' term     {$$ = $1 + $3;}
        | expr '-' term     {$$ = $1 - $3;}
        ;
 term   : factor           {$$ = $1;}
        | term '*' factor  {$$ = $1 * $3;}
        | term '/' factor  {if ( $3 == 0 ) yyerror("divisionbyzero\n"); else $$ = $1 / $3;}
        ;
 factor : NUM               {$$ = $1;}
        | equals            {$$ = $1;}
        | var               {$$ = vals[$<num>1];}
        | '(' expr ')'      {$$ = $2;}
        ;

                                                          
%%

int main()
{
   if (yyparse())
      printf("\nInvalid expression.\n");
   else
      printf("\nCalculator off.\n");
}

