// A source file that defines the tokenize function

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vect.h"
#include "tokens.h"

// Splits up an input line into meaningful tokens
void tokenize(const char *input, vect_t **tokens) {
    *tokens = vect_new(); // Create a new string vector to store tokens
    int i = 0; // Initialize an index to be used when traversing the input string

    // While the end of the input string is not reached, tokenize the input
    while (input[i] != '\0') {

        // If the current character is a whitespace character, skip it and move to the next character
        if (input[i] == ' ' || input[i] == '\t' || input[i] == '\n') {
            i++;
        }

        // If the current character is '(', add it as a token and move to the next character
        else if (input[i] == '(') {
            vect_add(*tokens, "(");
            i++;
        }

        // If the current character is ')', add it as a token and move to the next character
        else if (input[i] == ')') {
            vect_add(*tokens, ")");
            i++;
        }

        // If the current character is '<', add it as a token and move to the next character
        else if (input[i] == '<') {
            vect_add(*tokens, "<");
            i++;
        }

        // If the current character is '>', add it as a token and move to the next character
        else if (input[i] == '>') {
            vect_add(*tokens, ">");
            i++;
        }

        // If the current character is ';', add it as a token and move to the next character
        else if (input[i] == ';') {
            vect_add(*tokens, ";");
            i++;
        }

        // If the current character is '|', add it as a token and move to the next character
        else if (input[i] == '|') {
            vect_add(*tokens, "|");
            i++;
        }

        // If the current character is '"', process the quoted string
        else if (input[i] == '"') {
            i++; // Move the index to the first character after the quote
            char quoted[256]; // Create an array to hold the quoted string
            int j = 0; // Initialize an index for the quoted array

            // Start a loop that continues until either the ending quote is encountered or the end of the input string is reached
            while (input[i] != '"' && input[i + 1] != '\0') {
                quoted[j] = input[i]; // Copy the character from the input string to the quoted array
                i++; // Move to the next character in the input string
                j++; // Move to the next index in the quoted array
            }

            // If an ending quote is found,
            if (input[i] == '"') {
                quoted[j] = '\0'; // Null terminate the quoted string to mark the end
                vect_add(*tokens, quoted); // Add the quoted string as a token to the token vector
                i++; // Move the index to the character in the input string that follows the closing quote
            }

            // If an ending quote was not found,
            else {
                fprintf(stderr, "ERROR: Unmatched double quote.\n"); // Print an error
                exit(1); // Exit with an error code to indicate failure
            }
        }

        // If no tokens were recognized, treat this character as the start of a word
        else {
            char word[256]; // Create an array to hold the word
            int k = 0; // Initialize an index for the word array

            // Start a loop that continues until a token is encountered or the end of the input string is reached
            while (input[i] != '(' && input[i] != ')' && input[i] != '<' && input[i] != '>'
                   && input[i] != ';' && input[i] != '|' && input[i] != '"' && input[i] != ' '
                   && input[i] != '\t' && input[i] != '\n' &&  input[i] != '\0') {

                word[k] = input[i]; // Copy the word to the string vector
                i++;
                k++;
            }

            // Once the end of the word is reached,
            word[k] = '\0'; // Null terminate the word string to mark the end
            vect_add(*tokens, word); // Add the word string as a token to the token vector
        }
    }
}
