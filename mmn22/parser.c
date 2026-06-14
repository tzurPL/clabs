/*
 * parser.c
 * maman22
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

/*
 * Matches a given character to its complex variable from the array
 * the input is a character representing the variable name and the array of complex variables
 * returns a pointer to the matched complex variable or NULL if it isnt valid
 */
complex* getVar(char name, complex *variables) {
    if (name == 'A') return &variables[0];
    if (name == 'B') return &variables[1];
    if (name == 'C') return &variables[2];
    if (name == 'D') return &variables[3];
    if (name == 'E') return &variables[4];
    if (name == 'F') return &variables[5];
    return NULL;
}

/*
 * goes over the string until it hit a non space character and moves foward the pointer to it
 * the input is a pointer to the string input pointer
 * returns void
 */
void skipSpaces(char **str) {
    /* go over the string until it hit a non space character*/
    while (**str != '\0' && isspace(**str)) {
        (*str)++;
    }
}

/*
 * checks and takes a valid complex variable from the input.
 * moves foward the pointer past the variable and exits with error if invalid or missing.
 * the input is a pointer to the input string pointer and a pointer to store the variable and the variables array.
 * returns 1 if valid variable was found. 0 otherwise.
 */
int checkVar(char **str, complex **var, complex *variables) {
    skipSpaces(str);
    if (**str == '\0' || **str == ',') {/* cheack if it hit end or , meaning that the user forgot the variable*/
        fprintf(stderr, "Missing parameter\n");
        return 0;
    }
    if (isalpha(**str)) {/*check if it is a character*/
        *var = getVar(**str, variables);/*get the desired variavble from the getvar func*/
        (*str)++;/*continue*/
        if (!isspace(**str) && **str != ',' && **str != '\0') {/*if the char its at is still not a space/,/end*/
            while (**str != '\0' && !isspace(**str) && **str != ',') (*str)++;/* skip the non official variable characters so to not ruin the rest*/
            fprintf(stderr, "Undefined complex variable\n");/*go with error*/
            return 0;
        }
        if (*var == NULL) {/*if the character that is supposed to be an official variable is not in the list*/
            fprintf(stderr, "Undefined complex variable\n");
            return 0;
        }
        return 1;
    }
    while (**str != '\0' && !isspace(**str) && **str != ',') (*str)++;/* if there is numbers or other non alpha non variable characters after skip*/
    fprintf(stderr, "Undefined complex variable\n");
    return 0;
}

/*
 * checks the string input to find a valid num.
 * moves foward the pointer past the number and exits with error if the format is invalid or if it is missing.
 * the input is pointer to the input string pointer and a pointer to a double to store the num.
 * returns 1 if valid number was found. 0 otherwise.
 */
int checkNum(char **str, double *num) {
    char *endptr;/*pointer for when the strtod function collects the num, to continume scanning the input*/
    skipSpaces(str);
    if (**str == '\0' || **str == ',') {/*check for end of string or for comma to check if it was skipped */
        fprintf(stderr, "Missing parameter\n");
        return 0;
    }
    *num = strtod(*str, &endptr);/*collecting the num and setting the pointer to after it in the input*/
    if (endptr == *str) {/*if the original pointer on the input is equal to the one for the new position then its an invalid parameter*/
        while (**str != '\0' && !isspace(**str) && **str != ',') (*str)++;/*take the original pointer to the next thing to check in the input*/
        fprintf(stderr, "Invalid parameter - not a number\n");/*exit with error*/
        return 0;
    }

    if (*endptr != '\0' && !isspace(*endptr) && *endptr != ',') {/*if after the number there is not a space/, like it should be then it is illigal*/
            /*skip to after the illigal thing*/
            while (*endptr != '\0' && !isspace(*endptr) && *endptr != ',') endptr++;
            *str = endptr;/*set the pointer that goes over the input to after the illegal thing*/
            fprintf(stderr, "Invalid parameter - not a number\n");/*exit with error*/
            return 0;
    }


    *str = endptr;/*set the pointer that goes over the input to after the number*/
    return 1;
}

/*
 * Checks if the current character in the input string is a comma.
 * moves foward the pointer past the comma if found or exist with a missing comma error
 * the input is a pointer to the input string pointer
 * returns 1 if a comma was  found and skipped. 0 otherwise.
 */
int checkComma(char **str) {
    skipSpaces(str);
    if (**str == ',') {/*check if it is a comma*/
        (*str)++;/*go to the next char after it*/
        return 1;
    }
    fprintf(stderr, "Missing comma\n");/*if not exit with error*/
    return 0;
}

/*
 * Checks the current position in the string to check if there is no illegal consecutive commas.
 * exits with a multiple commas error if an extra comma is found
 * the input is a pointer to the input string pointer.
 * returns 1 if no extra comma is found. 0 otherwise
 */
int checkNoExtraComma(char **str) {
    skipSpaces(str);
    if (**str == ',') {/*if there is a comma again*/
        fprintf(stderr, "Multiple consecutive commas\n");/*exit with error*/
        return 0;
    }
    return 1;
}

/*
 * checks that there is no extraneous text at the end of the command input.
 * exits with an error if there is extraneous text after the command
 * the input is a pointer to the input string pointer.
 * returns 1 if the string ends when it should. 0 otherwise
 */
int checkEnd(char **str) {
    skipSpaces(str);
    if (**str == '\0') return 1;/*if there is nothing after skipping spaces meaning nothing after the command*/
    fprintf(stderr, "Extraneous text after end of command\n");/*if not exit with error*/
    return 0;
}
