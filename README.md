# Linux-Command-Line-Interpreter

This project implements a custom shell command-line interpreter in C, which mimics some of the functionality of popular Linux shells like Bash. It allows the user to execute basic commands, including parameterized commands, output redirection, and piping.

## Features
* Execute simple commands and commands with parameters.
* Search for executable programs in directories specified by the PATH environment variable.
* Output redirection using the > symbol.
* Command piping using the | symbol.
* Continuous user prompting until the exit command is typed.

## How it Works
1. Command Parsing:
The program prompts the user with [Enter Command]>. The command entered by the user is parsed to extract the program name, parameters, and any special symbols like > or |.

2. Program Execution:
The shell searches for the program in the directories specified by the PATH environment variable using the getenv() and stat() functions. If the program is found, it is executed using execv().

3. Output Redirection:
If the command contains the > symbol, output is redirected to the specified file using dup2().

4. Command Piping:
If the command contains a |, a pipe is created between two processes, allowing the output of one command to serve as input for the next.

5. Loop Until Exit:
The shell continues to prompt for commands until the user types exit.

## Key Concepts
* Process Manipulation: Forking and creating child processes.
* Command Parsing: Splitting user input into executable commands and arguments.
* Input/Output Redirection: Redirecting standard output to a file.
* Pipes: Enabling inter-process communication using pipes.
* Environment Variables: Accessing and using the PATH environment variable to find executables.


Here are some examples of commands that the program can deal with:
* ls
* ls -l
* ls -l > file_list.txt
* ls -l | wc -l
* ls -l | wc -l > numfiles.txt
* ps -e ––forest
* ps -ef > process_info.txt
