/*
 * This file defines an example yacc grammar for simple C sumessions.
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

%token var '+' '-' '*' '/' '=' '(' ')' LSHIFT RSHIFT '%' '^' '|' '&' DUMP CLEAR 

%token <num> NUM

%type <num> expr assign sum prod term

%%
commands:
	|	commands command
	;

command	: expr ';'       { printf("%d\n", $1); }
        | DUMP ';'       { for ( i = 0; i < 26; i++) printf("%c: %d\n", i+'a', vals[i]); }
        | CLEAR ';'      { for ( i = 0; i < 26; i++) vals[i] = 0; }
	;

 expr   : var assign     { vals[$<num>1] = $2; $$ = vals[$<num>1]; }
        | sum            { $$ = $1; }
        ;
          
 assign : '=' assign      { $$ = $2; }
        | '+' '=' assign  { vals[$<num>1] += $3; $$ = vals[$<num>1]; }
        | '=' '-' assign  { vals[$<num>1] -= $3; $$ = vals[$<num>1]; }
        | '*' '=' assign  { vals[$<num>1] *= $3; $$ = vals[$<num>1]; }
        | '=' '/' assign  { if ( $3 == 0 ) yyerror("divisionbyzero\n"); 
                            else { vals[$<num>1] /= $3; 
                                   $$ = vals[$<num>1]; }             }
        | '=' '%' assign  { vals[$<num>1] -= $3; $$ = vals[$<num>1]; }
        | '=' '^' assign  { vals[$<num>1] -= $3; $$ = vals[$<num>1]; }
        | '=' '|' assign  { vals[$<num>1] -= $3; $$ = vals[$<num>1]; }
        | '=' '&' assign  { vals[$<num>1] -= $3; $$ = vals[$<num>1]; }
        | '=' RSHIFT assign { vals[$<num>1] >>= $3; $$ = vals[$<num>1]; }
        | '=' LSHIFT assign { vals[$<num>1] <<= $3; $$ = vals[$<num>1]; }
        | '=' expr        { $$ = $2; }
        ;

 sum    : prod             {$$ = $1;}
        | sum '+' prod     {$$ = $1 + $3;}
        | sum '-' prod     {$$ = $1 - $3;}
        ;

 prod   : term           {$$ = $1;}
        | prod '*' term  {$$ = $1 * $3;}
        | prod '/' term  {if ( $3 == 0 ) yyerror("divisionbyzero\n"); else $$ = $1 / $3;}
        | prod '%' term  {$$ = $1 % $3;}
        ;

 term   : NUM               {$$ = $1;}
        | assign            {$$ = $1;}
        | var               {$$ = vals[$<num>1];}
        | '~' sum           {$$ = ~$2;}
        | '-' sum           {$$ = -$2;}
        | '(' sum ')'       {$$ = $2;}
        ;
%%

int main()
{
   if (yyparse())
      printf("\nInvalid expression.\n");
   else
      printf("\nCalculator off.\n");
}

