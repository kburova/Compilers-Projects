%{
/*
 * CS461. Project 3.
 * Implementing grammar for calculator in C
 * Completed by : Ksenia Burova
*/
#include <stdio.h>
#include <limits.h>

// 0-25 corresponds to a-z
int vals[26];
int i;
int error = 0;

int yyerror( const char *s){
   error = 1;
   printf("%s\n", s);
   return 0;
}

/* I've used multiple sources to find calculations for overflow */
int Overflow( int operation, int a, int b) {

  // do not output error message twice
  if (error == 1) return 1;

  //addition
  if ( operation == 0 ) {
    if ( ((b > 0) && (a > INT_MAX - b)) || ((b < 0) && (a < INT_MIN - b)) ){
      yyerror("overflow"); 
      return 1;
    }

  //subtract
  }else if ( operation == 1 ) {
    if ( ((b < 0) && (a > INT_MAX + b)) || ((b > 0) && (a < INT_MIN + b)) ){
      yyerror("overflow"); 
      return 1;
    }

  //multiplication
  }else if (operation == 2) {
    if ( (b > 0 && a > INT_MAX / b) ||
         (b > 0 && a < INT_MIN / b) ||
         (b < 0 && a < INT_MAX / b) ||
         (b < 0 && a > INT_MIN / b) 
        ) {
      yyerror("overflow"); 
      return 1;
    }      
  //lshift
  }else if (operation == 3) {
    if ( (a < 0) || (b < 0) ||
         (b >= __builtin_popcount(UINT_MAX)) ||
         (a > (INT_MAX >> b))) {
      yyerror("overflow"); 
      return 1;
    }
  //negation
  }else if (operation == 4) {
    if ( a == INT_MIN ){
      yyerror("overflow"); 
      return 1;
    }
  //remainder
  }else if (operation == 5) {
    if ( (b == 0 ) || ((a == INT_MIN) && (b == -1)) ) {
      yyerror("overflow"); 
      return 1;
    }    
  }
  return 0;
}

//function that does opearation and assignment
int operation(int val, int varName, int a, int b){
  
  switch (val){

    case 0:
      a = b;
      break;

    case 1:
      if ( !Overflow(0,a,b) )
        a += b;
      break;
        
    case 2:
      if ( !Overflow(1,a,b) )
        a -= b;
      break;

    case 3:
      if ( !Overflow(2,a,b) )
        a *= b;
      break;

    case 4:
      if (b == 0) yyerror("dividebyzero");
      else {
        a /= b;
      }
      break;

    case 5:
      if ( !Overflow(5,a,b) )
        a %= b;
      break;

    case 6:
      if ( !Overflow(3,a,b) )
        a <<= b;
      break;

    case 7:
      a >>= b;
      break;

    case 8:
      a &= b;
      break;

    case 9:
      a ^= b;
      break;

    case 10:
      a |= b;
      break;
    
    default: 
      break;
  }
  vals[varName] = a;
  return a; 
}

%}

%union {
  int num;
  long lnum;
}

%token '+' '-' '*' '/' '=' '%' '^' '|' '&' '(' ')' LSHIFT RSHIFT DUMP CLEAR AADD ASUB AMULT ADIV AMOD ALSHIFT ARSHIFT AAND AXOR AOR

%token <num> var
%token <lnum> NUM

%type <num> expr sum prod term paran or shift and xor OP neg not

%%
commands:
	|	commands command
	;

command	: expr ';'       { if (!error) printf("%d\n", $1); error = 0; }
        | DUMP ';'       { for ( i = 0; i < 26; i++) printf("%c: %d\n", i+'a', vals[i]); }
        | CLEAR ';'      { for ( i = 0; i < 26; i++) vals[i] = 0; error = 0; }
	;

 expr   : var OP expr   { if (!error) $$ = operation($2, $1, vals[$1], $3); }
        | or            { $$ = $1; }
        ;

 OP     : '='         { $$ = 0; }
        | AADD        { $$ = 1; }
        | ASUB        { $$ = 2; }
        | AMULT       { $$ = 3; }
        | ADIV        { $$ = 4; }
        | AMOD        { $$ = 5; }
        | ALSHIFT     { $$ = 6; }
        | ARSHIFT     { $$ = 7; }
        | AAND        { $$ = 8; }
        | AXOR        { $$ = 9; }
        | AOR         { $$ = 10;}
        ;
 
 or     : xor               { $$ = $1; }
        | or '|' xor        { $$ = $1 | $3; }
        ;

 xor    : and               { $$ = $1; }
        | xor '^' and       { $$ = $1 ^ $3; }
        ;

 and    : shift             { $$ = $1; }
        | and '&' shift     { $$ = $1 & $3; }
        ;

 shift  : sum               { $$ = $1; }
        | shift LSHIFT sum  { if ( !Overflow(3,$1,$3) ) $$ = $1 << $3;}
        | shift RSHIFT sum  { $$ = $1 >> $3;}
        ;

 sum    : prod             { $$ = $1; }
        | sum '+' prod     { if ( !Overflow(0,$1,$3) ) $$ = $1 + $3; }
        | sum '-' prod     { if ( !Overflow(1,$1,$3) ) $$ = $1 - $3; }
        ;

 prod   : neg              { $$ = $1; }
        | prod '*' neg     { if ( !Overflow(2,$1,$3) ) $$ = $1 * $3; }
        | prod '/' neg     { if ( $3 == 0 ) { 
                              yyerror("dividebyzero");
                              } else {
                                  $$ = $1 / $3; 
                              }
                           }
        | prod '%' neg     { if ( !Overflow(5,$1,$3) ) $$ = $1 % $3; }
        ;
 
 neg    :  not             { $$ = $1; }
        |  '-' neg         { if ( !Overflow(4,$2,0) ) $$ = -$2; }
        ;
 
 not    :  term            { $$ = $1; }
        |  '~' not         { $$ = ~$2; }
        ;
 
 term   : NUM              { if($1 > INT_MAX || $1 < INT_MIN) yyerror("overflow"); else $$ = $1; }
        | var              { $$ = vals[$1];}
        | paran            { $$ = $1;}
        ;

 paran  : '(' or ')'       { $$ = $2;}
        ;
%%

int main()
{
   if (yyparse())
      printf("\nInvalid expression.\n");
   else
      printf("\nCalculator off.\n");
}

