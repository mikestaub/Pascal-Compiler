// tac.h

#ifndef TAC_H
#define TAC_H

#include "symbol.h"

#define TAC_UNDEF       0
#define TAC_ADD         1
#define TAC_SUB         2
#define TAC_MUL         3
#define TAC_DIV         4
#define TAC_NEG         5
#define TAC_COPY        6
#define TAC_GOTO        7
#define TAC_IFZ         8
#define TAC_IFNZ        9
#define TAC_ARG        10
#define TAC_CALL       11
#define TAC_RETURN     12
#define TAC_MOD        13
#define TAC_AND        14
#define TAC_OR         15
#define TAC_TDIV       16
#define TAC_POS        17
#define TAC_NOT        18

#define TAC_LABEL      19
#define TAC_VAR        20
#define TAC_BEGINFUNC  21
#define TAC_ENDFUNC    22
#define TAC_MAINIDLIST 23
#define TAC_BEGINPROC  24
#define TAC_ENDPROC    25
#define TAC_BEGINPROG  26
#define TAC_ENDPROG    27
#define TAC_OFFSET     34
#define TAC_PRINT      35

//Added for IF and WHILE 
#define TAC_LESSTHAN   28
#define TAC_GRTRTHAN   29
#define TAC_NE         30
#define TAC_LE         31
#define TAC_GE         32
#define TAC_EQUAL      33

typedef struct tac
{

  struct tac *next;
  struct tac *prev;
  int op;

  union
  {
    SYMB *var;
    struct tac *lab;
  } a;

  union
  {
    SYMB *var;
    struct tac *lab;
  } b;

  union
  {
    SYMB *var;
    struct tac *lab;
  } c;

} TAC ;

typedef struct enode
{
  struct enode *next;
  TAC *tac;
  SYMB *res;

  //for array expressions
  SYMB *array;
  SYMB *offset;
  int ndim;

} ENODE;

TAC *do_assign(SYMB *func, ENODE *expr);
ENODE *do_bin(int binop, ENODE *expr1, ENODE *expr2, SYMBTABLE* table);
TAC *join_tac(TAC *tac1, TAC *tac2);
ENODE *make_enode(ENODE *next, SYMB *res, TAC *code);
TAC* declare_var(SYMB* var);
TAC *do_program(SYMB *func, SYMB *args, TAC *code);
TAC *do_func(SYMB *func, TAC *args, TAC *code);
TAC *do_proc(SYMB *proc, TAC *args, TAC *code);
SYMB *make_label(int l);
ENODE *do_unary(int unop, ENODE *expr, SYMBTABLE *table);
TAC *do_test(ENODE *expr, TAC *stmt1, TAC *stmt2);
TAC *do_if(ENODE *expr, TAC *stmt);
TAC *do_while( ENODE *expr, TAC *stmt );
ENODE *do_relop(int relop, ENODE *expr1, ENODE *expr2);
TAC *do_assign_offset(ENODE *expr1, ENODE *expr2);
int limit(SYMB *array, int m);
ENODE *call_func(SYMB *func, ENODE *arglist);
TAC *call_proc(SYMB *proc, ENODE *arglist);
TAC *do_print(ENODE *expr);

#endif
