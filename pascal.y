/********************************************************************/
/*                                                                  */
/*                                                                  */
/*                              PARSER                              */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/********************************************************************/

%{

#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "symbol.h"
#include "stack.h"
#include "tac.h"
#include "stack_interface.h"

SYMBTABLE *st;
SYMBTABLE *main_symbol_table;
SYMBTABLE *constant_symbol_table;
SYMBTABLE *table_pointer;
SYMBTABLE *table_ptr;

#define YYDEBUG 1

DIM_TYPE *dim_p;

PARAM_TYPE *parameter_list;
PARAM_TYPE *param;

SYMB            *t;
int              offset;
int              len;
stack            symbol_table_stack;
stack            offset_stack;
struct decl_type temp;

int executable_statement_mode = 0;
int parameter_mode = 0;
int param_count = 0;
int block_depth = 0;
int line_count = 1;
int c;
int elements;
int next_tmp = 0;
int next_label = 0;
TAC *program_tac = NULL;
TAC *p_tac1 = NULL;
TAC *p_tac2 = NULL;
int global_nesting_depth = 1;

static void yyerror(char *str){ fprintf(stderr, "pc: %s\n", str); }

%}

%union {
  SYMB      *symb;
  DECL_TYPE  decl_type;
  DIM_TYPE  *dim_type;
  TAC       *tac;
  ENODE     *enode;
};

%token AND ARRAY _BEGIN CASE CONST DIV DO DOWNTO ELSE END _FILE
%token FOR FORWARD FUNCTION GOTO IF IN LABEL MOD NIL NOT OF OR 
%token PACKED PROCEDURE PROGRAM RECORD REPEAT SET THEN TO TYPE UNTIL
%token VAR WHILE WITH INTEGER REAL NUM
%token ASSIGN NE GE LE DOTDOT print  

%token <symb> ID
%token <symb> NUM_INT
%token <symb> NUM_REAL
%token <symb> STRING

%type <symb> main_identifier_list
%type <symb> parameters
%type <symb> optional_parameters
%type <enode> variable
%type <enode> expression
%type <enode> simple_expression
%type <enode> function_expression_list
%type <enode> array_expression_list
%type <enode>  term
%type <enode>  factor
%type <enode> optional_arguments
%type <enode> argument_list
%type <decl_type> standard_type
%type <decl_type> type
%type <decl_type> declaration_identifier_list
%type <dim_type> dimension_declaration_list
%type <tac> statement
%type <tac> program
%type <tac> optional_declarations
%type <tac> declaration
%type <tac> declarations
%type <tac> subprogram_declaration
%type <tac> subprogram_declarations
%type <symb> subprogram_head
%type <tac> compound_statement
%type <tac> optional_statements
%type <tac> statement_list
%type <tac> procedure_statement

%%

program                 :  M1 PROGRAM ID '(' main_identifier_list ')' ';'
                           optional_declarations
                           subprogram_declarations
                           compound_statement
                           '.'
                        {
                           pop_table(&symbol_table_stack, &table_pointer);
                           len = strlen($3->name);			   
                           table_pointer->name = (char*)malloc(sizeof(char) * len + 1);
                           strcpy(table_pointer->name, $3->name);
                           table_pointer->name[len] = '\0';
                           pop_offset(&offset_stack, &offset);
                           table_pointer->width = offset;
			   $3->size = offset;
                           $3->token_type_1 = T_SELF;
                           $3->token_type_2 = T_PROGRAM;
                           $3->data_type = T_UNDEF;
			   $3->nesting_depth = 1;

			   TAC *temp1 = join_tac($8, $10);			   
			   TAC *temp2 = do_program($3, $5, temp1);
			   program_tac = join_tac(temp2, $9);
			   $$ = program_tac;
                         }
                         ;

M1                      :
                        {
                          init_stack(&symbol_table_stack);
                          init_stack(&offset_stack);
                          constant_symbol_table = mktable(NULL);
                          table_ptr = mktable(NULL);
                          main_symbol_table = table_ptr;
                          len = strlen("CONSTANT SYMBOLS");
                          constant_symbol_table->name = (char*)malloc(sizeof(char) * len + 1);
                          strcpy(constant_symbol_table->name, "CONSTANT SYMBOLS");
                          main_symbol_table->width = 0;
                          constant_symbol_table->width = 0;
                          push_table(&symbol_table_stack, table_ptr);
                          push_offset(&offset_stack, 0);
                          parameter_mode = 0;
                        } 
                        ;

main_identifier_list    :  ID
                        {
                          $1->token_type_1 = T_PARAM;
                          $1->token_type_2 = T_FILE;
                          $1->data_type = T_UNDEF;
                          $$ = $1;
                          main_symbol_table->parameters = 1;
                        }

                        |  main_identifier_list ',' ID
			{
			  $3->token_type_1 = T_PARAM;
			  $3->token_type_2 = T_FILE;
			  $3->data_type = T_UNDEF;
			  main_symbol_table->parameters = main_symbol_table->parameters + 1;
                        }
                        ;
                          
optional_declarations   :  declarations
                        {
			  $$ = $1;
                        }
                        |
                        {
			  $$ = NULL;
                        }
                        ;

declaration_identifier_list
                        :  ',' ID declaration_identifier_list
			{
			  if(parameter_mode == 1)
			    { $2->token_type_1 = T_PARAM; param_count++; }
			  else $2->token_type_1 = T_LOCAL;

			  if(parameter_mode == 1){
			    param = get_param();
			    param->param_symb = $2;
			    param->next = parameter_list;
			    parameter_list = param;			   
			  }

			  $$.data_type = $3.data_type;
			  $$.token_type_1 = $3.token_type_1;
			  $$.token_type_2 = $3.token_type_2;

			  $$.size = $2->size = $3.size;
			  $$.dimensions = $2->dimensions = $3.dimensions;
			  $$.dimension_list = $2->dimension_list = $3.dimension_list;
			  $2->line = line_count;

			  $2->token_type_2 = $3.token_type_2;

			  if($3.token_type_2 == T_ARRAY){
			    $2->token_type_2 = $3.token_type_2;
			    $2->c = $3.c;
			    $$.c = $3.c;
			    $2->size = $3.elements * $3.size;
			    $$.elements = $3.elements;
			  }
			  else {
			    $2->token_type_2 = T_VAR;
			    $2->size = $3.size;
			  }

			  $2->data_type = $3.data_type;
			  pop_offset(&offset_stack, &offset);
			  $2->offset = offset;
			  offset = offset + $2->size;
			  push_offset(&offset_stack, offset);

                         if(p_tac1 != NULL){
			    TAC* temp_tac = declare_var($2);
			    p_tac1 = join_tac(temp_tac, p_tac1);		    
			  }
			  else{
			    p_tac1 = declare_var($2);
			  }			
			}

                        |  ':' type
			{
			  $$ = $2;
			}
			;

declaration             :  ID declaration_identifier_list
                        {
                          if(parameter_mode == 1)
                            { $1->token_type_1 = T_PARAM; param_count++; }
                          else $1->token_type_1 = T_LOCAL;

                          if(parameter_mode == 1){
                            param = get_param();
                            param->param_symb = $1;
                            param->next = parameter_list;
                            parameter_list = param;
                          }

                          $1->line = line_count;
                          if($2.token_type_2 == T_ARRAY){
                            $1->token_type_2 = $2.token_type_2;
                            $1->c = $2.c;
                            $1->size = $2.elements * $2.size;
                          }

                          else {
                          $1->token_type_2 = T_VAR;
                          $1->offset = offset;
                          $1->size = $2.size;
                          }

                          $1->data_type = $2.data_type;
                          $1->dimensions = $2.dimensions;
                          $1->dimension_list = $2.dimension_list;

                          pop_offset(&offset_stack, &offset);
                          $1->offset = offset;
                          offset = offset + $1->size;
                          push_offset(&offset_stack, offset);

			  if(p_tac1 != NULL){
			    TAC* temp_tac = declare_var($1);		    
			    $$ = join_tac(temp_tac, p_tac1);		    
			    p_tac1 = NULL;			    
			  }
			  else{
			    $$ = declare_var($1);
			  }			  
                        }
                        ;

declarations            :  declarations VAR declaration ';'
                        {
			  $$ = join_tac($1, $3);			  
                        }
                        |  VAR declaration ';'
			{
			  $$ = $2;			  
			}
                        ;

type                    :  standard_type { $$ = $1; }
                        |  ARRAY dimension_declaration_list ']' OF standard_type
			{
			   $$.token_type_2 = T_ARRAY;
			   $$.data_type = $5.data_type;			   
			   $$.size = $5.size;
			   $$.elements = $2->node_elements;
			   DIM_TYPE *temp = $2;
			   while(temp->next != NULL){
			     temp = temp->next;
			     $$.elements *= temp->node_elements;                            
                            }			   
			   $$.dimensions = $2->dimension;
			   $$.dimension_list = $2; 
			   $$.c = calculate_c($2, $5.size);
                        }
                        ;

standard_type           :  INTEGER
                        {
                          $$.data_type = T_INT;
                          $$.size = SIZEOF_INT;
                          $$.dimensions = 0;
                          $$.dimension_list = NULL;
                        }

                        |  REAL
                        {
                          $$.data_type = T_REAL;
                          $$.size = SIZEOF_REAL;
                          $$.dimensions = 0;
                          $$.dimension_list = NULL;
                        }
                        ;

subprogram_declarations :  subprogram_declarations subprogram_declaration
                        {
			  $$ = join_tac($1, $2);			  
                        }
                        |
                        {			  
			  $$ = NULL;
                        }
                        ;

subprogram_declaration  :  subprogram_head optional_declarations subprogram_declarations compound_statement ';'
                        {
                          pop_table(&symbol_table_stack, &table_pointer);
                          pop_offset(&offset_stack, &offset);
                          table_pointer->width = offset;
			  
			  //make a function TAC
			  if($1->token_type_2 == T_FUNC){
			    TAC *temp1 = join_tac($2, $4);			    

			    //make the param tac list
			    PARAM_TYPE *temp2 = get_param();
			    temp2 = $1->parameter_list;
			    TAC *temp3 = NULL;
			    TAC *temp4 = NULL;
			    if(temp2 != NULL){

			      temp3 = (TAC*)mktac(TAC_ARG, (SYMB*)temp2->param_symb, NULL, NULL);
			      while(temp2->next != NULL){
				temp2 = temp2->next;
				temp4 = (TAC*)mktac(TAC_ARG, (SYMB*)temp2->param_symb, NULL, NULL);
				temp3 = join_tac(temp4, temp3);
			      }
			    }
			    TAC *temp5 = do_func($1, temp3, temp1);
			    $$ = join_tac(temp5, $3);
			    p_tac2 = NULL;
			  }

			  //make a procedure TAC
			  if($1->token_type_2 == T_PROC){
			    TAC *temp1 = join_tac($2, $4);			    

			    //make the param tac list
			    PARAM_TYPE *temp2 = get_param();
			    temp2 = $1->parameter_list;
			    TAC *temp3 = NULL;
			    TAC *temp4 = NULL;
			    if(temp2 != NULL){

			      temp3 = (TAC*)mktac(TAC_ARG, (SYMB*)temp2->param_symb, NULL, NULL);
			      while(temp2->next != NULL){
				temp2 = (PARAM_TYPE*)temp2->next;
				temp4 = (TAC*)mktac(TAC_ARG, (SYMB*)temp2->param_symb, NULL, NULL);
				temp3 = join_tac(temp4, temp3);
			      }
			    }
			    TAC *temp5 = do_proc($1, temp3, temp1);
			    $$ = join_tac(temp5, $3);
			    p_tac2 = NULL;
			  }
			  global_nesting_depth--;			  
                        }
                        ;

subprogram_head         :  FUNCTION ID N1 optional_parameters ':' standard_type ';'
                        {			   
			  $2->line = line_count;
			  $2->token_type_1 = T_SELF;
			  $2->token_type_2 = T_FUNC;
			  $2->data_type = $6.data_type;
			  $2->function_st_ptr = table_ptr;
			  table_ptr->name = $2->name;
			  $2->parameter_list = parameter_list;
			  $2->nesting_depth = global_nesting_depth;

			  //make the functions label at this time
			  TAC *label = (TAC*)mktac(TAC_LABEL, (SYMB*)make_label(next_label++), NULL, NULL);
			  $2->label = label;
			  //make a local symb for function return value
			  mkname(table_ptr, $2->name, T_RETURN);
			  SYMB *t = lookup(table_ptr, $2->name);			  
			  t->label = label;			  

			  $$ = $2;
                        }
                        |  PROCEDURE ID N1 optional_parameters ';'
                        {
			  $2->line = line_count;
			  $2->token_type_1 = T_SELF;
			  $2->token_type_2 = T_PROC;
			  $2->data_type = T_UNDEF;
			  $2->function_st_ptr = table_ptr;
			  table_ptr->name = $2->name;
			  $2->parameter_list = parameter_list;
			  $2->nesting_depth = global_nesting_depth;

			  TAC *label = (TAC*)mktac(TAC_LABEL, (SYMB*)make_label(next_label++), NULL, NULL);
			  $2->label = label;
			  $$ = $2; 
                        }
                        ;

N1                      :
                        {
                          table_ptr = mktable(table_ptr);
                          push_table(&symbol_table_stack, table_ptr);
                          push_offset(&offset_stack, offset);
			  global_nesting_depth++;
                          parameter_mode = 0;			  
                        }
                        ;

optional_parameters     :  '(' N2 parameters ')'
                        {
			  top_table(&symbol_table_stack, &table_pointer);
			  table_pointer->parameters = param_count;
			  table_pointer->parameter_list = parameter_list;
			  parameter_mode = 0;			  
			  $$ = $3;
                        }
                        |
                        {
			  $$ = NULL;
                        }
                        ;

N2                      :  
                        { parameter_mode = 1; param_count = 0; parameter_list = NULL; }

parameters              :  parameters ';' declaration
                        { 			 
			  $$ = $1;
                        } 
                        |  declaration
			{			  
			  $$ = $1->a.var; 
			}                        
                        ;

compound_statement      :  _BEGIN M2 optional_statements END M3
                        {
			  $$ = $3;
                        }
                        ;

M2                      :  { block_depth++; executable_statement_mode = 1; }
M3                      :  { if(--block_depth == 0) executable_statement_mode = 0; }

optional_statements     :  statement_list
                        | 
                        {
			  $$ = NULL;
                        }
                        ;

dimension_declaration_list 
                        :  dimension_declaration_list ',' NUM_INT DOTDOT NUM_INT
			{
			  DIM_TYPE *dim = (DIM_TYPE*)safe_malloc(sizeof(DIM_TYPE));
			  $3->int_value = atoi($3->name);
			  $5->int_value = atoi($5->name);
			  dim->high = $5->int_value;
			  dim->low = $3->int_value;
			  dim->prev = NULL;
			  dim->next = $$;
			  dim->node_elements = dim->high - dim->low + 1;
			  
			  $$->prev = dim;
			  $$ = dim;
			  $$->dimension = $$->next->dimension + 1;
			}
                        | '[' NUM_INT DOTDOT NUM_INT
			{
			  DIM_TYPE *dim = (DIM_TYPE*)malloc(sizeof(DIM_TYPE));
			  dim->dimension = 1;
			  $2->int_value = atoi($2->name);
                          $4->int_value = atoi($4->name);
			  dim->high = $4->int_value;
			  dim->low = $2->int_value;
			  dim->prev = NULL;
			  dim->next = NULL;
			  dim->node_elements = dim->high - dim->low + 1;
			  $$ = dim;
			}
			;

statement_list          :  statement
                        |  statement_list ';' statement
                        {
			  $$ = join_tac($1, $3);
                        }
                        ;

statement               :  expression ASSIGN expression
                        {
			   if($1->offset == NULL)
			     $$ = do_assign($1->res, $3);
			   else				     
			     $$ = do_assign_offset($1, $3);
                        }
                        |  procedure_statement
                        |  compound_statement
                        |  IF expression THEN statement ELSE statement
			{
			  $$ = do_test($2, $4, $6);
			}
                        |  WHILE expression DO statement 
			{
			  $$ = do_while($2, $4);
			}
                        |  print expression
			{
			  $$ = do_print($2);
			}
                        ;



procedure_statement     :  ID optional_arguments
                        {			  
			  $$ = call_proc($1, $2);			  
                        }
                        ;

optional_arguments      :  argument_list ')'
                        {
			  $$ = $1;
                        }
                        ;

argument_list           :  argument_list ',' expression
                        {
			  $1->next = $3;
			  $$ = $1;
                        }
                        |  '(' expression
                        {
			  $$ = $2;
                        }
			;

function_expression_list :  expression
                        |  function_expression_list ',' expression
                        {
			   $1->next = $3;
			   $$ = $1;
                        }
                        ;

array_expression_list   :  array_expression_list ',' expression
                        {			  
			  int m = $1->ndim + 1;
			  int l = limit($1->array, m);
			  SYMB *limit = (SYMB*)safe_malloc(sizeof(SYMB));
			  limit->int_value = l;
			  limit->token_type_2 = T_INT;

			  TAC *temp1 = (TAC*)mktac(TAC_VAR, (SYMB*)mktmp(), NULL, NULL);
			  TAC *temp2 = (TAC*)mktac(TAC_MUL, temp1->a.var, $1->res, limit);
			  TAC *temp3 = (TAC*)mktac(TAC_ADD, temp1->a.var, temp1->a.var, $3->res);
			  TAC *temp4 = join_tac(temp1, temp2);
			  TAC *temp5 = join_tac(temp4, temp3);			  
			  $$ = make_enode(NULL, temp1->a.var, temp5);
			  $$->array = $1->array;
			  $$->ndim = m;			   
                        }
                        |  ID '[' expression
                        {
			  $$ = make_enode(NULL, $3->res, $3->tac);
			  $$->array = $1;			  
			  $$->ndim = 1;
                        }
                        ;

expression              :  simple_expression
                        |  variable
                        {
			  if($1->offset == NULL)
			    $$ = $1;
			  else{
			    TAC *temp1 = (TAC*)mktac(TAC_VAR, (SYMB*)mktmp(), NULL, NULL);
			    TAC *temp2 = (TAC*)mktac(TAC_OFFSET, temp1->a.var, $1->res, $1->offset);
			    TAC *temp3 = (TAC*)mktac(TAC_VAR, (SYMB*)mktmp(), NULL, NULL);
			    TAC *temp4 = (TAC*)mktac(TAC_COPY, temp3->a.var, temp1->a.var, NULL);
			    TAC *temp5 = join_tac(temp1, temp2);
			    TAC *temp6 = join_tac(temp5, temp3);
			    TAC *temp7 = join_tac(temp6, temp4);
			    TAC *temp8 = join_tac($1->tac, temp7);
			    $$ = make_enode(NULL, temp3->a.var, temp8);
			    $$->offset = $1->offset;			    
			  }
                        }  
                        |  simple_expression '=' simple_expression
                        {
			  $$ = do_relop(TAC_EQUAL, $1, $3);
                        }
                        |  simple_expression NE simple_expression
			{
			  $$ = do_relop(TAC_NE, $1, $3);
			}
                        |  simple_expression GE simple_expression
			{
			  $$ = do_relop(TAC_GE, $1, $3);
			}
                        |  simple_expression LE simple_expression
			{
			  $$ = do_relop(TAC_LE, $1, $3);
			}
                        |  simple_expression '<' simple_expression
			{
			  $$ = do_relop(TAC_LESSTHAN, $1, $3);
			}
                        |  simple_expression '>' simple_expression
			{
			  $$ = do_relop(TAC_GRTRTHAN, $1, $3);
			}
                        ;  

variable                :  ID
                        {
			  $$ = make_enode(NULL, $1, NULL);
			  $$->offset = NULL;
                        }
                        |  array_expression_list ']'
			{
			  TAC *temp1 = (TAC*)mktac(TAC_VAR, (SYMB*)mktmp(), NULL, NULL);
			  ENODE *final = make_enode(NULL, temp1->a.var, NULL);
			  $$ = final;		  

			  SYMB *c = (SYMB*)safe_malloc(sizeof(SYMB));
			  c->token_type_2 = T_INT;
			  c->int_value = $1->array->c;			  
			  TAC *temp2 = (TAC*)mktac(TAC_COPY, $$->res, c, NULL);

			  SYMB *width = (SYMB*)safe_malloc(sizeof(SYMB));			  
			  if($1->array->data_type == T_INT){
			    width->token_type_2 = T_INT;
			    width->int_value = SIZEOF_INT;
			  }
			  if($1->array->data_type == T_REAL){
			    width->token_type_2 = T_REAL;
			    width->dbl_value = SIZEOF_REAL;
			  }
			  TAC *temp3 = (TAC*)mktac(TAC_VAR, (SYMB*)mktmp(), NULL, NULL);
			  TAC *temp4 = (TAC*)mktac(TAC_MUL, temp3->a.var, $1->res, width);
			  TAC *temp5 = (TAC*)mktac(TAC_VAR, (SYMB*)mktmp(), NULL, NULL);	  
			  TAC *temp6 = (TAC*)mktac(TAC_COPY, temp5->a.var, temp4->a.var, NULL);
			  TAC *temp7 = join_tac(temp1, temp2);			  
			  TAC *temp8 = join_tac(temp7, temp3);			  
			  TAC *temp9 = join_tac(temp8, temp4);			  
			  TAC *temp10 = join_tac(temp9, temp5);
			  TAC *temp11 = join_tac(temp10, temp6);
			  TAC *temp12 = join_tac($1->tac, temp11);
			  $$->offset = temp5->a.var;			  
			  $$->tac = temp12;			  
			}
                        ;                        

simple_expression       :  term
                        |  '+' term
                        {
			  $$ = do_unary(TAC_POS, $2, table_pointer);
                        }
                        |  '-' term
			{			  
			  $$ = do_unary(TAC_NEG, $2, table_pointer);
			}
                        |  simple_expression '+' term
                        {			  
			  $$ = do_bin(TAC_ADD, $1, $3, table_pointer);
                        }
                        |  simple_expression '-' term
			{			  
			  $$ = do_bin(TAC_SUB, $1, $3, table_pointer);
			}
                        |  simple_expression OR term
			{
			  $$ = do_bin(TAC_OR, $1, $3, table_pointer);
			}
                        ;

term                    :  factor
                        |  term '*' factor
                        {
			  $$ = do_bin(TAC_MUL, $1, $3, table_pointer);
                        }
                        |  term '/' factor
			{
			  $$ = do_bin(TAC_DIV, $1, $3, table_pointer);
			}
                        |  term DIV factor
			{
			  $$ = do_bin(TAC_TDIV, $1, $3, table_pointer);	  
			}
                        |  term MOD factor
			{
			  $$ = do_bin(TAC_MOD, $1, $3, table_pointer);
			}
                        |  term AND factor
			{
			  $$ = do_bin(TAC_AND, $1, $3, table_pointer);
			}
                        ;

factor                  :  ID    
                        {
			  $$ = make_enode(NULL, $1, NULL);
                        }                     
                        |  ID '(' function_expression_list ')'
			{
			  $$ = call_func($1, $3);
			}
                        |  NUM_INT
                        {
			  $$ = make_enode(NULL, $1, NULL);			  
                        }
                        |  NUM_REAL
                        {
			  $$ = make_enode(NULL, $1, NULL);
                        }
                        |  '(' expression ')'
			{
			  $$ = $2;
			}
                        |  NOT factor
			{
			  $$ = do_unary(TAC_NOT, $2, table_pointer);
			}
                        ;

%%

