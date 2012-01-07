/*******************************************************************/
/* globals.h                                                       */
/*                                                                 */
/* generic_ptr                                                     */
/* status             see page 41 in Esakov                        */
/* bool               see page 41 in Esakov                        */
/* DATA, NEXT         see page 57 in Esakov                        */
/*******************************************************************/

#ifndef _globals
#define _globals

#define DATA( L ) ( ( L ) -> datapointer )
#define NEXT( L ) ( ( L ) -> next )

typedef enum { OK , ERROR } status ;
typedef enum { FALSE=0 , TRUE=1 } bool ;
typedef void *generic_ptr ;

#endif
