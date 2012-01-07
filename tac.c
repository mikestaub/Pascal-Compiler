/********************************************************************/
/*                                                                  */
/*                                                                  */
/*                       TAC GENERATOR                              */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/********************************************************************/

#include "tac.h"
#include <stdlib.h>

TAC *mktac(int op, SYMB *a, SYMB *b, SYMB *c){

  TAC *t = (TAC*)safe_malloc(sizeof(TAC));

  t->next = NULL;
  t->prev = NULL;
  t->op = op;
  t->a.var = a;
  t->b.var = b;
  t->c.var = c;

  return t;
}

TAC *do_assign(SYMB *var, ENODE *expr){

  TAC *code;

  if((var->token_type_2 != T_VAR) && (var->token_type_2 != T_RETURN)){
    printf("assignment to non_variable, line: %d\n", var->line);    
    //exit(1);
  }

  code = mktac(TAC_COPY, var, expr->res, NULL);
  code->prev = expr->tac;

  return code;
}

ENODE *do_bin(int binop, ENODE *expr1, ENODE *expr2, SYMBTABLE* table){

  TAC *temp;
  TAC *res;
  
  //case: both expr are int
  if((expr1->res->token_type_2 == T_INT) && (expr2->res->token_type_2 == T_INT)){

    switch(binop)
      {
      case TAC_ADD:	
	expr1->res->int_value = expr1->res->int_value + expr2->res->int_value;	
	break;
      case TAC_SUB:
	expr1->res->int_value = expr1->res->int_value - expr2->res->int_value;	
	break;
      case TAC_MUL:
	expr1->res->int_value = expr1->res->int_value * expr2->res->int_value;	
	break;
      case TAC_DIV:	
	expr1->res->int_value = expr1->res->int_value / expr2->res->int_value;	
	break;
      case TAC_TDIV:	
	expr1->res->int_value = (int)(expr1->res->int_value / expr2->res->int_value) % 1;	
	break;
      case TAC_MOD:
	expr1->res->int_value = expr1->res->int_value % expr2->res->int_value;  
	break;
      //these 2 are boolean operators and only expect 1 or 0 as inputs	
      case TAC_AND:
	{
	  if(expr1->res->int_value && expr2->res->int_value)
	    expr1->res->int_value = 1;
	  else
	    expr1->res->int_value = 0;
	  break;
	}
      case TAC_OR:
	  if(expr1->res->int_value || expr2->res->int_value)
	    expr1->res->int_value = 1;
	  else
	    expr1->res->int_value = 0;	
	break;
      }
    sprintf(expr1->res->name, "%d", expr1->res->int_value);
    return expr1;
  }    

  //case: both expr are real
  if((expr1->res->token_type_2 == T_REAL) && (expr2->res->token_type_2 == T_REAL)){

    switch(binop)
      {
      case TAC_ADD:
	expr1->res->dbl_value = expr1->res->dbl_value + expr2->res->dbl_value;
	
	break;
      case TAC_SUB:
	expr1->res->dbl_value = expr1->res->dbl_value - expr2->res->dbl_value;
	
	break;
      case TAC_MUL:
	expr1->res->dbl_value = expr1->res->dbl_value * expr2->res->dbl_value;
	
	break;
      case TAC_DIV:
	expr1->res->dbl_value = expr1->res->dbl_value / expr2->res->dbl_value;
	
	break;      
      case TAC_TDIV:	
	expr1->res->dbl_value = (int)(expr1->res->dbl_value / expr2->res->dbl_value) % 1;
	
	break;
      }
    sprintf(expr1->res->name, "%lf", expr1->res->int_value);
    return expr1;
  }

  //case: 1st expr is int, 2nd is real - promote 1st
  if((expr1->res->token_type_2 == T_INT) && (expr2->res->token_type_2 == T_REAL)){

    expr1->res->dbl_value = expr1->res->int_value;

    switch(binop)
      {
      case TAC_ADD:
	expr1->res->dbl_value = expr1->res->dbl_value + expr2->res->dbl_value;
	
	break;
      case TAC_SUB:
	expr1->res->dbl_value = expr1->res->dbl_value - expr2->res->dbl_value;
	
	break;
      case TAC_MUL:
	expr1->res->dbl_value = expr1->res->dbl_value * expr2->res->dbl_value;
	
	break;
      case TAC_DIV:
	expr1->res->dbl_value = expr1->res->dbl_value / expr2->res->dbl_value;
	
	break;       
      case TAC_TDIV:	
	expr1->res->dbl_value = (int)(expr1->res->dbl_value / expr2->res->dbl_value) % 1;
	
	break; 
      }
    sprintf(expr1->res->name, "%lf", expr1->res->int_value);
    return expr1;    
  }

  //case: 1st expr is real, 2nd is int - promote 2nd
  if((expr1->res->token_type_2 == T_REAL) && (expr2->res->token_type_2 == T_INT)){

    expr2->res->dbl_value = expr2->res->int_value;

    switch(binop)
      {
     case TAC_ADD:
	expr1->res->dbl_value = expr1->res->dbl_value + expr2->res->dbl_value;
	
	break;
      case TAC_SUB:
	expr1->res->dbl_value = expr1->res->dbl_value - expr2->res->dbl_value;
	
	break;
      case TAC_MUL:
	expr1->res->dbl_value = expr1->res->dbl_value * expr2->res->dbl_value;
	
	break;
      case TAC_DIV:
	expr1->res->dbl_value = expr1->res->dbl_value / expr2->res->dbl_value;
	
	break;
      case TAC_TDIV:	
	expr1->res->dbl_value = (int)(expr1->res->dbl_value / expr2->res->dbl_value) % 1;
	
	break;      
      }
    sprintf(expr1->res->name, "%lf", expr1->res->int_value);
    return expr1;
  }

  //expressions were not constants, so create a TAC for a binary operator
  temp = mktac(TAC_VAR, (SYMB*)mktmp(), NULL, NULL);
  temp->prev = join_tac(expr1->tac, expr2->tac);
  res = mktac(binop, temp->a.var, expr2->res, expr1->res);
  res->prev = temp;

  expr1->res = temp->a.var;
  expr1->tac = res;

  return expr1;   
}

TAC *join_tac(TAC *tac1, TAC *tac2){

  TAC *t;

  if(tac1 == NULL)
    return tac2;

  if(tac2 == NULL)
    return tac1;

  t = tac2;

  while(t->prev != NULL)
    t = t->prev;

  t->prev = tac1;
  return tac2;

}

ENODE *make_enode(ENODE *next, SYMB *res, TAC *code){
  
  ENODE *temp = (ENODE*)safe_malloc(sizeof(ENODE));
  temp->next = next;
  temp->res = res;
  temp->tac = code;

  return temp;
}
  
TAC *declare_var(SYMB* var){
  
  return mktac(TAC_VAR, var, NULL, NULL);
}

TAC *do_program(SYMB *func, SYMB *args, TAC *code){

  TAC *tlist;
  TAC *tlab;
  TAC *tbegin;
  TAC *tend;
  TAC *tidlist;

  extern int next_label;

  tlab    =  mktac(TAC_LABEL, make_label(next_label++), NULL, NULL);
  tbegin  =  mktac(TAC_BEGINPROG, func, NULL, NULL);
  tend    =  mktac(TAC_ENDPROG, NULL, NULL, NULL);
  tidlist =  mktac(TAC_MAINIDLIST, args, NULL, NULL);

  tbegin->prev = tlab;
  
  code = join_tac(tidlist, code);  
  tend->prev = join_tac(tbegin, code);

  return tend;
}

TAC *do_func(SYMB *func, TAC *args, TAC *code){

  TAC *tlist;
  TAC *tlab;
  TAC *tbegin;
  TAC *tend;  

  extern int next_label;

  tlab   = (TAC*)func->label;
  tbegin = mktac(TAC_BEGINFUNC, NULL, NULL, NULL);
  tend   = mktac(TAC_ENDFUNC, NULL, NULL, NULL);  

  tbegin->prev = tlab;
  code = join_tac(args, code);  
  tend->prev = join_tac(tbegin, code);
  
  tlist = (TAC*)func->label;

  while(tlist != NULL){

    TAC *tnext = tlist->b.lab;
    tlist->b.lab = tlab;
    tlist = tnext;
  }

  func->label = tlab;  

  return tend;
}

TAC *do_proc(SYMB *proc, TAC *args, TAC *code){

  TAC *tlist;
  TAC *tlab;
  TAC *tbegin;
  TAC *tend;  

  extern int next_label;

  tlab   = (TAC*)proc->label;
  tbegin = mktac(TAC_BEGINPROC, NULL, NULL, NULL);
  tend   = mktac(TAC_ENDPROC, NULL, NULL, NULL);  

  tbegin->prev = tlab;  
  code = join_tac(args, code);  
  tend->prev = join_tac(tbegin, code);  

  return tend;
}

SYMB *make_label(int l){  

  SYMB *s = (SYMB*)safe_malloc(sizeof(SYMB));
  s->token_type_2 = T_LABEL;
  s->int_value = l;
  return s;
}

ENODE *do_unary(int unop, ENODE *expr, SYMBTABLE *table){

  TAC *temp;
  TAC *res;

  if(expr->res->token_type_2 == T_INT){

    switch(unop){

      case TAC_NEG:
	{
	if(expr->res->int_value > 0)
          expr->res->int_value *= -1;	
        break;
	}
      case TAC_POS:
	{
	if(expr->res->int_value < 0)
          expr->res->int_value *= -1;	
        break;
	}
      case TAC_NOT:
	if(expr->res->int_value == 0)
	  expr->res->int_value = 1;
	else
	  expr->res->int_value = 0;	
	break;
    }
    return expr;
  }

  if(expr->res->token_type_2 == T_REAL){

    switch(unop){

      case TAC_NEG:
	{
	if(expr->res->dbl_value > 0)
          expr->res->dbl_value *= -1;	
        break;
	}
      case TAC_POS:
	{
	if(expr->res->dbl_value < 0)
          expr->res->dbl_value *= -1;	
        break;
	}
      case TAC_NOT:
	if(expr->res->dbl_value == 0)
	  expr->res->int_value = 1;
	else
	  expr->res->int_value = 0;	
	break;
    }
    return expr;
  }

  temp = mktac(TAC_VAR, (SYMB*)mktmp(), NULL, NULL);
  temp->prev = expr->tac;
  res = mktac(unop, temp->a.var, NULL, expr->res);
  res->prev = temp;

  expr->res = temp->a.var;
  expr->tac = res;

  return expr;
}

TAC *do_test(ENODE *expr, TAC *stmt1, TAC *stmt2){
  
  extern int next_label;

  TAC *label1 = mktac(TAC_LABEL, (SYMB*)make_label(next_label++), NULL, NULL);
  TAC *label2 = mktac(TAC_LABEL, (SYMB*)make_label(next_label++), NULL, NULL);
  TAC *code1  = mktac(TAC_IFZ, (SYMB*)label1, expr->res, NULL);
  TAC *code2  = mktac(TAC_GOTO, (SYMB*)label2, NULL, NULL);

  code1->prev  = expr->tac;
  code1        = join_tac(code1, stmt1);
  code2->prev  = code1;
  label1->prev = code2;
  label1       = join_tac(label1, stmt2);
  label2->prev = label1;

  return label2;
}

TAC *do_if(ENODE *expr, TAC *stmt){

  extern int next_label;

  TAC *label = mktac(TAC_LABEL, make_label(next_label++), NULL, NULL);
  TAC *code  = mktac(TAC_IFZ, (SYMB*)label, expr->res, NULL);

  code->prev  = expr->tac;
  code        = join_tac(code, stmt);
  label->prev = code;

  return label;
}

TAC *do_while( ENODE *expr, TAC *stmt ){

  extern int next_label;

  TAC *label = mktac( TAC_LABEL, make_label(next_label++), NULL, NULL);
  TAC *code  = mktac( TAC_GOTO, (SYMB *)label, NULL, NULL);

  code->prev = stmt;
  
  return join_tac(label, do_if(expr, code));
}

ENODE *call_func(SYMB *func, ENODE *arglist){

  ENODE *alt;
  SYMB *res;
  TAC *code;
  TAC *temp;

  if((func->data_type != T_UNDEF) && (func->token_type_2 != T_FUNC)){
    error("function declared other than function");
    return NULL;
  }

  res = (SYMB*)mktmp();
  code = mktac(TAC_VAR, res, NULL, NULL);

  for(alt = arglist; alt != NULL; alt = alt->next)
    code = join_tac(code, alt->tac);

  while(arglist != NULL){

    temp = mktac(TAC_ARG, arglist->res, NULL, NULL);
    temp->prev = code;
    code = temp;

    alt = arglist->next;    
    arglist = alt;
  }

  temp = mktac(TAC_CALL, res, (SYMB*)func->label, NULL);
  temp->prev = code;
  code = temp;

  if(func->data_type == T_UNDEF)
    func->label = code;

  return make_enode(NULL, res, code);
}

ENODE *do_relop(int relop, ENODE *expr1, ENODE *expr2) {

  TAC *temp;
  TAC *res;
  
  if((expr1->res->token_type_1 == T_CONSTANT) && (expr2->res->token_type_1 == T_CONSTANT)){
    //both expressions are ints
    if((expr1->res->token_type_2 == T_INT) && (expr2->res->token_type_2 == T_INT)){   
      switch(relop) 
	{
	case TAC_EQUAL:
	  if (expr1->res->int_value == expr2->res->int_value) expr1->res->int_value = 1;
	  else expr1->res->int_value = 0;
	  break;
	case TAC_LESSTHAN:
	  if (expr1->res->int_value < expr2->res->int_value) expr1->res->int_value = 1;
	  else expr1->res->int_value = 0;
	  break;
	case TAC_GRTRTHAN:
	  if (expr1->res->int_value > expr2->res->int_value) expr1->res->int_value = 1;
	  else expr1->res->int_value = 0;
	  break;
	case TAC_NE:
	  if (expr1->res->int_value != expr2->res->int_value) expr1->res->int_value = 1;
	  else expr1->res->int_value = 0;
	  break;
	case TAC_LE:
	  if (expr1->res->int_value <= expr2->res->int_value) expr1->res->int_value = 1;
	  else expr1->res->int_value = 0;
	  break;
	case TAC_GE:
	  if (expr1->res->int_value >= expr2->res->int_value) expr1->res->int_value = 1;
	  else expr1->res->int_value = 0;
	  break; 
	}
      sprintf(expr1->res->name, "%d", expr1->res->int_value);
      return expr1;
    }
    //both are real
    if((expr1->res->token_type_2 == T_REAL) && (expr2->res->token_type_2 == T_REAL)){   
      switch(relop) 
	{
	case TAC_EQUAL:
	  if (expr1->res->dbl_value == expr2->res->dbl_value) expr1->res->int_value = 1;
	  else expr1->res->int_value = 0;
	  break;
	case TAC_LESSTHAN:
	  if (expr1->res->dbl_value < expr2->res->dbl_value) expr1->res->int_value = 1;
	  else expr1->res->int_value = 0;
	  break;
	case TAC_GRTRTHAN:
	  if (expr1->res->dbl_value > expr2->res->dbl_value) expr1->res->int_value = 1;
	  else expr1->res->int_value = 0;
	  break;
	case TAC_NE:
	  if (expr1->res->dbl_value != expr2->res->dbl_value) expr1->res->int_value = 1;
	  else expr1->res->int_value = 0;
	  break;
	case TAC_LE:
	  if (expr1->res->dbl_value <= expr2->res->dbl_value) expr1->res->int_value = 1;
	  else expr1->res->int_value = 0;
	  break;
	case TAC_GE:
	  if (expr1->res->dbl_value >= expr2->res->dbl_value) expr1->res->int_value = 1;
	  else expr1->res->int_value = 0;
	  break; 
	}
      sprintf(expr1->res->name, "%lf", expr1->res->int_value);
      return expr1;
    }
    //first was real, second was int, promote second
    if((expr1->res->token_type_2 == T_REAL) && (expr2->res->token_type_2 == T_INT)){
      expr2->res->dbl_value = expr2->res->int_value;
      switch(relop) 
	{
	case TAC_EQUAL:
	  if (expr1->res->dbl_value == expr2->res->dbl_value) expr1->res->int_value = 1;
	  else expr1->res->int_value = 0;
	  break;
	case TAC_LESSTHAN:
	  if (expr1->res->dbl_value < expr2->res->dbl_value) expr1->res->int_value = 1;
	  else expr1->res->int_value = 0;
	  break;
	case TAC_GRTRTHAN:
	  if (expr1->res->dbl_value > expr2->res->dbl_value) expr1->res->int_value = 1;
	  else expr1->res->int_value = 0;
	  break;
	case TAC_NE:
	  if (expr1->res->dbl_value != expr2->res->dbl_value) expr1->res->int_value = 1;
	  else expr1->res->int_value = 0;
	  break;
	case TAC_LE:
	  if (expr1->res->dbl_value <= expr2->res->dbl_value) expr1->res->int_value = 1;
	  else expr1->res->int_value = 0;
	  break;
	case TAC_GE:
	  if (expr1->res->dbl_value >= expr2->res->dbl_value) expr1->res->int_value = 1;
	  else expr1->res->int_value = 0;
	  break; 
	}
      sprintf(expr1->res->name, "%lf", expr1->res->int_value);
      return expr1;
    }
    //first was an int, second was a real, promote first
    if((expr1->res->token_type_2 == T_INT) && (expr2->res->token_type_2 == T_REAL)){   
      expr1->res->dbl_value = expr1->res->int_value;
      switch(relop) 
	{
	case TAC_EQUAL:
	  if (expr1->res->dbl_value == expr2->res->dbl_value) expr1->res->int_value = 1;
	  else expr1->res->int_value = 0;
	  break;
	case TAC_LESSTHAN:
	  if (expr1->res->dbl_value < expr2->res->dbl_value) expr1->res->int_value = 1;
	  else expr1->res->int_value = 0;
	  break;
	case TAC_GRTRTHAN:
	  if (expr1->res->dbl_value > expr2->res->dbl_value) expr1->res->int_value = 1;
	  else expr1->res->int_value = 0;
	  break;
	case TAC_NE:
	  if (expr1->res->dbl_value != expr2->res->dbl_value) expr1->res->int_value = 1;
	  else expr1->res->int_value = 0;
	  break;
	case TAC_LE:
	  if (expr1->res->dbl_value <= expr2->res->dbl_value) expr1->res->int_value = 1;
	  else expr1->res->int_value = 0;
	  break;
	case TAC_GE:
	  if (expr1->res->dbl_value >= expr2->res->dbl_value) expr1->res->int_value = 1;
	  else expr1->res->int_value = 0;
	  break; 
	}
      sprintf(expr1->res->name, "%lf", expr1->res->int_value);
      return expr1;
    }
  }
  //one of the expressions was a variable, make a temp
  temp = mktac(TAC_VAR, (SYMB*)mktmp(), NULL, NULL);
  temp->prev = join_tac(expr1->tac, expr2->tac);
  res = mktac(relop, temp->a.var, expr2->res, expr1->res);
  res->prev = temp;

  expr1->res = temp->a.var;
  expr1->tac = res;

  return expr1;
}

TAC *do_assign_offset(ENODE *expr1, ENODE *expr2){

  TAC *code;
  
  code = mktac(TAC_COPY, expr1->res, expr2->res, NULL);  
  TAC *temp = join_tac(expr1->tac, expr2->tac);
  code->prev = temp;

  return code;  
}

int limit(SYMB *array, int m){

  int i;

  DIM_TYPE *temp = array->dimension_list;

  while(temp->next != NULL)
    temp = temp->next;

  for(i = 1; i < m; i++)
    temp = temp->prev;

  int elements = temp->node_elements;

  return elements;
}

TAC *call_proc(SYMB *proc, ENODE *arglist){

  ENODE *alt;
  SYMB *res;
  TAC *code = NULL;
  TAC *temp;
  
  if((proc->data_type != T_UNDEF) && (proc->token_type_2 != T_PROC)){
    error("procedure declared other than procedure");
    return NULL;
  }

  for(alt = arglist; alt != NULL; alt = alt->next)
    code = join_tac(code, alt->tac);

  while(arglist != NULL){    
    temp = mktac(TAC_ARG, arglist->res, NULL, NULL);
    temp->prev = code;
    code = temp;

    alt = arglist->next;    
    arglist = alt;
  }

  temp = mktac(TAC_CALL, NULL, (SYMB*)proc->label, NULL);
  temp->prev = code;
  code = temp;

  if(proc->data_type == T_UNDEF)
    proc->label = code;

  return code;
}

TAC *do_print(ENODE *expr){

  TAC *code;

  code = mktac(TAC_PRINT, expr->res, NULL, NULL);
  code->prev = expr->tac;

  return code;
}
