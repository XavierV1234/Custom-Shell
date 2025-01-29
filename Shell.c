#define _GNU_SOURCE 
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#define WHITESPACE " \t\n"      // Delimiters for tokenizing the input
#define MAX_COMMAND_SIZE 255    // Maximum command-line size
#define MAX_NUM_ARGUMENTS 32     // Maximum number of arguments

// The error function
void errormg() {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message)); 
}

int main(int argc, char *argv[]) {
    char *command_string = (char *)malloc(MAX_COMMAND_SIZE);
    FILE *fd = NULL;

    // Deciding for the modes
    if (argc > 2) {
        errormg();
        exit(1);
    }

    if (argc == 2) {
        fd = fopen(argv[1], "r");
        if (fd == NULL) {
            errormg();  
            exit(1);
        }
    }

    while (1) {
        if (fd != NULL) {
            if (!fgets(command_string, MAX_COMMAND_SIZE, fd)) {
                if (feof(fd)) {
                    fclose(fd);
                    break;  
                }
                continue;  
            }
        } else {
            printf("msh> ");
            if (!fgets(command_string, MAX_COMMAND_SIZE, stdin)) {
                if (feof(stdin)) {
                    break; 
                }
                continue; 
            }
        }

        //Parse inputs
        char *token[MAX_NUM_ARGUMENTS];
        int token_count = 0;
        char *argument_pointer;
        char *working_string = strdup(command_string);  
        char *head_ptr = working_string;  

        // Tokenize the string
        while (((argument_pointer = strsep(&working_string, WHITESPACE)) != NULL)
                     && (token_count < MAX_NUM_ARGUMENTS)) {
            if (strlen(argument_pointer) == 0) {
                continue;  
            }
            token[token_count] = strndup(argument_pointer, MAX_COMMAND_SIZE);
            token_count++;
        }
        token[token_count] = NULL;  

        // Checking for exit
        if (token[0] != NULL &&strcmp(token[0], "exit") == 0) {
            if (token[1] != NULL) {
                errormg();  
                free(head_ptr);
                return 0;
            } else {
                free(head_ptr);
                break;
            }
        }

        //handling the cd once imputted
        if (token[0] != NULL &&strcmp(token[0], "cd") == 0) {
            if (token[1] == NULL || token[2] != NULL) {
                errormg();  
            } else {
                if (chdir(token[1]) != 0) {  
                    perror("cd");  
                }
            }
            free(head_ptr);  
            continue;  
        }

        pid_t pid = fork();

        if (pid == -1) {
            perror("fork has failed");
            exit(1);
        } else if (pid == 0) { 

            // Check if the command is a shell and if it doesnt have a contain a path
            if (strstr(token[0], ".sh") != NULL) {  
                if (strstr(token[0], "/") == NULL) { 
                    errormg();  
                    exit(1);   
                }
            }

            int redirectionSymbol = -1; // looking at the redirection symbol
            int add = 0;  //adding data or not 

            // Louking for for arroews
            for (int i = 0; i < token_count; i++) {
                if (token[i] == NULL) {
                    break;
                }
                if (strcmp(token[i], ">") == 0) {
                    redirectionSymbol = i;
                    add = 0; 
                    break;
                } else if (strcmp(token[i], ">>") == 0) {
                    redirectionSymbol = i;
                    add = 1;
                    break; 
                }
            }

            // Throwing error if there is nothing before the redirection error
            if (redirectionSymbol == 0) {
                errormg();
                exit(1);
            }

            // Checking if there is more than one output file in redirection
            if (redirectionSymbol != -1) {
              
                if (token[redirectionSymbol + 1] != NULL) {
                 
                    if (token[redirectionSymbol + 2] != NULL) {
                        errormg(); 
                        exit(1);
                    }
                }

                // Openning the file depending on the mode
                int fd;
                if (add) {
                   //Checks for both modes
                    fd = open(token[redirectionSymbol + 1], O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
                } else {
                
                    fd = open(token[redirectionSymbol + 1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
                }
                
                if (fd < 0) {
                    errormg(); // Error if unable to open the file
                    exit(0);                    
                }

                // Redirect stdout to the file
                dup2(fd, STDOUT_FILENO);
                close(fd);

                // set redirection to NULL
                token[redirectionSymbol] = NULL; 
            }

            // Execute the command
            if (execvp(token[0], token) == -1) {
                perror("exec has failed");  
                exit(1);
            }//
        } else { 
            int status;
            waitpid(pid, &status, 0);
        }

        // free tokens
        for (int i = 0; i < token_count; i++) {
            free(token[i]);  
        }

        free(head_ptr);  
    }

    free(command_string);  
    return 0;
}