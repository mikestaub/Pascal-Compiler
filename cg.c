/********************************************************************/
/*                                                                  */
/*                                                                  */
/*                      CODE GENERATOR                              */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/********************************************************************/

#include "cg.h"

void cg(TAC *t1){

  TAC *tls = init_cg(t1);

  printf("    .file \"%s\"\n", input_file_name);
  printf(".PRINTF_ARG:\n");
  printf("    .string %s\n", M_PRINTF_ARG);
  printf("    .text\n");
  printf(".globl main\n\n"); //entry point
  printf("    .type main, @function\n\n");
  printf("main:\n");

  tls = tls->next; //always take off the first label for main

  for(; tls != NULL; tls = tls->next){
    printf("/*");
    print_tac(tls);
    printf("*/\n");
    cg_instruction(tls);
  }
}

TAC *init_cg(TAC *t1){

  int r;
  TAC *c;
  TAC *p;

  for(r = 0; r < R_MAX; r++)
    rdesc[r].name = NULL;

  insert_desc(0, mkconst(0), UNMODIFIED);

  tos = VAR_OFF;
  next_arg = 0;

  c = NULL;
  p = t1;

  //reverse the TAC list
  while(p != NULL){
    p->next = c;
    c = p;
    p = p->prev;
  }

  return c;
}

void cg_instruction(TAC *c){

  switch(c->op)
    {

    case TAC_UNDEF:
      error("cannot translate TAC_UNDEF");
      return;

    case TAC_ADD:
      cg_binop(TAC_ADD, c->a.var, c->b.var, c->c.var);
      return;

    case TAC_SUB:
      cg_binop(TAC_SUB, c->a.var, c->b.var, c->c.var);
      return;

    case TAC_MUL:
      cg_binop(TAC_MUL, c->a.var, c->b.var, c->c.var);
      return;

    case TAC_DIV:
      cg_binop(TAC_DIV, c->a.var, c->b.var, c->c.var);
      return;

    case TAC_NEG:
      cg_unop(TAC_NEG, c->a.var, c->c.var);
      return;

    case TAC_COPY:
      cg_copy(c->a.var, c->b.var);
      return;

    case TAC_GOTO:
      cg_cond("GOTO", NULL, c->a.lab->a.var->int_value);
      return;

    case TAC_IFZ:
      cg_cond("IFZ", c->b.var, c->a.lab->a.var->int_value);
      return;

    case TAC_IFNZ:
      cg_cond("IFNZ", c->b.var, c->a.lab->a.var->int_value);
      return;

    case TAC_ARG:
      cg_arg(c->a.var);
      return;

    case TAC_CALL:
      cg_call(c->b.lab->a.var->int_value, c->a.var);
      return;

    case TAC_RETURN:
      cg_return(c->a.var);
      return;

    case TAC_MOD:
      cg_binop(TAC_MOD, c->a.var, c->b.var, c->c.var);
      return;

    case TAC_AND:
      cg_binop(TAC_AND, c->a.var, c->b.var, c->c.var);
      return;

    case TAC_OR:
      cg_binop(TAC_OR, c->a.var, c->b.var, c->c.var);
      return;

    case TAC_NOT:
      cg_unop(TAC_NOT, c->a.var, c->c.var);
      return;

    case TAC_TDIV:
      cg_binop(TAC_TDIV, c->a.var, c->b.var, c->c.var);
      return;

    case TAC_POS:
      cg_unop(TAC_POS, c->a.var, c->c.var);
      return;

    case TAC_LABEL:
      {
	flush_all();
	printf("L%d:\n", c->a.var->int_value);
	return;
      }

    case TAC_VAR:
      {      
	c->a.var->offset = tos;
	tos += c->a.var->size;
	return;
      }

    case TAC_BEGINFUNC:
      tos = VAR_OFF;
      printf("\n");
      return;

    case TAC_ENDFUNC:
      cg_return(NULL);
      return;

    case TAC_BEGINPROC:
      
      return;

    case TAC_ENDPROC:
      cg_return(NULL);
      return;

    case TAC_BEGINPROG:
      {
	printf("    pushl %s\n", M_EBP); //push the base pointer
	printf("    movl %s, %s\n", M_ESP, M_EBP); //set the stack pointer to the base pointer
	printf("    movl $0, -4(%s)\n", M_EBP); //set first static link to zero
	printf("    subl $%d, %s\n", c->a.var->size + 4, M_ESP);
	return;
      }

    case TAC_MAINIDLIST:
      {      
	return;
      }

    case TAC_ENDPROG:
      {
	printf("    leave\n");
	printf("    ret\n");      
	return;
      }

    case TAC_OFFSET:
      cg_offset(c->a.var, c->b.var, c->c.var);
      return;

    case TAC_LESSTHAN:
      cg_relop("LESSTHAN", c->a.var, c->b.var, c->c.var);
      return;

    case TAC_GRTRTHAN:
      cg_relop("GRTRTHAN", c->a.var, c->b.var, c->c.var);
      return;

    case TAC_NE:
      cg_relop("NE", c->a.var, c->b.var, c->c.var);
      return;

    case TAC_LE:
      cg_relop("LE", c->a.var, c->b.var, c->c.var);
      return;

    case TAC_GE:
      cg_relop("GE", c->a.var, c->b.var, c->c.var);
      return;

    case TAC_EQUAL:
      cg_relop("EQUAL", c->a.var, c->b.var, c->c.var);
      return;

    case TAC_PRINT:
      cg_print(c->a.var);
      return;

    default:      
      error("unknown TAC opcode to translate");
      return;      
    }
}

void cg_binop(int op, SYMB *a, SYMB *b, SYMB *c){

  int cr = get_rreg(c); // result register
  int br = get_areg(b, cr);

  switch(op)
    {
    case TAC_ADD:      
      printf("    addl %s, %s\n", find_reg(br), find_reg(cr));
      break;    
    case TAC_SUB:
      printf("    subl %s, %s\n", find_reg(br), find_reg(cr));
      break;
    case TAC_MUL:
      printf("    imull %s, %s\n", find_reg(br), find_reg(cr));
      break;
    //this is a logical AND, not bitwise
    case TAC_AND:
      {	
	printf("    andl %s, %s\n", find_reg(br), find_reg(cr));
	break; 
      }
    //this is a logical OR, not bitwise
    case TAC_OR:
      {
        printf("    orl %s, %s\n", find_reg(br), find_reg(cr));
	break;
      }    
    //special cases
    case TAC_MOD:
    case TAC_TDIV:
    case TAC_DIV:
      {
	int eax = 6; 
	int edx = 9; 	
	spill_one(eax);
	spill_one(edx);
	insert_desc(eax, c, MODIFIED);
	insert_desc(edx, c, MODIFIED);	
	int ar = get_rreg(a);	
	if((ar == eax) || (ar == edx)){ //unlikely case
	  error("encountered an unhandled exception!\n");
	  exit(1);
	}
	load_reg(eax, c);
	load_reg(ar, b);
	printf("    cltd %s\n", find_reg(eax)); //sign extend c
	printf("    idivl %s\n", find_reg(ar)); //ar has result
	clear_desc(eax);
	clear_desc(edx);
	insert_desc(ar, a, MODIFIED);	
	break;
      }     
    }

  if((op != TAC_DIV) && (op != TAC_TDIV) && (op != TAC_MOD)){
    //delete c from descriptors and insert a
    clear_desc(cr);
    insert_desc(cr, a, MODIFIED);
  }
}

void cg_unop(int op, SYMB *a, SYMB *b){

  int ar = get_rreg(b);

  switch(op)
    {
    case TAC_NEG:
      {
	if(b->int_value < 0)
	  break;
	else
	  printf("    negl %s\n", find_reg(ar));      
	break;
      }
    case TAC_POS:
      {
	if(b->int_value > 0)
	  break;
	else
	  printf("    negl %s\n", find_reg(ar));      
	break;
      }    
    //this is a logical NOT, not bitwise
    case TAC_NOT:
      {
	//is b > 0?
	if(ar == R_COND){ //check and see if ar is %al
	  error("encountered unhandled exception!\n");
	  exit(1);
	}
	else
	  printf("    cmpl $0, %s\n", find_reg(ar));
	printf("    setg %s\n", M_AL);
	spill_one(ar);
	printf("    movzbl %s, %s\n", M_AL, find_reg(ar));		
	//now invert what ever is in ar, by comparing to zero
	printf("    cmpl $0, %s\n", find_reg(ar));
	printf("    sete %s\n", M_AL);
	printf("    movzbl %s, %s\n", M_AL, find_reg(ar)); // this is the final result  	
	break;
      }
    }

  //insert descriptor for a
  insert_desc(ar, a, MODIFIED);
}

void cg_relop(char *op, SYMB *a, SYMB *b, SYMB *c){

  int cr = get_rreg(c); // result register
  int br = get_areg(b, cr);

  printf("    cmpl %s, %s\n", find_reg(br), find_reg(cr));  

  if(!strcmp(op, "GRTRTHAN"))
    printf("    setg %s\n", M_AL);

  if(!strcmp(op, "GE"))
    printf("    setge %s\n", M_AL);

  if(!strcmp(op, "LESSTHAN"))
    printf("    setl %s\n", M_AL);

  if(!strcmp(op, "LE"))
    printf("    setle %s\n", M_AL);

  if(!strcmp(op, "EQUAL"))
    printf("    sete %s\n", M_AL);

  if(!strcmp(op, "NE"))
    printf("    setne %s\n", M_AL);

  clear_desc(cr);
  printf("    movzbl %s, %s\n", M_AL, find_reg(cr));
  insert_desc(cr, a, MODIFIED);
  spill_one(cr);
}

void cg_offset(SYMB *a, SYMB *b, SYMB *c){
 
  //not implemented in this version
}

void cg_copy(SYMB *a, SYMB *b){

  int br = get_rreg(b); //load b into a register
  insert_desc(br, a, MODIFIED); //indicate that a is there
}

void cg_cond(char *op, SYMB *a, int l){

  spill_all();

  if( a != NULL ){ // it is not a simple GOTO, set flags
        
    int  r ;

    for( r = R_GEN ; r < R_MAX ; r++ )   //Is it in reg?
      if( rdesc[r].name == a )
	break ;

    //was already in a reg
    if( r < R_MAX ){
      //use cmpl to set the flag 
      printf("    cmpl $0, %s\n", find_reg(r));
      if(!strcmp(op, "IFZ"))   
	printf("    je L%d\n", l);
    
      if(!strcmp(op, "IFNZ"))
	printf("    jne L%d\n", l);
    }
    //was not in a reg, load into new reg
    else{
      r = get_rreg(a);
      printf("    cmpl $0, %s\n", find_reg(r));
      if(!strcmp(op, "IFZ"))     
	printf("    je L%d\n", l);
    
      if(!strcmp(op, "IFNZ"))
	printf("    jne L%d\n", l);
    }
  } 

  if(!strcmp(op, "GOTO"))
    printf("    jmp L%d\n", l);
}

void cg_print(SYMB *s){

  int r = get_areg(s, &r);
  flush_all();

  if(s->token_type_1 == T_CONSTANT)
    printf("    pushl $%d\n", s->int_value);
  if((s->token_type_2 == T_VAR) || (s->token_type_2 == T_TEMP))
    printf("    pushl -%d(%s)\n", s->offset, M_EBP);

  printf("    pushl $.PRINTF_ARG\n");
  printf("    call printf\n");
  printf("    addl $8, %s\n", M_ESP); //always passed 2 arguments
}

void cg_arg(SYMB *a){

  // not implemented in this version
}

void cg_call(int f, SYMB *res){

// not implemented in this version
}

void cg_return(SYMB *a){

  // not implemented in this version
}

int insert_desc(int r, SYMB *n, int mod){

  rdesc[r].name = n;
  rdesc[r].modified = mod;
}

void clear_desc(int r){

  rdesc[r].name = NULL;
}

void load_reg(int r, SYMB *n){

  int s;

  //check if its already in a register
  for(s = 0; s < R_MAX; s++)
    if(rdesc[s].name == n){
      if(s == R_COND) //we are loading the contents of the cond register
	printf("    movzbl %s, %s\n", find_reg(s), find_reg(r));
      else
	printf("    movl %s, %s\n", find_reg(s), find_reg(r));
      insert_desc(r, n, rdesc[s].modified);
      return;
    }

  //not in a register, so load it
  if(n->token_type_1 == T_CONSTANT)
    printf("    movl $%d, %s\n", n->int_value, find_reg(r));
  else
    printf("    movl -%d(%s), %s\n", n->offset, M_EBP, find_reg(r));

  insert_desc(r, n, UNMODIFIED);
}

int get_rreg(SYMB *c){

  int r;

  //already in a register
  for(r = R_GEN; r < R_MAX; r++)
    if(rdesc[r].name == c){
      spill_one(r);
      return r;
    }

  //empty register
  for(r = R_GEN; r < R_MAX; r++)
    if(rdesc[r].name == NULL){
      load_reg(r, c);
      return r;
    }

  //unmodified register
  for(r = R_GEN; r < R_MAX; r++)
    if(!(rdesc[r].modified)){
      clear_desc(r);
      load_reg(r, c);
      return r;
    }

  //modified register
  spill_one(R_GEN);
  clear_desc(R_GEN);
  load_reg(R_GEN, c);
  return R_GEN;
}

int get_areg(SYMB *b, int cr){

  int r;

  //already in register
  for(r = R_ZERO; r < R_MAX; r++)
    if(rdesc[r].name == b)
      return r;

  //empty register
  for(r = R_GEN; r < R_MAX; r++)
    if(rdesc[r].name == NULL){
      load_reg(r, b);
      return r;
    }

  //unmodified register
  for(r = R_GEN; r < R_MAX; r++)
    if((!rdesc[r].modified) && (r != cr)){
      clear_desc(r);
      load_reg(r, b);
      return r;
    }

  //modified register
  for(r = R_GEN; r < R_MAX; r++)
    if(r != cr){
      spill_one(r);
      clear_desc(r);
      load_reg(r, b);
      return r;
    }
}

void flush_all(){

  int r;

  spill_all();

  for(r = R_GEN; r < R_MAX; r++)
    clear_desc(r);
}

void spill_all(){

  int r;

  for(r = R_GEN; r < R_MAX; r++)
    spill_one(r);
}

void spill_one(int r){

  if(rdesc[r].modified){
    printf("    movl %s, -%d(%s)\n", find_reg(r), rdesc[r].name->offset, M_EBP);
    rdesc[r].modified = UNMODIFIED;
  }
}

char *find_reg(int enum_val){

  switch(enum_val)
    {
    case 5:
      return "%al";
      break;
    case 6:
      return "%eax";
      break;
    case 7:
      return "%ebx";
      break;
    case 8:
      return "%ecx";
      break;
    case 9:
      return "%edx";
      break;
    case 10:
      return "%esi";
      break;
    case 11:
      return "%edi";
      break;
    }
}
