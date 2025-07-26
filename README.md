# Mini-Shell-using-Lex-and-Yacc

## Project Description
This repository contains a mini-shell implementation developed using Lex and Yacc in C/C++. The shell serves as a command-line interface to interact with the operating system, parsing user input with Lex for lexical analysis and Yacc for syntax parsing. It supports basic command execution and may include advanced features like piping, redirection, or background processes, depending on the implementation. The project is designed to run on Unix-like systems and demonstrates the use of Lex and Yacc for building robust command parsers.

## Key Features
- **Lexical Analysis**: Lex tokenizes user input into keywords, operators, and symbols.
- **Syntax Parsing**: Yacc defines grammar rules and parses tokenized input to execute commands.
- **Command Execution**: Supports basic shell commands with potential extensions for piping or redirection.
- **Build Automation**: Includes a Makefile for compiling Lex, Yacc, and C/C++ files.

## Requirements
- C/C++ Compiler (e.g., gcc, g++)
- Lex (or Flex)
- Yacc (or Bison)
- Unix-like system (e.g., Ubuntu, WSL on Windows)
- Make (for build automation)

## Installation
1. Clone the repository: `git clone https://github.com/Mazen-Elsayed/Mini-Shell-using-Lex-and-Yacc.git`
2. Install dependencies: 
   - On Ubuntu: `sudo apt install flex bison make g++`
3. Navigate to the project directory: `cd Mini-Shell-using-Lex-and-Yacc`
4. Compile the project: `make`
5. Run the shell: `./shell`

## Usage
- Launch the shell with `./shell`.
- Enter commands (e.g., `ls`, `cd`, `echo`) as in a standard Unix terminal.
- Use supported features like piping (`|`) or redirection (`>`), if implemented.
- Exit the shell with `exit` or `Ctrl+C`.
