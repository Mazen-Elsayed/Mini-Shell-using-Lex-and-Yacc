%{
#include <string.h>
#include <cstdio>
#include "command.h"
extern "C" {
    int yylex();
    void yyerror(const char *s);
}
#define yylex yylex
%}

%union {
    char *string_val;
}

%token <string_val> WORD
%token NOTOKEN OVERWRITE APPEND LESS PIPE AMPERSAND NEWLINE
%token APPEND_ERR

%%

goal:
    commands
    ;

commands:
    command
    | commands command
    ;

command:
    pipe_list iomodifier_opt background_opt NEWLINE {
        printf("   Yacc: Execute command\n");
        Command::_currentCommand.execute();
    }
    | NEWLINE
    | error NEWLINE { yyerrok; }
    ;

pipe_list:
    simple_command
    | pipe_list PIPE simple_command
    ;

simple_command:
    command_word arg_list {
        Command::_currentCommand.
            insertSimpleCommand(Command::_currentSimpleCommand);
    }
    ;

command_word:
    WORD {
        printf("   Yacc: insert command \"%s\"\n", $1);
        Command::_currentSimpleCommand = new SimpleCommand();
        Command::_currentSimpleCommand->insertArgument($1);
    }
    ;

arg_list:
    arg_list WORD {
        printf("   Yacc: insert argument \"%s\"\n", $2);
        Command::_currentSimpleCommand->insertArgument($2);
    }
    | /* can be empty */
    ;

iomodifier_opt:
    iomodifier_opt OVERWRITE WORD {
        printf("   Yacc: output redirection (overwrite) to \"%s\"\n", $3);
        Command::_currentCommand._outFile = $3;
        Command::_currentCommand._append = 0;
    }
    | iomodifier_opt APPEND WORD {
        printf("   Yacc: output redirection (append) to \"%s\"\n", $3);
        Command::_currentCommand._outFile = $3;
        Command::_currentCommand._append = 1;
    }
    | iomodifier_opt APPEND_ERR WORD {
        printf("   Yacc: output and error redirection to \"%s\"\n", $3);
        Command::_currentCommand._outFile = $3;
        Command::_currentCommand._errFile = $3;
        Command::_currentCommand._append = 1;
    }
    | iomodifier_opt LESS WORD {
        printf("   Yacc: input redirection from \"%s\"\n", $3);
        Command::_currentCommand._inputFile = $3;
    }
    | /* can be empty */
    ;

background_opt:
    AMPERSAND {
        Command::_currentCommand._background = 1;
    }
    | /* can be empty */
    ;

%%

extern "C" void yyerror(const char * s)
{
    fprintf(stderr, "%s\n", s);
}