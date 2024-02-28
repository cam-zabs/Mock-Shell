// A header file that declares the tokenize function

#ifndef _TOKENS_H
#define _TOKENS_H

#include "vect.h" // Include the vect library from assignment 4

#define MAX_INPUT_LENGTH 255 // Define the maximum input string length to be 255

/**
 * Splits up an input line into meaningful tokens
 *
 * The tokens (, ), <, >, ;, |, and the whitespace characters (space ' ', tab '\t', newline '\n') are special
 * Whitespace is not a token, but might separate tokens
 *
 * @param input The input string to be tokenized
 * @param tokens A pointer to a string vector where the tokens will be stored
 */
void tokenize(const char *input, vect_t **tokens);

#endif
