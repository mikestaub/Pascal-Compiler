// utilities.h

#ifndef UTIL_H
#define UTIL_H

void error(char* s);
void *safe_malloc(int n);
void dump_table(SYMBTABLE *table);
void dump_constant_table(SYMBTABLE *constant_symbol_table);
char *token_to_string(int token);
void print_tac(TAC *i);
char *ts(SYMB *s, char *str);
TAC *insert_tac(TAC *t, TAC *list);
void dump_tac(TAC *tac_list);

#endif
