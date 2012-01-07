// cg.h

#ifndef CG_H
#define CG_H

#include "tac.h"

#define R_ZERO   0  //constant zero
#define R_P      1  //stack pointer
#define R_CALL   2  //address of called routine
#define R_RET    3  //return address
#define R_RES    4  //result reg and last reserved
#define R_COND   5  //conditional code register
#define R_GEN    6  //first general purpose register
#define R_MAX    12 //6 general purpose registers

#define P_OFF    0  //offset of stack pointer on frame
#define PC_OFF   4  //offset of ret address on frame
#define VAR_OFF  8  //offset of variables on frame

#define MODIFIED   1
#define UNMODIFIED 0

//register macros for printing
#define M_EBP "%ebp" //frame pointer
#define M_ESP "%esp" //stack pointer
#define M_EAX "%eax" //general purpose
#define M_EBX "%ebx" //general purpose
#define M_ECX "%ecx" //general purpose
#define M_EDX "%edx" //general purpose
#define M_ESI "%esi" //general purpose
#define M_EDI "%edi" //general purpose
#define M_AL  "%al"  //used for condition codes

#define M_PRINTF_ARG "\"%d\\n\""

struct {
  struct symb *name;
  int modified;
} rdesc[R_MAX];
int tos;
int next_arg;

void cg(TAC *t1);
TAC *init_cg(TAC *t1);
void cg_instruction(TAC *c);
void cg_binop(int op, SYMB *a, SYMB *b, SYMB *c);
void cg_relop(char *op, SYMB *a, SYMB *b, SYMB *c);
void cg_unop(int op, SYMB *a, SYMB *b);
void cg_copy(SYMB *a, SYMB *b);
void cg_cond(char *op, SYMB *a, int l);
void cg_offset(SYMB *a, SYMB *b, SYMB *c);
void cg_arg(SYMB *a);
void cg_print(SYMB *s);
void cg_call(int f, SYMB *res);
void cg_return(SYMB *a);
int insert_desc(int r, SYMB *n, int mod);
void load_reg(int r, SYMB *s);
void clear_desc(int r);
void flush_all();
void spill_all();
void spill_one(int r);
char *find_reg(int enum_val);

#endif CG_H
