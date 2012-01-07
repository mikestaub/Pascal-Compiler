/********************************/
/* stack_interface.c            */
/********************************/

#include "stack_interface.h"
#include <stdio.h>
#include <stdlib.h>

status push_table ( stack *p_S , SYMBTABLE *c ) {

  if ( push(p_S, (generic_ptr)c) == ERROR ) {
    free (c); 
    return ERROR ;
  }
  
  return OK ;
}

status pop_table ( stack *p_S , SYMBTABLE **p_c ) {

  SYMBTABLE *p_data ;

  if ( pop( p_S, (generic_ptr *) &p_data ) == ERROR )
    return ERROR ;

  *p_c = p_data ;

  //free(p_data);

  return OK ;

}

status top_table ( stack *p_S , SYMBTABLE **table ) {

  SYMBTABLE *p_data ;

  if ( top(p_S, (generic_ptr *) &p_data ) == ERROR )
    return ERROR ;

  *table = p_data ;

  return OK ;

}


status push_offset ( stack *p_S , int offset ) {
 
  int *p_c = (int *) malloc (sizeof(int)) ;
  if ( p_c == NULL ) return ERROR ;
  *p_c = offset ;
  if ( push(p_S, (generic_ptr) p_c) == ERROR ) {
    free (p_c); return ERROR ;
  }
  
  return OK ;
}


status pop_offset ( stack *p_S , int *offset ) {

  int *p_data ;

  if ( pop( p_S, (generic_ptr *)&p_data ) == ERROR )
    return ERROR ;

  *offset = *p_data ;

  free (p_data) ;

  return OK ;

}

status top_offset ( stack *p_S , int *p_c ) {

  int *p_data ;

  if ( top(p_S, (generic_ptr *) &p_data ) == ERROR )
    return ERROR ;

  p_c = p_data ;

  return OK ;

}
