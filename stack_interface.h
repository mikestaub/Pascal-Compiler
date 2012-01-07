/********************************/
/* stack_interface.h            */
/********************************/

#ifndef _stack_interface
#define _stack_interface

#include "symbol.h"
#include "globals.h"
#include "stack.h"

extern status push_table ( stack *p_S , SYMBTABLE *c ) ;
extern status pop_table ( stack *p_S , SYMBTABLE **c ) ;
extern status top_table ( stack *p_S , SYMBTABLE **c ) ;

extern status push_offset ( stack *p_S , int c ) ;
extern status pop_offset ( stack *p_S , int *c ) ;
extern status top_offset ( stack *p_S , int *c ) ;

#endif
