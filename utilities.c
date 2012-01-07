/********************************************************************/
/*                                                                  */
/*                                                                  */
/*                      COMPILER UTILITIES                          */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol.h"
#include "tac.h"
#include "stack.h"
#include "utilities.h"

void error(char* s){

  fprintf(stderr, "Compiler: %s\n", s);
}

void *safe_malloc(int n){

  void *t = malloc(n);

  if(t == NULL){
    error("malloc() failed");
    exit(0);
  }

  return t;
}

void dump_table(SYMBTABLE *table){

  int i = 0;
  int count = 0;
  SYMBTABLE* temp[100];

  printf( "\n*********************************************************************************************************************************\n" );
  printf( "SYMBOL TABLE NAME: %s \nWIDTH: %d \nPARAMETERS: %d\n\n", table->name, table->width, table->parameters);
  

  printf( "\nName\t Hash\t Line\t Hops\t TokenType1\t TokenType2\t DataType\t Size\t Dimensions\t Offset\t c\n");
  printf( "___________________________________________________________________________________________________________________\n\n" );

  for ( i = 1; i < 997; i++ ){
    if ( table->symtab[i] != NULL ){
      int x = table->symtab[i]->token_type_2;
      int y = table->symtab[i]->token_type_1;
      if ((y == T_SELF) && ((x == T_PROC) || (x == T_FUNC))){	
        temp[count] = table->symtab[i]->function_st_ptr;
	count++;
      }
      if(table->symtab[i]->token_type_2 == T_TEMP){
	printf( "%s\t %d\t %d\t %d\t %s \t\t %s  \t%s\t\t %d\t %d\t\t %d\t %d\n", 
		table->symtab[i]->name, 
		i, 
		table->symtab[i]->line, 
		table->symtab[i]->hops, 
		token_to_string(table->symtab[i]->token_type_1), 
		token_to_string(table->symtab[i]->token_type_2), 
		token_to_string(table->symtab[i]->data_type), 
		table->symtab[i]->size, 
		table->symtab[i]->dimensions,
		table->symtab[i]->offset,
		table->symtab[i]->c);
      }
      else{
	printf( "%s\t %d\t %d\t %d\t %s \t %s  \t%s\t\t %d\t %d\t\t %d\t %d\n", 
		table->symtab[i]->name, 
		i, 
		table->symtab[i]->line, 
		table->symtab[i]->hops, 
		token_to_string(table->symtab[i]->token_type_1), 
		token_to_string(table->symtab[i]->token_type_2), 
		token_to_string(table->symtab[i]->data_type), 
		table->symtab[i]->size, 
		table->symtab[i]->dimensions,
		table->symtab[i]->offset,
		table->symtab[i]->c);
      }
    }
  }
printf( "\n*********************************************************************************************************************************\n" );

 for(i = 0; i < count; i++)
   dump_table(temp[i]);

 printf("\n\n");

}

void dump_constant_table(SYMBTABLE *constant_symbol_table){

  int i;

  printf("\n\n%s\n", constant_symbol_table->name);
  printf("********************************************\n");
  for(i = 1; i < 997; i++){

    if(constant_symbol_table->symtab[i] != NULL){
      printf("CONSTANT_SYMBOL: %s\n", constant_symbol_table->symtab[i]->name);   
    }
  }   
}

char *token_to_string(int token){

  switch (token){

  case 0:
    return "T_UNDEF";
  case 1:
    return "T_VAR";
  case 2:
    return "T_PROC";
  case 3:
    return "T_FUNC";
  case 4:
    return "T_INT";
  case 5:
    return "T_LABEL";
  case 6:
    return "T_REAL";
  case 7:
    return "T_ARRAY";
  case 8:
    return "T_STRING";
  case 9:
    return "T_BOOL";
  case 10:
    return "T_SPECIAL";
  case 11:
    return "T_PROGRAM";
  case 12:
    return "T_NOT_LOCAL";
  case 13:
    return "T_PARAM";
  case 14:
    return "T_ARG";
  case 15:
    return "T_LOCAL";
  case 16:
    return "T_FILE";
  case 17:
    return "T_CONSTANT";
  case 18:
    return "T_SELF";
  case 19:
    return "T_TEMP";
  case 20:
    return "T_RETURN";
  default:
    return "ERROR NO TYPE";
  }
}

void print_tac(TAC *i){

  char sa[12];
  char sb[12];
  char sc[12];

  printf("\nTAC\n");
  printf("  op: ");
  switch(i->op)
    {
    case TAC_UNDEF:
      printf("UNDEF\n");
      break;
    case TAC_ADD:
      printf("ADD\n");
      break;
    case TAC_SUB:
      printf("SUB\n");
      break;
    case TAC_MUL:
      printf("MUL\n");
      break;
    case TAC_DIV:
      printf("DIV\n");
      break;
    case TAC_NEG:
      printf("NEG\n");
      break;
    case TAC_COPY:
      printf("COPY\n");
      break;
    case TAC_GOTO:
      printf("GOTO\n");
      break;
    case TAC_IFZ:
      printf("IFZ\n");
      break;
    case TAC_IFNZ:
      printf("IFNZ\n");
      break;
    case TAC_ARG:
      printf("ARG\n");
      break;
    case TAC_CALL:
      printf("CALL\n");
      break;
    case TAC_RETURN:
      printf("RETURN\n");
      break;
    case TAC_LABEL:
      printf("LABEL\n");
      break;
    case TAC_VAR:
      printf("VAR\n");
      break;
    case TAC_BEGINFUNC:
      printf("BEGINFUNC\n");
      break;
    case TAC_ENDFUNC:
      printf("ENDFUNC\n");
      break;
    case TAC_BEGINPROC:
      printf("BEGINPROC\n");
      break;
    case TAC_ENDPROC:
      printf("ENDPROC\n");
      break;
    case TAC_BEGINPROG:
      printf("BEGINPROC\n");
      break;
    case TAC_ENDPROG:
      printf("ENDPROC\n");
      break;
    case TAC_MAINIDLIST:
      printf("MAIN_ID_LIST\n");
      break;
    case TAC_MOD:
      printf("MOD\n");
      break;
    case TAC_AND:
      printf("AND\n");
      break;
    case TAC_OR:
      printf("OR\n");
      break;
    case TAC_TDIV:
      printf("DIV\n");
      break;
    case TAC_POS:
      printf("POS\n");
      break;
    case TAC_NOT:
      printf("NOT\n");
      break;
    case TAC_EQUAL:
      printf("EQUAL\n");
      break;
    case TAC_NE:
      printf("NE\n");
      break;
    case TAC_LE:
      printf("LE\n");
      break;
    case TAC_GE:
      printf("GE\n");
      break;
    case TAC_GRTRTHAN:
      printf("GRTRTHAN\n");
      break;
    case TAC_LESSTHAN:
      printf("LESSTHAN\n");
      break;
    case TAC_OFFSET:
      printf("OFFSET\n");
      break;
    case TAC_PRINT:
      printf("PRINT\n");
      break;
    default:
      error("unknown TAC opcode");
      break;
    }

  printf("  instruction: ");

  switch(i->op)
    {
    case TAC_UNDEF:
      printf("undef\n");
      break;
    case TAC_ADD:
      printf("%s := %s + %s\n", ts(i->a.var, sa), ts(i->c.var, sb), ts(i->b.var, sc));
      break;
    case TAC_SUB:
      printf("%s := %s - %s\n", ts(i->a.var, sa), ts(i->c.var, sb), ts(i->b.var, sc));
      break;
    case TAC_MUL:
      printf("%s := %s * %s\n", ts(i->a.var, sa), ts(i->c.var, sb), ts(i->b.var, sc));
      break;
    case TAC_DIV:
      printf("%s := %s / %s\n", ts(i->a.var, sa), ts(i->c.var, sb), ts(i->b.var, sc));
      break;
    case TAC_NEG:
      printf("%s := %s\n", ts(i->a.var, sa), ts(i->c.var, sc));
      break;
    case TAC_COPY:      
      printf("%s := %s\n", ts(i->a.var, sa), ts(i->b.var, sb));
      break;
    case TAC_GOTO:
      printf("goto L%d\n", i->a.lab->a.var->int_value);
      break;
    case TAC_IFZ:
      printf("ifz %s goto L%d\n", ts(i->b.var, sb), i->a.lab->a.var->int_value);
      break;
    case TAC_IFNZ:
      printf("ifnz %s goto L%d\n", ts(i->b.var, sb), i->a.lab->a.var->int_value);
      break;
    case TAC_ARG:
      printf("arg %s\n", ts(i->a.var, sa));
      break;
    case TAC_CALL:
      {
        if(i->a.var == NULL)
	  printf("call L%d\n", i->b.lab->a.var->int_value);
        else	  
	  printf("%s = call L%d\n", ts(i->a.var, sa), i->b.lab->a.var->int_value);	
        break;
      }
    case TAC_RETURN:
      printf("return %s\n", ts(i->a.var, sa));
      break;
    case TAC_LABEL:
      printf("label L%d\n", i->a.var->int_value);
      break;
    case TAC_VAR:
      printf("var %s\n", ts(i->a.var, sa));
      break;
    case TAC_BEGINFUNC:
      printf("beginfunc\n");
      break;
    case TAC_ENDFUNC:
      printf("endfunc\n");
      break;
    case TAC_BEGINPROC:
      printf("beginproc\n");
      break;
    case TAC_ENDPROC:
      printf("endproc\n");
      break;
    case TAC_BEGINPROG:
      printf("beginprog\n");
      break;
    case TAC_ENDPROG:
      printf("endprog\n");
      break;
    case TAC_MAINIDLIST:
      printf("main_identifier_list\n");
      break;
    case TAC_MOD:
      printf("%s := %s MOD %s\n", ts(i->a.var, sa), ts(i->c.var, sb), ts(i->b.var, sc));
      break;
    case TAC_AND:
      printf("%s := %s AND %s\n", ts(i->a.var, sa), ts(i->c.var, sb), ts(i->b.var, sc));
      break;
    case TAC_OR:
      printf("%s := %s OR %s\n", ts(i->a.var, sa), ts(i->c.var, sb), ts(i->b.var, sc));
      break;
    case TAC_TDIV:
      printf("%s := %s DIV %s\n", ts(i->a.var, sa), ts(i->c.var, sb), ts(i->b.var, sc));
      break;
    case TAC_POS:
      printf("%s := + %s\n", ts(i->a.var, sa), ts(i->c.var, sc));
      break;
    case TAC_NOT:
      printf("%s := NOT( %s )\n", ts(i->a.var, sa), ts(i->c.var, sc));
      break;
    case TAC_EQUAL:
      printf("%s := ( %s = %s )\n", ts(i->a.var, sa), ts(i->c.var, sb), ts(i->b.var, sc));
      break;
    case TAC_NE:
      printf("%s := ( %s <> %s )\n", ts(i->a.var, sa), ts(i->c.var, sb), ts(i->b.var, sc));
      break;
    case TAC_LE:
      printf("%s := ( %s <= %s )\n", ts(i->a.var, sa), ts(i->c.var, sb), ts(i->b.var, sc));
      break;
    case TAC_GE:
      printf("%s := ( %s >= %s )\n", ts(i->a.var, sa), ts(i->c.var, sb), ts(i->b.var, sc));
      break;
    case TAC_GRTRTHAN:
      printf("%s := ( %s > %s )\n", ts(i->a.var, sa), ts(i->c.var, sb), ts(i->b.var, sc));
      break;
    case TAC_LESSTHAN:
      printf("%s := ( %s < %s )\n", ts(i->a.var, sa), ts(i->c.var, sb), ts(i->b.var, sc));
      break;
    case TAC_OFFSET:
      printf("%s := %s[%s]\n", ts(i->a.var, sa), ts(i->b.var, sb), ts(i->b.var, sc));
      break;
    case TAC_PRINT:
      printf("print %s\n", ts(i->a.var, sa));
      break;
    default:
      {
        error("print_tac(): unknown TAC opcode");
        printf("unknown %d\n", i->op);
        break;
      }
      fflush(stdout);
    }
}

char *ts(SYMB *s, char *str){

  if(s == NULL)
    return "NULL";

  switch(s->token_type_2)
    {
    case T_FUNC:
      return s->name;
    case T_VAR:
      return s->name;

    case T_INT:
      sprintf(str, "%d" , s->int_value);
      return str;

   case T_REAL:
     sprintf(str, "%lf" , s->dbl_value);
      return str;

    case T_ARRAY:
      sprintf(str, "%s", s->name);
      return str;

    case T_TEMP:
      sprintf(str, "%s", s->name);
      return str;

    case T_RETURN:
      sprintf(str, "%s", s->name);
      return str;

    default:
      printf("ts(): unknown TAC arg type, symb name: %s", s->name);
      return;
    }
}

TAC *insert_tac(TAC *t, TAC *list){

  if(list == NULL){
    t->next = NULL;
    return t;
  }

  t->next = list;
  list = t;

  return list;
}

void dump_tac(TAC *tac_list){

  if(tac_list == NULL)
    printf("dump_tac() failed: NULL argument\n");

  TAC *list = NULL;
  TAC *temp = tac_list;

  while(temp->prev != NULL){
    list = insert_tac(temp, list);
    temp = temp->prev;
  }
 
  printf("\nTHREE ADDRESS CODES\n");
  printf("********************************************");

  print_tac(list);

  while(list->next != NULL){
    list = list->next;
    print_tac(list);    
  }
}
