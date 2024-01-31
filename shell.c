#include <sys/types.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>

#define MAX_COMMAND_LENGTH 1024

// Signal handler for Ctrl+C
void ctrlCHandler(int signum) {
    printf("\nYou typed Control-C!\n");
}

int main() {
     // Set up the signal handler for Ctrl+C
    signal(SIGINT, ctrlCHandler);
    char command[MAX_COMMAND_LENGTH];
    char *prompt = "hello: ";
    char lastCommand[MAX_COMMAND_LENGTH];  // Variable to store the last command
    bool isRepeat = false;
    int fd, amper, redirect, piping, retid, status, argc1;
    int fildes[2];
    char *outfile;

    while (1) {
        if(!isRepeat){
            printf("%s",prompt);
            strcpy(lastCommand, command);
            fgets(command, sizeof(command), stdin);
            // Remove the newline character
            command[strlen(command) - 1] = '\0';
             
        }else{
            isRepeat = false;
            strcpy(command, lastCommand);
        }

        if (strcmp(command, "quit") == 0) {
            break;  // Exit the loop if the user types "quit"
        }

        char **argv1 = malloc(MAX_COMMAND_LENGTH * sizeof(char *));
        char *command_memory[MAX_COMMAND_LENGTH];
        int num_commands = 0;
        // Tokenize the command based on the pipe character '|'
        char *token = strtok(command, "|");
        while (token != NULL) {
            argv1[num_commands] = strdup(token);
            command_memory[num_commands] = argv1[num_commands];
            num_commands++;
            if (num_commands >= MAX_COMMAND_LENGTH) {
                fprintf(stderr, "Too many argv1 in the pipeline\n");
                exit(EXIT_FAILURE);
            }
            token = strtok(NULL, "|");
        }

        int pipes[num_commands - 1][2];
        for (int i = 0; i < num_commands - 1; i++) {
            if (pipe(pipes[i]) == -1) {
                perror("Pipe creation failed");
                exit(EXIT_FAILURE);
            }
        }

        for (int i = 0; i < num_commands; i++) {
            bool isCommandExists = false;
            // Tokenize the command
            char *args[MAX_COMMAND_LENGTH];
            char *token = strtok(argv1[i], " ");
            int j = 0;
            while (token != NULL) {
                args[j] = strdup(token);
                j++;
                token = strtok(NULL, " ");
            }
            args[j] = NULL;  // Null-terminate the argument list
            argc1 = j;

            if (! strcmp(args[argc1 - 1], "&")) {
                amper = 1;
                args[argc1 - 1] = NULL;
            }
            else{
                amper = 0;
            } 
            if(num_commands > 1){
                amper = 1;
            }
            if (argc1 > 1 && ! strcmp(args[argc1 - 2], ">")) {
                redirect = 1;
                args[argc1 - 2] = NULL;
                outfile = args[argc1 - 1];
            }
            else {
                redirect = 0; 
            }

            //Section 2
            if (strcmp(args[0], "prompt") == 0 && strcmp(args[1], "=") == 0) {
                char myPrompt[1024];
                strcpy(myPrompt, strcat(args[2], ": "));
                prompt = myPrompt;
                isCommandExists = true;
            }

            //Section 5
            char cwd[256];
            //Checks if at the beginning of the sentence the word "cd" appears
            if (!strcmp(args[0], "cd")) {
                /*
                chdir() changes the current working directory of the calling
                process to the directory specified in path.
                */
                if (chdir(args[1]) != 0) {
                    fprintf(stderr, " chdir() to %s failed: %s\n",args[1],strerror(errno));
                }else{
                    /*
                    getcwd - These functions return a null-terminated string containing an absolute
                    pathname that is the current working directory of the calling process
                    and print the new directory.
                    */
                    getcwd(cwd, sizeof(cwd));
                    printf("%s\n", cwd);
                }
                isCommandExists = true;

            }

            //Section 6
            /* Check for the command to repeat the last command */
            if (strcmp(argv1[0], "!!") == 0) {
                if (strlen(lastCommand) > 0) {
                    printf("Repeating last command: %s\n",lastCommand);
                    isRepeat = true;
                    isCommandExists = true;
                } else {
                    printf("No previous command to repeat.\n");
                }
            }

            pid_t pid = fork();
            if (pid == -1) {
                perror("Fork failed");
                exit(EXIT_FAILURE);
            } else if (pid == 0) {  // Child process
            // Reset the signal  to default behavior 
                signal(SIGINT, SIG_DFL);
                /* redirection of IO ? */
                if (redirect) {
                    fd = creat(outfile, 0660); 
                    close (STDOUT_FILENO) ; 
                    dup(fd); 
                    close(fd); 
                    /* stdout is now redirected */
                } 
                if (i > 0) {
                    // Redirect command from the previous pipe
                    dup2(pipes[i - 1][0], STDIN_FILENO);
                    close(pipes[i - 1][0]);
                    close(pipes[i - 1][1]);
                }
                if (i < num_commands - 1) {
                    // Redirect output to the next pipe
                    dup2(pipes[i][1], STDOUT_FILENO);
                    close(pipes[i][0]);
                    close(pipes[i][1]);
                }
                // Close all pipes in child process
                for (int j = 0; j < num_commands - 1; j++) {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }

                if(!isCommandExists){
                // Execute the command
                    execvp(args[0], args);
                    // If execvp fails
                    perror("Execution failed");
                    exit(EXIT_FAILURE);
                }else{
                    isCommandExists = false;
                }         
            }
            /* parent continues over here... */
            /* waits for child to exit if required */
            if (amper == 0){
                retid = wait(&status);
            }
        }

        // Close all pipes in the parent process
        for (int i = 0; i < num_commands - 1; i++) {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        // Wait for all child processes to finish
        for (int i = 0; i < num_commands; i++) {
            wait(NULL);
        }

        // Free dynamically allocated memory
        for (int i = 0; i < num_commands; i++) {
            free(command_memory[i]);
        }
        free(argv1);  
    }

    return 0;
}