/**************************/
/* stack.h                */
/**************************/

#ifndef _STACK
#define _STACK

#define MAXSTACKSIZE 100
#include "globals.h"

typedef struct { generic_ptr base[MAXSTACKSIZE] ; generic_ptr *top ; } stack ;

extern status init_stack(  stack *p_S ) ;
extern bool   empty_stack ( stack *p_S ) ;
extern status push ( stack *p_S , generic_ptr data ) ;
extern status pop ( stack *p_S , generic_ptr *p_data ) ;
extern status top ( stack *p_S , generic_ptr *p_data ) ;

#endif


