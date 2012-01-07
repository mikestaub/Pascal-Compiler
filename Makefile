# Programmer: Mike Staub, symbol1 assignment

compiler: main.o y.tab.o symbol.o stack_interface.o stack.o utilities.o tac.o lex.yy.o cg.o
	gcc -o compiler main.o symbol.o stack_interface.o stack.o utilities.o tac.o lex.yy.o y.tab.o cg.o -g -lfl -lm

main.o: main.c
	gcc -c main.c -g

lex.yy.o:  pascal.l symbol.h y.tab.h stack.h stack_interface.h
	lex pascal.l
	gcc -c lex.yy.c

y.tab.o: pascal.y symbol.h stack.h stack_interface.h
	yacc -d -v pascal.y
	gcc -c y.tab.c -w

stack.o: stack.h stack.c
	gcc -c stack.c -g

stack_interface.o: stack_interface.h stack_interface.c
	gcc -c stack_interface.c -g

utilities.o: utilities.h utilities.c
	gcc -c utilities.c -g

symbol.o: symbol.h symbol.c
	gcc -c symbol.c -g

tac.o: tac.c tac.h symbol.h
	gcc -c tac.c -g -w

cg.o: tac.h cg.c cg.h
	gcc -c cg.c -g -w

clean:
	rm *.o
	rm y.tab.c
	rm y.tab.h
	rm lex.yy.c
	rm y.output
	rm compiler
	rm *~
