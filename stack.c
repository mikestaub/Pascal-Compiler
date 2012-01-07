/*************************/
/* stack.c               */
/*************************/

#include "stack.h"
#include "globals.h"

#define current_stacksize( p_S ) ( (p_S) -> top - (p_S) -> base )

extern status init_stack ( stack *p_S ) {

  p_S -> top = p_S -> base ;
  return OK ;
}

extern status pop( stack *p_S , generic_ptr *p_data ) {

  if ( empty_stack ( p_S ) == TRUE ) return ERROR ;

  p_S -> top-- ;

  *p_data = *p_S->top ;

  return OK ;
}

extern status top ( stack *p_S , generic_ptr *p_data ) {

  if ( pop ( p_S , p_data ) == ERROR ) return ERROR ;

       return push ( p_S , *p_data ) ;
}

extern bool empty_stack ( stack *p_S ) {

  return ( ( p_S -> top == p_S -> base) ? TRUE : FALSE ) ;
}

extern status push ( stack *p_S , generic_ptr data ) {

  if ( current_stacksize( p_S) == MAXSTACKSIZE )
    return ERROR ;

  *p_S -> top = data ;
  p_S -> top++ ;

  return OK ;
}
