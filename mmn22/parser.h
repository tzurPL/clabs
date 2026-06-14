/*
 * parser.h
 * maman22
 */

#ifndef PARSER_H
#define PARSER_H

#include "complex.h"

/*
 * Matches a given character to its complex variable from the array
 * the input is a character representing the variable name and the array of complex variables
 * returns a pointer to the matched complex variable or NULL if it isnt valid
 */
complex* getVar(char name, complex *variables);

/*
 * goes over the string until it hit a non space character and advances the pointer to it
 * the input is a pointer to the string input
 * returns void
 */
void skipSpaces(char **str);


/*
 * checks and finds a valid complex variable from the input.
 * moves foward the pointer past the variable and exits with error if invalid or missing.
 * the input is a pointer to the input string pointer and a pointer to store the variable and the variables array.
 * returns 1 if valid variable was found. 0 otherwise.
 */
int checkVar(char **str, complex **var, complex *variables);

/*
 * checks the string input to find a valid num.
 * moves foward the pointer past the number and exits with error if the format is invalid or if it is missing.
 * the input is pointer to the input string pointer and a pointer to a double to store the num.
 * returns 1 if valid number was found. 0 otherwise.
 */
int checkNum(char **str, double *num);

/*
 * Checks if the current character in the input string is a comma.
 * moves foward the pointer past the comma if found or exist with a missing comma error
 * the input is a pointer to the input string pointer
 * returns 1 if a comma was  found and skipped. 0 otherwise.
 */
int checkComma(char **str);

/*
 * Checks the current position in the string to check if there is no illegal consecutive commas.
 * exits with a multiple commas error if an extra comma is found
 * the input is a pointer to the input string pointer.
 * returns 1 if no extra comma is found. 0 otherwise
 */
int checkNoExtraComma(char **str);

/*
 * checks that there is no extraneous text at the end of the command input.
 * exits with an error if there is extraneous text after the command
 * the input is a pointer to the input string pointer.
 * returns 1 if the string ends when it should. 0 otherwise
 */
int checkEnd(char **str);

#endif /* PARSER_H */
