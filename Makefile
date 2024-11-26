# Use GNU compiler
CC = gcc -g
CXX = g++ -g

all: shell

# Rule to generate y.tab.c and y.tab.h from shell.y
y.tab.c y.tab.h: shell.y
	yacc -d shell.y

# Rule to compile y.tab.o from y.tab.c
y.tab.o: y.tab.c
	$(CXX) -c y.tab.c

# Rule to compile lex.yy.o from shell.l
lex.yy.o: shell.l
	lex shell.l
	$(CC) -c lex.yy.c

# Rule to compile command.o from command.cc
command.o: command.cc command.h
	$(CXX) -c command.cc

# Rule to link all object files into the shell executable
shell: y.tab.o lex.yy.o command.o
	$(CXX) -o shell lex.yy.o y.tab.o command.o -ll

# Clean up generated files
clean:
	rm -f lex.yy.c y.tab.c y.tab.h shell *.o
