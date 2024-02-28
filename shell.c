#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "tokens.h"
#include "vect.h"

// Declaring the built-in commands to be defined later in this file
void help();
int cd(const char *path);
void prev(const char* prev_command);
void source(const char* filename);

/**
 * Executes command with its arguments.
 *
 * @param args A character array that holds the command and its arguments.
 */
void execute(const char **args, const char *input_file, const char *output_file) {
    pid_t pid; // Declare a variable to store the process ID of the child process
    int status; // Declare a variable to store the exit status of the child process

    pid = fork(); // Create a new process by forking the current process

    // If this is the child process,
    if (pid == 0) {

        // If there is an input file to handle,
        if (input_file) {

            // Attempt to open the input file and save it as a file descriptor
            int input_fd = open(input_file, O_RDONLY);

            // If the file could not be opened,
            if (input_fd == -1) {
                perror("ERROR: could not open the input file"); // Print an error message
                exit(1); // Exit with an error code to indicate failure
            }

            dup2(input_fd, STDIN_FILENO); // Duplicate the file descriptor
            close(input_fd); // Close the file descriptor
        }

        // If there is an output file to handle,
        if (output_file) {

            // Attempt to open the output file and save it as a file descriptor
            int output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);

            // If the file could not be opened,
            if (output_fd == -1) {
                perror("ERROR: could not open the output file"); // Print an error message
                exit(1); // Exit with an error code to indicate failure
            }

            dup2(output_fd, STDOUT_FILENO); // Duplicate the file descriptor
            close(output_fd); // Close the file descriptor
        }

        // Replace the current process with a new program specified by args[0]
        if (execvp(args[0], (char *const *)args) == -1) {
            perror("ERROR: execvp failed in execute"); // Print an error message
            exit(1); // Exit the child process with an error code to indicate failure
        }
    }

    // If the child process was not created,
    else if (pid < 0) {
        perror("ERROR: fork failed in execute"); // Print an error message
        exit(1); // Exit the parent process with an error code to indicate failure
    }

    // Otherwise, we are in the parent process, so,
    else {
        waitpid(pid, &status, 0); // Wait for the child process to complete and store its status
    }
}

/**
 * Executes two shell commands in a piped fashion with optional input and output redirection.
 *
 * @param command_one_args An array of strings representing the first command and its arguments.
 * @param command_two_args An array of strings representing the second command and its arguments.
 * @parm input_file A string specifying an input file for redirection (can be NULL for no redirection).
 * @param output_file A string specifying an output file for redirection (can be NULL for no redirection).
 */
void execute_piped(const char **command_one_args, const char **command_two_args, const char *input_file, const char *output_file) {

    int pipefd[2]; // Declare an array to hold the read and write end of the pipe
    pid_t command_one_pid, command_two_pid; // Declare two variables that will store the PIDs of the child processes
    int status; // Declare a variable to store the exit status of the child processes when they terminate

    // Attempts to create a pipe. If the pipe cannot be created,
    if (pipe(pipefd) == -1) {
        perror("ERROR: pipe failed to be created"); // Print an error message
        exit(1); // Exit with an error code to indicate failure
    }

    command_one_pid = fork(); // Creates a child process for the first command

    // If this is the child process,
    if (command_one_pid == 0) {
        close(pipefd[0]); // Close the read end of the pipe
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to the write end of the pipe
        close(pipefd[1]); // Close the right end of the pipe

        // Attempts to execute the first command. If the command cannot be executed,
        if (execvp(command_one_args[0], (char *const *)command_one_args) == -1) {
            perror("ERROR: execvp failed to run the first command"); // Print an error message
            exit(1); // Exit with an error code to indicate failure
        }
    }

    // If fork fails for the first command,
    else if (command_one_pid < 0) {
        perror("ERROR: fork failed the execute the first command"); // Print an error message
        exit(1); // Exit with an error code to indicate failure
    }

    command_two_pid = fork(); // Creates a child process for the second command

    // If this is the child process,
    if (command_two_pid == 0) {
        close(pipefd[1]); // Close the write end of the pipe
        dup2(pipefd[0], STDIN_FILENO); // Redirect stdin to the read end of the pipe
        close(pipefd[0]); // Close the read end of the pipe

        // If there is an input file to handle,
        if (input_file) {
            int input_fd = open(input_file, O_RDONLY); // Open the file and set the file descriptor

            // If the file cannot be opened,
            if (input_fd == -1) {
                perror("ERROR: opening the input file failed"); // Print an error message
                exit(1); // Exit with an error code to indicate failure
            }

            dup2(input_fd, STDIN_FILENO); // Duplicate the file descriptor to standard input
            close(input_fd); // Close the input file descriptor
        }

        // If there is an output file to handle,
        if (output_file) {
            int output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644); // Open the file and set the file descriptor

            // If the file cannot be opened,
            if (output_fd == -1) {
                perror("ERROR: opening the output file failed"); // Print an error message
                exit(1); // Exit with an error code to indicate failure
            }

            dup2(output_fd, STDOUT_FILENO); // Duplicate the file descriptor to standard output
            close(output_fd); // Close the output file descriptor
        }

        // Attempts to execute the second command. If the command cannot be executed,
        if (execvp(command_two_args[0], (char *const *)command_two_args) == -1) {
            perror("ERROR: execvp failed to run the second command"); // Print an error message
            exit(1); // Exit with an error code to indicate failure
        }
    }

    // If fork fails for the second command,
    else if (command_two_pid < 0) {
        perror("ERROR: fork failed the execute the first command"); // Print an error message
        exit(1); // Exit with an error code to indicate failure
    }

    // Close both ends of the pipe in the parent process
    close(pipefd[0]);
    close(pipefd[1]);

    // Wait for the two child processes (the first and second commands) to finish execution and store their exit status
    waitpid(command_one_pid, &status, 0);
    waitpid(command_two_pid, &status, 0);
}

// START OF THE BUILT-IN COMMANDS SECTION

/**
 * Changes the current working directory of the shell to the path specified as the argument.
 *
 * @param path A pointer to a null-terminated string that represents the directory path to which the current
 * working directory should be changed.
 *
 * @return 0 for success, 1 for an error (e.g., if the specified directory does not exist).
 */
int cd(const char *path) {
    // Attempt to change the directory using the chdir function.
    if (chdir(path) != 0) {
        perror("ERROR: Directory does not exist"); // Print an error message
        return 1; // Return an exit code of 1 to indicate failure
    }
    return 0; // Return an exit code of 0 to indicate success
}

/**
 * Executes each line of the given file as a command.
 *
 * @param filename A pointer to a null-terminated string representing the name of the file to read.
 *                 The function will execute each line from this file as a command, as if it was entered
 *                 by the user at the prompt
 */
void source(const char* filename) {
    // Declares a file pointer and opens the given file for reading
    FILE *file = fopen(filename, "r");

    // If the file was not successfully opened,
    if (file == NULL) {
        perror("ERROR: fopen failed in source"); // Print an error message
        return; // Return
    }

    char line[MAX_INPUT_LENGTH]; // Define a buffer of size 255 to read each line of the file

    // Iterates over the lines of the file, reading them until the end of the file is reached
    while (fgets(line, sizeof(line), file) != NULL) {
        size_t length = strlen(line); // Stores the length of the current line

        // Removes the newline character from the end of the line
        if (length > 0 && line[length - 1] == '\n') {
            line[length - 1] = '\0';
        }

        vect_t *tokenizedLine; // Declare a vector to hold each tokenized line
        tokenize(line, &tokenizedLine); // Tokenize the line

        // Allocate memory to store the arguments obtained from the token vector
        const char **args = (const char **)malloc((vect_size(tokenizedLine) + 1) * sizeof(char *));

        // Fill the argument array with pointers to each token
        for (unsigned int i = 0; i < vect_size(tokenizedLine); i++) {
            args[i] = vect_get(tokenizedLine, i);
        }

        // Null terminate the argument array
        args[vect_size(tokenizedLine)] = NULL;

        // Execute the argument array
        execute(args, NULL, NULL);

        vect_delete(tokenizedLine); // Free all the memory used by the tokens
        free(args); // Free the memory used by the argument array
    }

    // Close the file once all lines have been processed
    fclose(file);
}

/**
 * Prints the previous command line and executes it again.
 *
 * @param prev_command A pointer to a null-terminated string that represents the previous command
 *                     to be printed and executed. If set to NULL, no action is taken.
 */
void prev(const char* prev_command) {

    // If the previous command is not NULL,
    if (prev_command != NULL) {
        printf("Previous command: %s\n", prev_command); // Print a message containing the previous command

        char* args[4]; // Declares an array of size 4 to be used to pass arguments to the execvp function
        args[0] = "sh"; // Set the first argument to the string sh to specify which shell should execute the command
        args[1] = "-c"; // Set the second argument to the string -c to indicate that the following command should be executed by the shell
        args[2] = (char*)prev_command; // Set the third argument to the previous command (casted as a character pointer)
        args[3] = NULL; // Set the fourth argument to NULL to indicate the end of the argument list

        // Execute the previous command. If it could not be executed,
        if (execvp("sh", args) == -1) {
            perror("ERROR: execvp failed in prev"); // Print an error
            exit(1); // Return an exit code of 1 to indicate failure
        }
    }
}

/**
 * Explains all the built-in commands available in our shell.
 */
void help() {
    printf("cd: Changes the current working directory of the shell to the path specified as the argument.\n");
    printf("source: Executes each line of the given file as a command.\n");
    printf("prev: Prints the previous command line and executes it again.\n");
    printf("help: Explains all the built-in commands available in our shell.\n");
}

// END OF THE BUILT-IN COMMANDS SECTION

int main(int argc, char **argv) {
    printf("Welcome to mini-shell.\n"); // Prints the welcome message
    char input[MAX_INPUT_LENGTH];  // Declare an array to store user input
    vect_t *tokens; // Declare a vector to store the tokenized input

    char previous_command[MAX_INPUT_LENGTH]; // Declare an array to store the previous command entered by the user
    char *prev_command = NULL; // Initializes a pointer to be used to track the previous command

    // Starts an infinite loop, where the shell continually waits for user input and processes it
    while (1) {
        printf("shell $ "); // Print the shell prompt

        // If the user presses Ctrl-D (end-of-file),
        if (fgets(input, MAX_INPUT_LENGTH, stdin) == NULL) {
            printf("Bye bye.\n"); // Print the exit message
            break; // Exit the shell
        }

        input[strcspn(input, "\n")] = '\0'; // Remove the newline character at the end of the input string

        // If the help command is called,
        if (strcmp(input, "help") == 0) {
            help();
        } 

        // If the cd command is called,
        else if (strncmp(input, "cd ", 3) == 0) {
            // Change directory command
            const char *directory = input + 3;
            if (cd(directory) != 0) {
                fprintf(stderr, "Failed to change directory to %s\n", directory);
            }
        } 

        // If the prev command is called,
        else if (strcmp(input, "prev") == 0) {
            // Execute the previous command
            if (prev_command != NULL) {
                prev(prev_command);
            } else {
                printf("No previous command to execute.\n");
                continue;
            }
        }

        // If the source command is called,
        else if (strncmp(input, "source ", 7) == 0) {
            const char *filename = input + 7;
            if (filename != NULL) {
                source(filename);
            } else {
                printf("Missing filename after 'source' command.\n");
            }
        }

        // Otherwise, 
        else {
            char *command = strtok(input, ";");  // Tokenize any commands that are separated by a semicolon

            // While there is a command to run,
            while (command != NULL) {
                char *input_file = NULL; // Set the input file to NULL
                char *output_file = NULL; // Set the output file to NULL

                strcpy(previous_command, command); // Set the previous command variable to the current command
                prev_command = previous_command; // Copy the current command for later use

                // Set variables to indicate whether there is input/output redirection or piping to handle
                char *input_redirect = strstr(command, "<");
                char *output_redirect = strstr(command, ">");
                char *pipe_operator = strstr(command, "|");

                // Handle input redirection
                if (input_redirect) {
                    *input_redirect = '\0'; // Null-terminates the command string at the position where the '<' character was found
                    input_file = strtok(input_redirect + 1, " "); // Sets the input file to the given file name
                }

                // Handle output redirection
                if (output_redirect) {
                    *output_redirect = '\0'; // Null-terminates the command string at the position where the '>' character was found
                    output_file = strtok(output_redirect + 1, " "); // Sets the output file to the given file name
                }

                // Handle piping
                if (pipe_operator) {
                    *pipe_operator = '\0'; // Replace the pipe character with a null terminator to split the command
                    
                    tokenize(command, &tokens); // Tokenize the first command
                    
                    // Allocate memory to hold the arguments of the first command
                    const char **command_one_args = (const char **)malloc((vect_size(tokens) + 1) * sizeof(char *));
                    
                    // Copy each token from the tokens vector to the command one array
                    for (unsigned int i = 0; i < vect_size(tokens); i++) {
                        command_one_args[i] = vect_get(tokens, i);
                    }
                    
                    // Null terminate the command one array
                    command_one_args[vect_size(tokens)] = NULL;

                    tokenize(pipe_operator + 1, &tokens); // Tokenize the second command
                    
                    // Allocate memory to hold the arguments of the second command
                    const char **command_two_args = (const char **)malloc((vect_size(tokens) + 1) * sizeof(char *));
                    
                    // Copy each token from the tokens vector to the command two array
                    for (unsigned int i = 0; i < vect_size(tokens); i++) {
                        command_two_args[i] = vect_get(tokens, i);
                    }
                    
                    // Null terminate the command two array
                    command_two_args[vect_size(tokens)] = NULL;

                    // Execute the piped command
                    execute_piped(command_one_args, command_two_args, input_file, output_file);

                    free(command_one_args); // Free the memory allocated by the pipe for the first command
                    free(command_two_args); // Free the memory allocated by the pipe for the second command
                } 
                
                // If there are no advanced shell features handle,
                else {
                    // Tokenize the individual command
                    tokenize(command, &tokens);
                    
                    // Allocate memory to store the arguments of the command
                    const char **args = (const char **)malloc((vect_size(tokens) + 1) * sizeof(char *));
                    
                    // Copy the arguments of the tokens vector to the argument array
                    for (unsigned int i = 0; i < vect_size(tokens); i++) {
                        args[i] = vect_get(tokens, i);
                    }
                    
                    // Null terminate the arguments array
                    args[vect_size(tokens)] = NULL;

                    // Execute the command with the arguments
                    execute(args, input_file, output_file);

                    // Free memory used by the tokens and arguments
                    for (unsigned int i = 0; i < vect_size(tokens); i++) {
                        char *token_copy = vect_get_copy(tokens, i);
                        free(token_copy);
                    }
                    
                    free(args); // Free the memory used by the argument array
                }

                vect_delete(tokens); // Free all the memory used by the tokens

                command = strtok(NULL, ";"); // Get the next command
            }
        }
    }

    return 0; // Return 0 to indicate success
}
