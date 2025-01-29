# Custom-Shell
## Summary of Custom-Shell

The custom shell program provides a basic command-line interface that executes user commands interactively or from a script file. It supports built-in commands like cd (change directory) and exit (terminate the shell), and uses fork() and execvp() to execute external commands in child processes while the parent waits using waitpid(). The shell handles file redirection with > (overwrite) and >> (append) for output, ensuring proper syntax validation. Commands are tokenized using whitespace, and errors like invalid arguments or missing files are displayed. It also prevents execution of .sh files without explicit paths. The program demonstrates process creation, command execution, and file management efficiently.

## Key Features of Custom-Shell

### Command Parsing

-The shell reads user input or a script file and tokenizes the command using whitespace as delimiters.

-It supports a maximum command size of 255 characters and up to 32 arguments.

### Built-in Commands

-exit: Terminates the shell.

-cd <directory>: Changes the current working directory.

### Executing Commands

-If the command is not built-in, the shell attempts to execute it using fork() and execvp().

-If execution fails, an error message is displayed.

### Handling Shell Scripts (.sh files)

-If a command includes a .sh extension but lacks an absolute or relative path (/), it is considered invalid.

### File Redirection (> and >>):
-(>) redirects standard output to a file, overwriting it.

-(>>) appends standard output to a file.

-If improper redirection syntax is detected, an error message is displayed.

### Error Handling

-The function errormg() is used to display error messages consistently.

-Errors are shown when commands are invalid, files cannot be opened, or execution fails.
### Interactive and Script Mode

-Interactive Mode: The user enters commands at the prompt (msh>).

-Script Mode: If a filename is provided as a command-line argument, the shell reads and executes commands from the file.
