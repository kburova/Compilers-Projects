# include <stdio.h>
# include <stdlib.h> 
# include "cc.h"
# include "semutil.h"
# include "sem.h"
# include "sym.h"

extern int formalnum;               /* number of formal arguments */
extern char formaltypes[];   /* types of formal arguments  */
extern int localnum;                /* number of local variables  */
extern char localtypes[];    /* types of local variables   */
extern int localwidths[];    /* widths of local variables  */
int label_num = 0;
int bplabel_num = 0;

/*
 * backpatch - backpatch list of quadruples starting at p with k
 */
void backpatch(struct sem_rec *p, int k)
{
    //assign L labels to temporary B labels
    printf("B%d=L%d\n", p->s_place, k);
}

/*
 * bgnstmt - encountered the beginning of a statement
 */
void bgnstmt()
{
    extern int lineno;
    printf("bgnstmt %d\n", lineno);
}

/*
 * call - procedure invocation
 */
struct sem_rec *call(char *f, struct sem_rec *args)
{
    fprintf(stderr, "sem: call not implemented\n");
    return ((struct sem_rec *) NULL);
}

/*
 * ccand - logical and
 */
struct sem_rec *ccand(struct sem_rec *e1, int m, struct sem_rec *e2)
{
    fprintf(stderr, "sem: ccand not implemented\n");
    return ((struct sem_rec *) NULL);
}

/*
 * ccexpr - convert arithmetic expression to logical expression
 */
struct sem_rec *ccexpr(struct sem_rec *e)
{
    struct sem_rec *t;

    t = (struct sem_rec *)malloc(sizeof(struct sem_rec));
    
    //TODO: convert....
    fprintf(stderr, "sem: ccexpr not implemented\n");
    return ((struct sem_rec *) NULL);
}

/*
 * ccnot - logical not
 */
struct sem_rec *ccnot(struct sem_rec *e)
{
    fprintf(stderr, "sem: ccnot not implemented\n");
    return ((struct sem_rec *) NULL);
}

/*
 * ccor - logical or
 */
struct sem_rec *ccor(struct sem_rec *e1, int m, struct sem_rec *e2)
{
    fprintf(stderr, "sem: ccor not implemented\n");
    return ((struct sem_rec *) NULL);
}

/*
 * con - constant reference in an expression
 */
struct sem_rec *con(char *x)
{
    struct id_entry *p;
    struct sem_rec *t;

    if((p = lookup(x, 0)) == NULL) {
        p = install(x, 0);
        p->i_type = T_INT;
        p->i_scope = GLOBAL;
        p->i_defined = 1;
    }
    t = node(nexttemp(), p->i_type, (struct sem_rec *) NULL, (struct sem_rec *) NULL);
    
    printf("t%d := %s\n", currtemp(), x);
    
    return t;
}

/*
 * dobreak - break statement
 */
void dobreak()
{
    n();
}

/*
 * docontinue - continue statement
 */
void docontinue()
{
    n();
}

/*
 * :/startdodo - do statement
 */
void dodo(int m1, int m2, struct sem_rec *e, int m3)
{
    //backpatch(e, m2);
    backpatch(e->back.s_true, m1);
    backpatch(e->s_false, m3);
}

/*
 * dofor - for statement
 */
void dofor(int m1, struct sem_rec *e2, int m2, struct sem_rec *n1,
        int m3, struct sem_rec *n2, int m4)
{
    backpatch(e2->back.s_true, m3);
    backpatch(e2->s_false, m4);
    backpatch(n1, m1);
    backpatch(n2, m2);
}

/*
 * dogoto - goto statement
 */
void dogoto(char *id)
{
    n();
}

/*
 * doif - one-arm if statement
 */
void doif(struct sem_rec *e, int m1, int m2)
{
    //slide 38
    backpatch(e->back.s_true, m1);
    backpatch(e->s_false, m2);
}

/*
 * doifelse - if then else statement
 */
void doifelse(struct sem_rec *e, int m1, struct sem_rec *n,
        int m2, int m3)
{
    //slide 38
    backpatch(e->back.s_true, m1);
    backpatch(n, m2);
    backpatch(e->s_false, m3);
}

/*
 * doret - return statement
 */
void doret(struct sem_rec *e)
{
    char mode;
    // check if T_DOUBLE bit is set and without address bit
    if ( (e->s_mode & T_DOUBLE) && (!e->s_mode & T_ADDR) ){
        mode = 'f';
    }else {
        mode = 'i';
    }

    printf("ret%c t%d\n", mode, e->s_place);
}

/*
 * dowhile - while statement
 */
void dowhile(int m1, struct sem_rec *e, int m2, struct sem_rec *n,
        int m3)
{
    //35
    backpatch(e->back.s_true, m2);
    backpatch(e->s_false, m3);
    backpatch(n, m1);
}

/*
 * endloopscope - end the scope for a loop
 */
void endloopscope(int m)
{
    leaveblock();
}

/*
 * exprs - form a list of expressions
 */
struct sem_rec *exprs(struct sem_rec *l, struct sem_rec *e)
{
    fprintf(stderr, "sem: exprs not implemented\n");
    return ((struct sem_rec *) NULL);
}

/*
 * fhead - beginning of function body
 */
void fhead(struct id_entry *p)
{
    int i;
    printf("func %s\n", p->i_name);
    for (i = 0; i < formalnum; i++){
        if (formaltypes[i] == 'f'){
            printf("formal 8\n");
        } else if (formaltypes[i] == 'i') {
            printf("formal 4\n");
        } else {
            yyerror("formal type stored incorrectly");
        }
    }

    for (i = 0; i < localnum; i++){
        if (localtypes[i] == 'f'){
            printf("localloc %d\n", localwidths[i] * 8);
        } else if (localtypes[i] == 'i') {
            printf("localloc %d\n", localwidths[i] * 4);
        } else {
            yyerror("local type stored incorrectly");
        }
    }
}

/*
 * fname - function declaration
 * P.S. --- Implemented in class ---
 * Install function into symbol table, or get it from
 * the symbol table
 */
struct id_entry *fname(int t, char *id)
{
    struct id_entry *p;

    if (( p = lookup(id, 0)) == NULL) {
        p = install(id, 0);
    }else if (p->i_defined){
        yyerror("procedure previously declared");
    }else if (p->i_type != t){
        yyerror("function declaration types do not match");
    }

    p->i_type = t;
    p->i_scope = GLOBAL;
    p->i_defined = 1;

    // enter function
    enterblock();
    formalnum = 0;
    localnum = 0;

    return p;
}

/*
 * ftail - end of function body
 */
void ftail()
{
    printf("fend\n");
    leaveblock();
}

/*
 * id - variable reference
 */
struct sem_rec *id(char *x)
{
    struct id_entry *p;
    struct sem_rec *t;

    if ((p = lookup(x, 0)) == NULL) {
        yyerror("Undeclared identifier");
        exit(1);
    //    p = install(x, -1);
    //    p->i_type = T_INT;
    //    p->i_scope = LOCAL;
    //    p->i_defined = 1;
    }
    
    if ( p->i_scope == LOCAL ) {
        printf("t%d := local %d\n", nexttemp(), p->i_offset);
    }else if (p->i_scope == PARAM) {
        printf("t%d := param %d\n", nexttemp(), p->i_offset);
        
        //dereference if an array
        if (p->i_type & T_ARRAY) {
            printf("t%d := @i t%d\n", nexttemp(), currtemp());
        }
    }else{
        printf("t%d := global %s\n", nexttemp(), x);
    }
    
    t = node(currtemp(), p->i_type | T_ADDR, (struct sem_rec *) NULL,(struct sem_rec *) NULL);
    
    return t;
}

/*
 * indx - subscript
 */
struct sem_rec *indx(struct sem_rec *x, struct sem_rec *i)
{
    struct sem_rec *t;
    char type = 'i';
    
    //TODO: check i for being integer! 
    t = node(nexttemp(), x->s_mode & ~(T_ARRAY), (struct sem_rec *) NULL,(struct sem_rec *) NULL);
    
    if ( t->s_mode & T_DOUBLE ) {
        type = 'f';
    }

    printf("t%d := t%d []%c t%d\n", currtemp(), x->s_place, type, i->s_place);
    return t;    
}

/*
 * labeldcl - process a label declaration
 */
void labeldcl(char *id)
{
    backpatch(n(), m());
}

/*
 * m - generate label and return next temporary number
 */
int m()
{
    label_num++;
    printf("label L%d\n", label_num);
    return label_num;
}

/*
 * n - generate goto and return backpatch pointer
 */
struct sem_rec *n()
{
    struct sem_rec *t;

    t = (struct sem_rec *)malloc(sizeof(struct sem_rec));
    t->s_place = ++bplabel_num;
    printf("br B%d\n", bplabel_num);

    return t;
}

/*
 * op1 - unary operators
 */
struct sem_rec *op1(char *op, struct sem_rec *y)
{
    struct sem_rec *t;
    char type = 'i';

    // when dereference, type of new temp isn't an address any more
    if ( *op == '@' && !(y->s_mode & T_ARRAY) ) {
        y->s_mode &= ~T_ADDR;
    }
    
    // see if type is double
    if ( (y->s_mode & T_DOUBLE) && ( !(y->s_mode & T_ADDR) ) ) {
        type = 'f';
    }
    
    printf("t%d := %s%c t%d\n", nexttemp(), op, type, y->s_place);

    t = node(currtemp(), y->s_mode, (struct sem_rec *) NULL, (struct sem_rec *) NULL);
    
    return t;
}

/*
 * op2 - arithmetic operators
 */
struct sem_rec *op2(char *op, struct sem_rec *x, struct sem_rec *y)
{
    fprintf(stderr, "sem: op2 not implemented\n");
    return ((struct sem_rec *) NULL);
}

/*
 * opb - bitwise operators
 */
struct sem_rec *opb(char *op, struct sem_rec *x, struct sem_rec *y)
{
    fprintf(stderr, "sem: opb not implemented\n");
    return ((struct sem_rec *) NULL);
}

/*
 * rel - relational operators
 */
struct sem_rec *rel(char *op, struct sem_rec *x, struct sem_rec *y)
{
    struct sem_rec *b, *t;
    char type = 'i';
    int type_i;
   
    // typecast
    if (x->s_mode == y->s_mode) {
        type_i = x->s_mode;
        if ( (type_i & T_DOUBLE) && (!(type_i & T_ADDR)) ) {
            type = 'f';
        }
    }else{
        if (x->s_mode > y->s_mode){
            type_i = x->s_mode;
        }else{
            type_i = y->s_mode;
        }

        if ( (type_i & T_DOUBLE) && (!(type_i & T_ADDR)) ) {
            type = 'f';
        }

        t = node(nexttemp(), type_i, (struct sem_rec *) NULL, (struct sem_rec *) NULL);
        printf("t%d := cv%c t%d\n", currtemp(), type, currtemp()-1);
    }

    b = (struct sem_rec *) malloc (sizeof(struct sem_rec));
    b->back.s_true = (struct sem_rec *) malloc(sizeof(struct sem_rec));
    
    printf("t%d := t%d %s%c t%d\n", nexttemp(), x->s_place, op, type, currtemp());
    
    // set temp label when result is true
    b->back.s_true->s_place = ++bplabel_num;
    printf("bt t%d B%d\n", currtemp(), bplabel_num);
    
    // set another label when result is false
    b->s_false = (struct sem_rec *) malloc(sizeof(struct sem_rec));
    b->s_false->s_place = ++bplabel_num;
    printf("br B%d\n", bplabel_num);

    return b;
}

/*
 * set - assignment operators
 */
struct sem_rec *set(char *op, struct sem_rec *x, struct sem_rec *y)
{
    struct sem_rec *p, *tx, *ty;
    int x_type, y_type;
    char rv_type, type;

    x_type = ( x->s_mode & T_INT ) ? T_INT : T_DOUBLE ;
    y_type = ( y->s_mode & T_INT ) ? T_INT : T_DOUBLE ;
    rv_type = ( x_type == T_INT ) ? 'i' : 'f';
    type = ( y_type == T_INT ) ? 'i' : 'f';
    
    // 1. dereference addresses
    if (y->s_mode & T_ADDR &&  *op != '\0') {
        ty = node(nexttemp(), y_type, (struct sem_rec *) NULL, (struct sem_rec *) NULL); 
        printf("t%d := @%c t%d\n", currtemp(), rv_type, y->s_place);
    }else { ty = y; }

    if (x->s_mode & T_ADDR && *op != '\0') {
        tx = node(nexttemp(), x_type, (struct sem_rec *) NULL, (struct sem_rec *) NULL);
        printf("t%d := @%c t%d\n", currtemp(), rv_type, x->s_place);
    }else { tx = x; }

    // 2. convert x to integer for binary ops ( |, ^, &, <<, >> )
    if ( x_type == T_DOUBLE && 
            ( *op == '|' ||
              *op == '^' ||
              *op == '&' ||
              strncmp(op, "<<", 2) == 0 ||
              strncmp(op, ">>", 2) == 0 ) ){
        
        x_type = T_INT;
        printf("t%d := cvi t%d\n", nexttemp(), currtemp());
        tx = node(currtemp(), x_type, (struct sem_rec *) NULL, (struct sem_rec *) NULL);
    }

    if ( (x_type == T_DOUBLE) && (*op == '%') ) {
        yyerror("cannot %% floating-point values");
    }

    // typecast
    if (x_type != y_type) {

        y_type = x_type;
        type = ( x_type == T_INT ) ? 'i' : 'f';
        
        printf("t%d := cv%c t%d\n", nexttemp(), type, ty->s_place);
        ty = node(currtemp(), y_type, (struct sem_rec *) NULL, (struct sem_rec *) NULL);
    }
    
    if ( *op != '\0') {
        // new temporary for just operation
        printf("t%d := t%d %s%c t%d\n", nexttemp(), tx->s_place, op, type, ty->s_place);
        tx = node(currtemp(), type, (struct sem_rec *) NULL, (struct sem_rec *) NULL);
    }

    // typecast x back if it was changed from f to i for binary ops
    if (rv_type != type) {
        printf("t%d := cv%c t%d\n", nexttemp(), rv_type, tx->s_place);
        tx = node(currtemp(), rv_type, (struct sem_rec *) NULL, (struct sem_rec *) NULL);
    }

    // new temporary for assignment
    printf("t%d := t%d =%c t%d\n", nexttemp(), x->s_place, rv_type, currtemp() );
    p = node(currtemp(), rv_type, (struct sem_rec *) NULL, (struct sem_rec *) NULL);
    
    return p;
}

/*
 * startloopscope - start the scope for a loop
 */
void startloopscope()
{
    enterblock();
}

/*
 * string - generate code for a string
 */
struct sem_rec *string(char *s)
{
    struct sem_rec *t;

    t = (struct sem_rec *)malloc(sizeof(struct sem_rec));
    t->s_place = nexttemp();
    printf("t%d := %s\n", currtemp(), s);

    return t;
}
