// symbol.h

#ifndef _symbol
#define _symbol

#include <stdio.h>
#include <stdlib.h>

#define HASHSIZE 997

#define T_UNDEF      0
#define T_VAR        1
#define T_PROC       2
#define T_FUNC       3
#define T_INT        4
#define T_LABEL      5
#define T_REAL       6
#define T_ARRAY      7
#define T_STRING     8
#define T_BOOL       9
#define T_SPECIAL   10
#define T_PROGRAM   11
#define T_NOT_LOCAL 12
#define T_PARAM     13
#define T_ARG       14
#define T_LOCAL     15
#define T_FILE      16
#define T_CONSTANT  17
#define T_SELF      18
#define T_TEMP      19
#define T_RETURN    20
#define T_OFFSET    21

#define SIZEOF_INT     4
#define SIZEOF_REAL    4
#define SIZEOF_POINTER 4

char *input_file_name;

typedef struct dim_type {

  struct dim_type *next;
  struct dim_type *prev;

  int high;
  int low;
  int dimension;
  int n;
  int node_elements; //count the amount of elements in a single dim_list node

} DIM_TYPE;

typedef struct decl_type {

  int data_type;
  int token_type_1;
  int token_type_2;
  int size;
  int c;
  
  int elements;
  int dimensions;
  struct dim_type *dimension_list;

} DECL_TYPE;

typedef struct param_type {

  struct param_type *next;
  struct symb *param_symb;
  int param;

} PARAM_TYPE;

typedef struct symb {

  struct symb *next;
  struct symb *root_symb;
  
  struct symbol_table *function_st_ptr;

  struct dim_type *dimension_list;
  struct param_type *parameter_list;

  struct TAC *label;

  int token_type_1;
  int token_type_2;
  int data_type;
  int offset;
  int size;
  int dimensions;
  int parameters;
  int int_value;
  int hops;
  int line;
  int c;
  double dbl_value;
  char *string_value;
  char *name;
  int nesting_depth;  

} SYMB, symb;

typedef struct symbol_table {

  char *name;
  struct symbol_table *parent;
  int width;
  int parameters;
  struct param_type *parameter_list;
  SYMB *symtab[HASHSIZE];

} SYMBTABLE;

typedef SYMBTABLE *tableptr;

extern SYMBTABLE *mktable(SYMBTABLE *previous);
extern void mkname(SYMBTABLE *table, char *yytext, int token_type);
extern SYMB *mkconst(int n);
extern void insert(SYMBTABLE *table, SYMB *s);
extern void init_constant_table(void);
extern DIM_TYPE *get_dim(void);
extern PARAM_TYPE *get_param(void);
extern SYMB *lookup(SYMBTABLE *tableptr, char *name);
extern SYMB *mktmp(void);
int calculate_c(DIM_TYPE *dim_list, int w);

#endif
