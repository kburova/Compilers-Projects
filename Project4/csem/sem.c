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
extern int lineno;

int label_num = 0;
int bplabel_num = 0;

/*
 * backpatch - backpatch list of quadruples starting at p with k
 */
void backpatch(struct sem_rec *p, int k)
{
    //assign L labels to temporary B labels
    
    printf("B%d=L%d\n", p->s_place, k);
    if (p->back.s_link != NULL){
        printf("B%d=L%d\n", p->back.s_link->s_place, k);
    }
}

/*
 * bgnstmt - encountered the beginning of a statement
 */
void bgnstmt()
{
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
    struct sem_rec *b;
    b = (struct sem_rec *) malloc (sizeof(struct sem_rec));
    
    // B -> B1 && M B2
    // { backpatch(B1. truelist, M.instr ) ;
    // B.truelist = B2.truelist;
    // B.falselist = merge(Bl.falselist,B2.falselist);}
    
    backpatch(e1->back.s_true, m);
    b->back.s_true = e2->back.s_true;
    b->s_false = merge(e1->s_false, e2->s_false);
    
    return b;
}

/*
 * ccexpr - convert arithmetic expression to logical expression
 */
struct sem_rec *ccexpr(struct sem_rec *e)
{
    struct sem_rec *t, *z;
    char type;

    z = con("0");
    
    if ( e->s_mode == T_DOUBLE ) {
        printf("t%d := cvf t%d\n", nexttemp(), z->s_place);
        z = node(currtemp(), T_DOUBLE, 
                (struct sem_rec *) NULL, (struct sem_rec *) NULL );
    }
    
    t = rel("!=", e, z);
    
    return t;
}

/*
 * ccnot - logical not
 */
struct sem_rec *ccnot(struct sem_rec *e)
{
    struct sem_rec *b;
    b = (struct sem_rec *) malloc (sizeof(struct sem_rec));
    
    // B -> !B
    //{ B.truelist = Bl.falselist;
    //  B.falselist = Bl.truelist; }
    
    b->back.s_true = e->s_false;
    b->s_false = e->back.s_true;
    
    return b;
}

/*
 * ccor - logical or
 */
struct sem_rec *ccor(struct sem_rec *e1, int m, struct sem_rec *e2)
{
    struct sem_rec *b;
    b = (struct sem_rec *) malloc (sizeof(struct sem_rec));
    
    // B -> B1 || M B2
    // { backpatch(B1.falselist, M.instr ) ;
    // B.truelist = merge(Bl.truelist,B2.truelist);}
    // B.falselist = B2.falselist;
    
    backpatch(e1->s_false, m);
    b->back.s_true = merge(e1->back.s_true, e2->back.s_true);
    b->s_false = e2->s_false;
    
    return b;
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
 * startdodo - do statement
 */
void dodo(int m1, int m2, struct sem_rec *e, int m3)
{
    //backpatch(e, m2);
    backpatch(e->back.s_true, m1);
    backpatch(e->s_false, m3);
   fprintf(stderr, "sem: docontinue not implemented\n");
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
    backpatch(e->s_false, m2);
    backpatch(n, m3);
}

/*
 * doret - return statement
 */
void doret(struct sem_rec *e)
{
    int type;

    // check if T_DOUBLE bit is set and without address bit
    type = ( (e->s_mode & T_DOUBLE) && !(e->s_mode & T_ADDR) ) ? T_DOUBLE : T_INT ;
    
    if ( type & T_DOUBLE ) {
        printf("t%d := cvi t%d\n", nexttemp(), e->s_place);
        e = node(currtemp(), T_INT, (struct sem_rec *)NULL, (struct sem_rec *)NULL);
    }
    
    printf("reti t%d\n", e->s_place);
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
     //   if (p->i_type & T_ARRAY) {
     //       printf("t%d := @i t%d\n", nexttemp(), currtemp());
     //   }
    }else{
        printf("t%d := global %s\n", nexttemp(), x);
    }
    
    t = node(currtemp(), p->i_type | T_ADDR, 
            (struct sem_rec *) NULL,(struct sem_rec *) NULL);
    
    return t;
}

/*
 * indx - subscript
 */
struct sem_rec *indx(struct sem_rec *x, struct sem_rec *i)
{
    struct sem_rec *t;
    char type = 'i';

    
    //reference array
    printf("t%d := @i t%d\n", nexttemp(), x->s_place);
    x = node(currtemp(), x->s_mode & ~T_ADDR, 
            (struct sem_rec *) NULL,(struct sem_rec *) NULL);
    

    //check if index is an integer
    if ( !(i->s_mode & T_INT) ) {
        printf("t%d := cvi t%d\n", nexttemp(), i->s_place);
        i = node(currtemp(), T_INT, 
                (struct sem_rec *) NULL,(struct sem_rec *) NULL);
    }

    type = ( x->s_mode & T_DOUBLE ) ? 'f' : 'i';
    
    printf("t%d := t%d []%c t%d\n", nexttemp(), x->s_place, type, i->s_place);
    t = node(currtemp(), x->s_mode & ~T_ARRAY, 
            (struct sem_rec *) NULL,(struct sem_rec *) NULL);
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
    char type;

    // when de-reference, new temp is not an address
    if (*op == '@' && !(y->s_mode & T_ARRAY) ) {
        y->s_mode &= ~T_ADDR;
    }

     //printf(" In op1 - %s ------------\n", op);

    type = ( y->s_mode & T_INT || y->s_mode & T_ADDR) ? 'i' : 'f';
    
    t = node(nexttemp(), y->s_mode, (struct sem_rec *) NULL, (struct sem_rec *) NULL);
    printf("t%d := %s%c t%d\n", currtemp(), op, type, y->s_place);
    
    return t;
}

/*
 * op2 - arithmetic operators
 */
struct sem_rec *op2(char *op, struct sem_rec *x, struct sem_rec *y)
{
    struct sem_rec *t;
    int x_type, y_type;
    char type;

    x_type = (x->s_mode & T_INT) ? T_INT : T_DOUBLE;
    y_type = (y->s_mode & T_INT) ? T_INT : T_DOUBLE;

    if ( (x_type == T_DOUBLE || y_type == T_DOUBLE) && *op == '%' ) {
        yyerror("cannot %% floating-point values");
    }

    // do typecast to higher type
    if ( x_type > y_type ) {
        y_type = T_DOUBLE;
        printf("t%d := cvf t%d\n", nexttemp(), y->s_place);
        y = node(currtemp(), x->s_mode,
                (struct sem_rec *) NULL, (struct sem_rec *) NULL);
    }else if (x_type < y_type) {
        x_type = T_DOUBLE;
        printf("t%d := cvf t%d\n", nexttemp(), x->s_place);
        x = node(currtemp(), y->s_mode,
                (struct sem_rec *) NULL, (struct sem_rec *) NULL);
    }

    type = ( x_type == T_INT ) ? 'i' : 'f';

    t = node(nexttemp(), x->s_mode, 
            (struct sem_rec *) NULL, (struct sem_rec *) NULL);
    printf("t%d := t%d %s%c t%d\n", currtemp(), x->s_place, op, type, y->s_place);

    return t;
}

/*
 * opb - bitwise operators
 */
struct sem_rec *opb(char *op, struct sem_rec *x, struct sem_rec *y)
{
    struct sem_rec *t;
    int x_type, y_type;

    x_type = (x->s_mode & T_INT) ? T_INT : T_DOUBLE;
    y_type = (y->s_mode & T_INT) ? T_INT : T_DOUBLE;

    // has to be integer arithmetic for these 5 ops
    if ( y_type == T_DOUBLE ) {
        y_type = T_INT;
        printf("t%d := cvi t%d\n", nexttemp(), y->s_place);
        y = node(currtemp(), y->s_mode & ~T_DOUBLE | T_INT,
                (struct sem_rec *) NULL, (struct sem_rec *) NULL);
    }

    if ( x_type == T_DOUBLE ) {
        x_type = T_INT;
        printf("t%d := cvi t%d\n", nexttemp(), x->s_place);
        x = node(currtemp(), x->s_mode & ~T_DOUBLE | T_INT,
                (struct sem_rec *) NULL, (struct sem_rec *) NULL);
    }

    t = node(nexttemp(), x->s_mode, 
            (struct sem_rec *) NULL, (struct sem_rec *) NULL);
    printf("t%d := t%d %si t%d\n", currtemp(), x->s_place, op, y->s_place);

    return t;
}

/*
 * rel - relational operators
 */
struct sem_rec *rel(char *op, struct sem_rec *x, struct sem_rec *y)
{
    struct sem_rec *b, *t;
    char type;
    int x_type, y_type;

    x_type = ( (x->s_mode & T_DOUBLE) && (!(x->s_mode & T_ADDR)) ) ? T_DOUBLE : T_INT ;
    y_type = ( (y->s_mode & T_DOUBLE) && (!(y->s_mode & T_ADDR)) ) ? T_DOUBLE : T_INT ;
    
    if (x_type > y_type){
        
        y_type = x_type;
        printf("t%d := cvf t%d\n", nexttemp(), y->s_place);
        y = node(currtemp(), x->s_mode, (struct sem_rec *) NULL, (struct sem_rec *) NULL);
    
    }else if (x_type < y_type) {
        
        x_type = y_type;
        printf("t%d := cvf t%d\n", nexttemp(), x->s_place);
        x = node(currtemp(), y->s_mode, (struct sem_rec *) NULL, (struct sem_rec *) NULL);
    }
    
    type = (x_type == T_INT) ? 'i' : 'f';

    b = (struct sem_rec *) malloc (sizeof(struct sem_rec));
    b->back.s_true = (struct sem_rec *) malloc(sizeof(struct sem_rec));

    printf("t%d := t%d %s%c t%d\n", nexttemp(), x->s_place, op, type, y->s_place);

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
    struct sem_rec *p, *t;
    int x_type, y_type;
    char rv_type, type;
    
    x_type = ( x->s_mode & T_DOUBLE ) ? T_DOUBLE : T_INT;
    y_type = ( y->s_mode & T_DOUBLE ) ? T_DOUBLE : T_INT;
   
    type = ( x_type == T_INT ) ? 'i' : 'f';
    
    if ( *op != '\0' ) {
        
        // in these 5 we typecast to
        if ( *op == '+' ||
             *op == '-' ||
             *op == '*' ||
             *op == '/' ||
             *op == '%'){

            // typecast right value to left
            if ( y_type !=  x_type ) {
                y_type = x_type;
                printf("t%d := cv%c t%d\n", nexttemp(), type, y->s_place);
                y = node(currtemp(), x->s_mode, 
                        (struct sem_rec *) NULL, (struct sem_rec *) NULL );
            } 

            //get value of left part
            t = op1("@", x);
            
            y = op2(op, t, y);
        // next 5 have to be typecasted to ints    
        } else {

            //get value of left part
            t = op1("@", x);    
            y = opb(op, t, y);
            y_type = y->s_mode;
        }
    }
    
    // typecast right value to left
    if ( y_type !=  x_type ) {
        printf("t%d := cv%c t%d\n", nexttemp(), type, y->s_place);
        y = node(currtemp(), x->s_mode, 
                (struct sem_rec *) NULL, (struct sem_rec *) NULL );
    }
    
    // assignment
    printf("t%d := t%d =%c t%d\n", nexttemp(), x->s_place, type, y->s_place);
    p = node(currtemp(), x->s_mode, 
            (struct sem_rec *) NULL, (struct sem_rec *) NULL );
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
