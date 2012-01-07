/********************************************************************/
/*                                                                  */
/*                                                                  */
/*                          MAIN ROUTINE                            */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol.h"
#include "tac.h"

extern int yyparse(void);
extern void init_table(void);

extern SYMBTABLE *main_symbol_table;
extern SYMBTABLE *constant_symbol_table;

extern FILE *yyin;
extern int yydebug;
extern TAC *program_tac;

void yyerror(char* text){
  printf("Error parsing, %s\n", text);
}

int main(int argc, char* argv[]){
 
  // set this to 1 if you want to see yacc debug output
  yydebug = 0;

  yyin = fopen(argv[1], "r");
  int len = strlen(argv[1]);
  input_file_name = (char*)safe_malloc(sizeof(char));
  strcpy(input_file_name, argv[1]);

  if(!yyparse()){     
    //dump_table(main_symbol_table);
     //dump_constant_table(constant_symbol_table);
     //dump_tac(program_tac);
     cg(program_tac);
  }
  else
     printf("The file fails to parse.\n");
   
  fclose(yyin);
  return 0;
}
