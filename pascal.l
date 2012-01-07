/********************************************************************/
/*                                                                  */
/*                                                                  */
/*                        LEXER / SCANNER                           */
/*                                                                  */
/*                                                                  */
/*                                                                  */
/********************************************************************/

%{
#include <stdio.h>
#include "stack.h"
#include "symbol.h"
#include "tac.h"
#include "y.tab.h"

SYMBTABLE *current_table;
extern SYMBTABLE *constant_symbol_table;
extern int line_count;
extern stack symbol_table_stack;
extern char *yytext;

#define token(x) x
%}

alpha       [a-zA-Z_]
alphanum    [a-zA-Z_0-9]
comment     "{"[^}]*"}"
exponent    e([+-])?[0-9]+
string      \'([^'\n]|\'\')+\'
unsignedint [0-9]+
signedint   "-"[0-9]+|"+"[0-9]+
whitespace  [ \t]
newline     [\n]
%%

">="                    return token(GE);
"<="                    return token(LE);
"<>"                    return token(NE);
":="                    return token(ASSIGN);
".."                    return token(DOTDOT);

"AND"                   return token(AND);
"ARRAY"                 return token(ARRAY);
"BEGIN"                 return token(_BEGIN);
"CASE"                  return token(CASE);
"CONST"                 return token(CONST);
"DIV"                   return token(DIV);
"DO"                    return token(DO);
"DOWNTO"                return token(DOWNTO);
"ELSE"                  return token(ELSE);
"END"                   return token(END);
"FILE"                  return token(_FILE);
"FOR"                   return token(FOR);
"FORWARD"               return token(FORWARD);
"FUNCTION"              return token(FUNCTION);
"GOTO"                  return token(GOTO);
"IF"                    return token(IF);
"IN"                    return token(IN);
"LABEL"                 return token(LABEL);
"MOD"                   return token(MOD);
"NIL"                   return token(NIL);
"NOT"                   return token(NOT);
"OF"                    return token(OF);
"OR"                    return token(OR);
"PACKED"                return token(PACKED);
"PROCEDURE"             return token(PROCEDURE);
"PROGRAM"               return token(PROGRAM);
"RECORD"                return token(RECORD);
"REPEAT"                return token(REPEAT);
"SET"                   return token(SET);
"THEN"                  return token(THEN);
"TO"                    return token(TO);
"TYPE"                  return token(TYPE);
"UNTIL"                 return token(UNTIL);
"VAR"                   return token(VAR);
"WHILE"                 return token(WHILE);
"WITH"                  return token(WITH);
"NUM"                   return token(NUM);
"INTEGER"               return token(INTEGER);
"REAL"                  return token(REAL);
"print"                 return token(print);

{alpha}{alphanum}*      {
                          top_table( &symbol_table_stack, &current_table );     
                          mkname( current_table, yytext, T_VAR );
                          return token(ID);
                        }
{signedint}             {
                          mkname( constant_symbol_table, yytext, T_INT );
                          return token(NUM_INT);
                        }
{unsignedint}           {
                          mkname( constant_symbol_table, yytext, T_INT );
                          return token(NUM_INT);
                        }
{unsignedint}\.{unsignedint}({exponent})? {
                          mkname( constant_symbol_table, yytext, T_REAL );
                          return token(NUM_REAL);
                        }
{unsignedint}{exponent} {
                          mkname( constant_symbol_table, yytext, T_REAL );
                          return token(NUM_REAL);
                        }
{string}                {
                          mkname( constant_symbol_table, yytext, T_STRING );
                          return token(STRING);
                        }
{whitespace}+           ;

{newline}               { line_count++; }

{comment}+              ;

.                       return token(yytext[0]);
%%

