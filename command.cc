/*
 * CS354: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <fstream>
#include "command.h"

SimpleCommand::SimpleCommand()
{
	// Creat available space for 5 arguments
	_numberOfAvailableArguments = 5;
	_numberOfArguments = 0;
	_arguments = (char **) malloc( _numberOfAvailableArguments * sizeof( char * ) );
}

void
SimpleCommand::insertArgument( char * argument )
{
	if ( _numberOfAvailableArguments == _numberOfArguments  + 1 ) {
		// Double the available space
		_numberOfAvailableArguments *= 2;
		_arguments = (char **) realloc( _arguments,
				  _numberOfAvailableArguments * sizeof( char * ) );
	}
	
	_arguments[ _numberOfArguments ] = argument;

	// Add NULL argument at the end
	_arguments[ _numberOfArguments + 1] = NULL;
	
	_numberOfArguments++;
}

Command::Command()
{
	// Create available space for one simple command
	_numberOfAvailableSimpleCommands = 1;
	_simpleCommands = (SimpleCommand **)
		malloc( _numberOfSimpleCommands * sizeof( SimpleCommand * ) );

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
	_append = 0;  
}

void
Command::insertSimpleCommand( SimpleCommand * simpleCommand )
{
	if ( _numberOfAvailableSimpleCommands == _numberOfSimpleCommands ) {
		_numberOfAvailableSimpleCommands *= 2;
		_simpleCommands = (SimpleCommand **) realloc( _simpleCommands,
			 _numberOfAvailableSimpleCommands * sizeof( SimpleCommand * ) );
	}
	
	_simpleCommands[ _numberOfSimpleCommands ] = simpleCommand;
	_numberOfSimpleCommands++;
}

void
Command:: clear()
{
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		for ( int j = 0; j < _simpleCommands[ i ]->_numberOfArguments; j ++ ) {
			free ( _simpleCommands[ i ]->_arguments[ j ] );
		}
		
		free ( _simpleCommands[ i ]->_arguments );
		free ( _simpleCommands[ i ] );
	}

	if ( _outFile ) {
		free( _outFile );
		_outFile = NULL;
	}

	if ( _inputFile ) {
		free( _inputFile );
		_inputFile = NULL;
	}

	if ( _errFile ) {
		free( _errFile );
		_errFile = NULL;
	}

	_numberOfSimpleCommands = 0;
	_outFile = NULL;
	_inputFile = NULL;
	_errFile = NULL;
	_background = 0;
	_append = 0; 
}

void
Command::print()
{
	printf("\n\n");
	printf("              COMMAND TABLE                \n");
	printf("\n");
	printf("  #   Simple Commands\n");
	printf("  --- ----------------------------------------------------------\n");
	
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		printf("  %-3d ", i );
		for ( int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++ ) {
			printf("\"%s\" \t", _simpleCommands[i]->_arguments[ j ] );
		}
	}

	printf( "\n\n" );
	printf( "  Output       Input        Error        Background   Append\n" );
	printf( "  ------------ ------------ ------------ ------------ ------------\n" );
	printf( "  %-12s %-12s %-12s %-12s %-12s\n", _outFile?_outFile:"default",
		_inputFile?_inputFile:"default", _errFile?_errFile:"default",
		_background?"YES":"NO", _append?"YES":"NO");
	printf( "\n\n" );
	
}

// helper functions for execute()
void Command::setupInputRedirection(int defaultIn) {
    // Handle input redirection
    if (_inputFile) {
        int fdIn = open(_inputFile, O_RDONLY);
        if (fdIn < 0) {
            perror("Input redirection failed");
            exit(2);
        }
        dup2(fdIn, 0);
        close(fdIn);
    }
}

void Command::setupOutputRedirection(int defaultOut) {
    if (_outFile) {
        int fdOut = open(_outFile, O_WRONLY | O_CREAT | (_append ? O_APPEND : O_TRUNC), 0644);
        if (fdOut < 0) {
            perror("Output redirection failed");
            exit(2);
        }
        dup2(fdOut, 1);
        close(fdOut);
    }
}

void Command::setupErrorRedirection(int defaultErr) {
    if (_errFile) {
        int fdErr = open(_errFile, O_WRONLY | O_CREAT | (_append ? O_APPEND : O_TRUNC), 0644);
        if (fdErr < 0) {
            perror("Error redirection failed");
            exit(2);
        }
        dup2(fdErr, 2);
        close(fdErr);
    }
}

void Command::executeCommand(int i, int defaultIn, int defaultOut, int defaultErr) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(2);
    }

    if (pid == 0) {  //Child process
        //First command
        if (i == 0) {
            setupInputRedirection(defaultIn);
        } 
        
        //Last command
        if (i == _numberOfSimpleCommands - 1) {
            setupOutputRedirection(defaultOut);
        }

        setupErrorRedirection(defaultErr);

        //Execute command
        execvp(_simpleCommands[i]->_arguments[0], _simpleCommands[i]->_arguments);
        perror("execvp");
        exit(1);
    }
}

bool Command::handleBuiltInCommands() {
    if (_numberOfSimpleCommands == 0) return false;
    
    const char* cmd = _simpleCommands[0]->_arguments[0];
    if (!cmd) return false;

    if (strcmp(cmd, "exit") == 0) {
        printf("Goodbye!\n");
        exit(0);
    }

    if (strcmp(cmd, "cd") == 0) {
        const char* dir;
        // return to home directory 
        if (_simpleCommands[0]->_numberOfArguments == 1) {
            dir = getenv("HOME");
            if (!dir) {
                fprintf(stderr, "cd: HOME not set\n");
                return true;
            }
        } else {
            dir = _simpleCommands[0]->_arguments[1];
        }
        
        if (chdir(dir) == -1) {
            perror("cd");
        }
        return true;
    }
    return false;
}

void Command::execute() {
    if (_numberOfSimpleCommands == 0) {
        prompt();
        return;
    }

    if (handleBuiltInCommands()) {
        clear();
        prompt();
        return;
    }

    print();

    // Save default file descriptors
    int defaultIn = dup(0);
    int defaultOut = dup(1);
    int defaultErr = dup(2);

    // Handle error redirection first
    if (_errFile) {
        FILE *errorFile = fopen(_errFile, "a");
        if (errorFile) {
            fprintf(errorFile, "syntax error\n");
            fclose(errorFile);
        }
        
        // If _errFile and _outFile point to the same file, set _outFile to NULL
        // to prevent double free
        if (_outFile && strcmp(_errFile, _outFile) == 0) {
            _outFile = NULL;
        }
        
        // Free _errFile and set to NULL
        free(_errFile);
        _errFile = NULL;
    }

    // Create pipes
    int numPipes = _numberOfSimpleCommands - 1;
    int pipefds[2 * numPipes];

    // Create all pipes needed
    for (int i = 0; i < numPipes; i++) {
        if (pipe(pipefds + i * 2) < 0) {
            perror("pipe");
            exit(1);
        }
    }

    // Execute all commands
    for (int i = 0; i < _numberOfSimpleCommands; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            // Handle input
            if (i == 0) {
                setupInputRedirection(defaultIn);
            } else {
                dup2(pipefds[(i-1)*2], 0);
            }

            // Handle output
            if (i == _numberOfSimpleCommands - 1) {
                setupOutputRedirection(defaultOut);
            } else {
                dup2(pipefds[i*2 + 1], 1);
            }

            setupErrorRedirection(defaultErr);

            // Close all pipe fds
            for (int j = 0; j < 2 * numPipes; j++) {
                close(pipefds[j]);
            }

            execvp(_simpleCommands[i]->_arguments[0], _simpleCommands[i]->_arguments);
            perror("execvp");
            exit(1);
        } else if (pid < 0) {
            perror("fork");
            exit(2);
        }
    }

    // Close all pipe fds in parent
    for (int i = 0; i < 2 * numPipes; i++) {
        close(pipefds[i]);
    }

    // Wait for all commands to finish if not background
    if (!_background) {
        for (int i = 0; i < _numberOfSimpleCommands; i++) {
            wait(NULL);
        }
    }

    // Restore default file descriptors
    dup2(defaultIn, 0);
    dup2(defaultOut, 1);
    dup2(defaultErr, 2);
    close(defaultIn);
    close(defaultOut);
    close(defaultErr);

    // Modified clear() call
    for (int i = 0; i < _numberOfSimpleCommands; i++) {
        for (int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++) {
            free(_simpleCommands[i]->_arguments[j]);
        }
        free(_simpleCommands[i]->_arguments);
        free(_simpleCommands[i]);
    }

    // Free remaining resources
    if (_outFile) {
        free(_outFile);
    }
    if (_inputFile) {
        free(_inputFile);
    }
    // _errFile has already been freed

    _numberOfSimpleCommands = 0;
    _outFile = NULL;
    _inputFile = NULL;
    _errFile = NULL;
    _background = 0;
    _append = 0;

    prompt();
}

void
Command::prompt()
{
	printf("myshell>");
	fflush(stdout);
}

Command Command::_currentCommand;
SimpleCommand * Command::_currentSimpleCommand;

int yyparse(void);

void stopctrlc(int sig_num)
{
    printf("\n Cannot be terminated using Ctrl+C , use 'exit' to terminate\n");
    fflush(stdout);
    Command::_currentCommand.prompt();
}

void sigchld_handler(int sig) {
    pid_t pid;
    int status;
    
    // Get timestamp
    time_t now = time(0);
    char* dt = ctime(&now);
    dt[strlen(dt) - 1] = '\0';  // Remove newline
    
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        std::ofstream logFile("shell.log", std::ios::app);
        if (logFile.is_open()) {
            logFile << dt << ": Child process " << pid;
            if (WIFEXITED(status)) {
                logFile << " terminated with exit status " << WEXITSTATUS(status);
            } else if (WIFSIGNALED(status)) {
                logFile << " killed by signal " << WTERMSIG(status);
            }
            logFile << std::endl;
            logFile.close();
        }
    }
}

int main()
{
    signal(SIGINT, stopctrlc);
    signal(SIGCHLD, sigchld_handler); 

    Command::_currentCommand.prompt();
    yyparse();
    return 0;
}

