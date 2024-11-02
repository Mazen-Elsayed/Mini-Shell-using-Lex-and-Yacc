# Use GNU compiler
CC = gcc -g
CXX = g++ -g

all: shell

lex.yy.o: shell.l
	lex shell.l
	$(CC) -c lex.yy.c

y.tab.o: shell.y y.tab.h
	yacc -d shell.y
	$(CXX) -c y.tab.c

command.o: command.cc command.h
	$(CXX) -c command.cc

shell: y.tab.o lex.yy.o command.o
	$(CXX) -o shell lex.yy.o y.tab.o command.o -ll

clean:
	rm -f lex.yy.c y.tab.c y.tab.h shell *.o
