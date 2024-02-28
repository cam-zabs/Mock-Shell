#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vect.h"
#include "tokens.h"

// Entry point of the program for tokenizing and printing the output
int main(int argc, char **argv) {
    char input[MAX_INPUT_LENGTH]; // Declare an array of length 255 to store user input

    // Check if the input can be read from standard input
    if (fgets(input, MAX_INPUT_LENGTH, stdin) != NULL) {
        vect_t *tokens; // Declare a pointer to a vector for storing tokens
        tokenize(input, &tokens); // Tokenize the input and store those tokens in the vector

        // Iterate through the tokens and print each one, followed by a new line
        for (unsigned int i = 0; i < vect_size(tokens); i++) {
            const char *token = vect_get(tokens, i);
            printf("%s\n", token);
        }

        // Free memory used by the individual tokens
        for (unsigned int i = 0; i < vect_size(tokens); i++) {
            char *token_copy = vect_get_copy(tokens, i);
            free(token_copy);
        }

        vect_delete(tokens); // Free memory used by the token vector
    }

    return 0; // Return 0 to indicate success
}
