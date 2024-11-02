#ifndef command_h
#define command_h

// Command Data Structure
struct SimpleCommand {
	// Available space for arguments currently preallocated
	int _numberOfAvailableArguments;

	// Number of arguments
	int _numberOfArguments;
	char ** _arguments;
	
	SimpleCommand();
	void insertArgument( char * argument );
};

struct Command {
	int _numberOfAvailableSimpleCommands;
	int _numberOfSimpleCommands;
	SimpleCommand ** _simpleCommands;
	char * _outFile;
	char * _inputFile;
	char * _errFile;
	int _background;
	int _append;

	// Public interface
	void prompt();
	void print();
	void execute();
	void clear();
	
	Command();
	void insertSimpleCommand( SimpleCommand * simpleCommand );

	void setupInputRedirection(int defaultIn);
	void setupOutputRedirection(int defaultOut);
	void setupErrorRedirection(int defaultErr);
	void executeCommand(int i, int defaultIn, int defaultOut, int defaultErr);
	bool handleBuiltInCommands();

	static Command _currentCommand;
	static SimpleCommand *_currentSimpleCommand;
};

#endif
