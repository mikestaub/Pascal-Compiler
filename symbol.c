/********************************************************************/
/*                                                                  */
/*                                                                  */
/*                       SYMBOL GENERATOR                           */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/********************************************************************/

#include "symbol.h"
#include "tac.h"
#include "y.tab.h"
#include <string.h>
#include <math.h>

extern SYMBTABLE  *constant_symbol_table;
extern SYMBTABLE  *table_ptr;
extern DIM_TYPE   *dim_p;
extern PARAM_TYPE *param;
extern YYSTYPE yylval;

extern SYMBTABLE *main_symbol_table;
extern int executable_statement_mode;
extern int line_count;
extern char *yytext;
extern int yyleng;

void mkname( SYMBTABLE *table, char *yytext, int token_type ){
  
  SYMB *tmp = NULL;
  SYMBTABLE *t = table;
  int len, count = 0;
  char *temp_name;

  //if its a temp, just add to symbol table and return
  if(token_type == T_TEMP){
    SYMB *temp = (symb*)safe_malloc(sizeof(symb));
    len = strlen(yytext);
    temp_name = (char*)safe_malloc(sizeof(char)*len + 1);
    strcpy(temp_name, yytext);
    temp->name = temp_name;
    temp->token_type_1 = T_VAR;
    temp->token_type_2 = T_TEMP;   
    temp->size = SIZEOF_INT;
    insert(table, temp);
    yylval.symb = temp;
    return;
  }  

  if((tmp = lookup(t, yytext)) != NULL){
    yylval.symb = tmp;
    return;
  }

  //check to see if the symb exists in any other symbol table
  while(t->parent != NULL){    
    if(tmp != NULL)
      break;
    t = t->parent;
    count++;
    tmp = lookup(t, yytext);
  }   

  SYMB *temp = (symb*)safe_malloc(sizeof(symb));    

  //populate the new symb as a non-local  
  if((count > 0) && (executable_statement_mode == 1)){
    len = strlen(yytext);
    temp_name = (char*)safe_malloc(sizeof(char)*len + 1);
    strcpy(temp_name, yytext);
    temp->name = temp_name;    
    temp->token_type_1 = T_NOT_LOCAL; 
    temp->token_type_2 = tmp->token_type_2;
    temp->data_type = tmp->data_type;
    temp->hops = count;
    temp->root_symb = tmp; 
    temp->line = line_count;
    temp->label = tmp->label;
    temp->size = tmp->size;
    temp->dimension_list = tmp->dimension_list;
    temp->dimensions = tmp->dimensions;
    temp->offset = tmp->offset;
    temp->c = tmp->c;
  }
  //populate the symb as a local
  else{
    len = strlen(yytext); 
    temp_name = (char*)safe_malloc(sizeof(char)*len + 1);
    strcpy(temp_name, yytext);     
    temp->name = temp_name;
    temp->token_type_1 = T_CONSTANT;
    temp->token_type_2 = token_type;
    if(token_type == T_INT)
      temp->int_value = atoi(temp->name);     
    if(token_type == T_REAL)      
      temp->dbl_value = atof(temp->name);
    
  }

  insert(table, temp);

  yylval.symb = temp;
}

int hash(char *s){

  int hval = 0;
  int i;
  
  for(i = 0; s[i] != '\0'; i++){

    int v = (hval >> 28) ^ (s[i] & 0xf);
    hval = (hval << 4) | v;
  }

  hval = hval & 0x7fffffff;

  return hval % HASHSIZE;
}

void insert(SYMBTABLE *table, SYMB *s){

  int hval = hash(s->name);

  //can 2 things hash to the same place in array?
  s->next = table->symtab[hval]; //insert at head
  table->symtab[hval] = s;
}

//lookup return NULL if symbol is not found
SYMB *lookup(SYMBTABLE *table, char *name){

  int hval = hash(name);
  SYMB *t = table->symtab[hval];

  while(t != NULL){

    if (strcmp(t->name, name) == 0)
      break;
    else 
      t = t->next;
  }
  return t;
}

void init_constant_table(void){

  constant_symbol_table = mktable(NULL);
}

DIM_TYPE *get_dim(void){

  DIM_TYPE *dimp_p = (DIM_TYPE*)malloc(sizeof(DIM_TYPE));
  return dim_p;
}

PARAM_TYPE *get_param(void){

  PARAM_TYPE *param = (PARAM_TYPE*)safe_malloc(sizeof(PARAM_TYPE));
  return param;
}

SYMBTABLE *mktable(SYMBTABLE *previous){

  int i;

  SYMBTABLE *temp = (SYMBTABLE*)safe_malloc(sizeof(SYMBTABLE));

  temp->parent = previous;

  for(i = 0; i < 997; i++){
    temp->symtab[i] = NULL;
  }

  return temp;
}

SYMB *mkconst(int n){
  //insert into const symb tab?
  SYMB *c = (SYMB*)safe_malloc(sizeof(SYMB));
  c->token_type_2 = T_INT;
  c->int_value = n;
  return c;
}

SYMB *mktmp(void){

  extern int next_tmp;

  SYMB *old_yylval = yylval.symb;
  SYMB *temp;
  char name[12];

  sprintf(name, "T%d", next_tmp++);
  mkname(table_ptr, name, T_TEMP);

  temp = yylval.symb;  
  yylval.symb = old_yylval;

  return temp;
}

int calculate_c(DIM_TYPE *dim_list, int w){

  int c;
  int base = 1000; //hardcode for testing purposes
  int rest;  
  int part1, part2;  
  int term, final_term = 1;
  DIM_TYPE *temp = dim_list;

  //because the dimension list is stored in reverse, we need
  //to start at end, and work our way forward through the dimensions
  while(temp->next != NULL)
    temp = temp->next;

  //single dimensional case
  if(temp->prev == NULL){
    rest = temp->low * w;
    //c = base - rest;
    c = rest;
    return c;
  }
  
  //multi dimensional case
  while(temp->prev != NULL){
    part1 = temp->low * temp->prev->node_elements + temp->prev->low;
    if(temp->prev->prev != NULL)
      part2 = temp->prev->prev->node_elements + temp->prev->prev->low;
    else
      part2 = 1;

    term = part1 * part2;    
    final_term = final_term * term;
    temp = temp->prev;
  }
  rest = final_term * w;
  
  //c = base - rest;
  c = rest;
  return c;
}
