/*
 * util.h
 * mmn14
 * Tzur Pinto Lazar
 */

#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include "errors.h"

typedef struct {
    char *name;
    int opcode;
    int funct;
    InstType type;
} Opcode;

/*
 * getOpcode func
 * retrieves the opcode details for a given instruction name.
 * the input is the name of the instruction.
 * returns a pointer to the Opcode struct if found, or NULL otherwise.
 */
Opcode *getOpcode(const char *name);

/*
 * isReservedKeyword func
 * checks if a given name is a reserved keyword in the assembler.
 * the input is the name to check.
 * returns TRUE if it is a reserved keyword, FALSE otherwise.
 */
boolean isReservedKeyword(const char *name);

/*
 * checkLabelN func
 * checks if a given string follows the valid format for a label.
 * the input is the name of the label.
 * returns TRUE if the format is valid, FALSE otherwise.
 */
boolean checkLabelN(const char *name);

/*
 * safeMalloc func
 * allocates memory safely and exits the program if allocation fails.
 * the input is the size of memory to allocate.
 * returns a pointer to the allocated memory.
 */
void *safeMalloc(size_t size);

/*
 * safeRealloc func
 * reallocates memory safely and exits the program if reallocation fails.
 * the input is a pointer to the previously allocated memory and the new size.
 * returns a pointer to the newly allocated memory.
 */
void *safeRealloc(void *ptr, size_t size);

/*
 * strdupp func
 * duplicates a string safely by allocating memory and copying the content.
 * the input is the string to duplicate.
 * returns a pointer to the newly allocated duplicated string.
 */
char *strdupp(const char *s);

/*
 * skipSpaces func
 * advances a string pointer past any leading whitespace characters.
 * the input is a pointer to the string pointer.
 * returns void.
 */
void skipSpaces(char **str);

/*
 * isEmptyLine func
 * checks if a line consists entirely of whitespace characters.
 * the input is the string to check.
 * returns TRUE if the line is empty or only whitespace, FALSE otherwise.
 */
boolean isEmptyLine(const char *str);

/*
 * isCommentLine func
 * checks if a line is a comment, indicated by a leading semicolon.
 * the input is the string to check.
 * returns TRUE if the line is a comment, FALSE otherwise.
 */
boolean isCommentLine(const char *str);

/*
 * getToken func
 * extracts the next token from a string, separated by whitespace or commas.
 * the input is a pointer to the string pointer.
 * returns a newly allocated string containing the token, or NULL if no token is found.
 */
char *getToken(char **str);

/*
 * getRegNum func
 * extracts the register number from a register token string.
 * the input is the register token string (e.g. "$4").
 * returns the register number if valid, or -1 otherwise.
 */
int getRegNum(const char *token);

/*
 * matchComma func
 * checks for and consumes a comma in the string, reporting an error if missing.
 * the input is a pointer to the string pointer, error list, and line number.
 * returns TRUE if a comma was found, FALSE otherwise.
 */
boolean matchComma(char **ptr, ErrorNode **errorList, int lineNum);

/*
 * checkRegOperand func
 * extracts a register operand from the string and validates it.
 * the input is a pointer to the string pointer, error list, and line number.
 * returns the valid register number, or -1 if invalid or missing.
 */
int checkRegOperand(char **ptr, ErrorNode **errorList, int lineNum);

/*
 * checkImmedOperand func
 * extracts an immediate value operand from the string.
 * the input is a pointer to the string pointer, error list, line number, and error flag.
 * returns the immediate value, or 0 if missing (sets error flag).
 */
short checkImmedOperand(char **ptr, ErrorNode **errorList, int lineNum, boolean *lineError);

/*
 * checkLabelOperand func
 * extracts a label operand from the string.
 * the input is a pointer to the string pointer, error list, line number, and error flag.
 * returns a newly allocated string containing the label, or NULL if missing (sets error flag).
 */
char *checkLabelOperand(char **ptr, ErrorNode **errorList, int lineNum, boolean *lineError);

/*
 * checkExtraText func
 * checks if there is any extraneous text remaining in the string and reports an error if so.
 * the input is a pointer to the string pointer, error list, line number, and error flag.
 * returns void.
 */
void checkExtraText(char **ptr, ErrorNode **errorList, int lineNum, boolean *lineError);

/*
 * checkLineLen func
 * checks if the length of a line is within the allowed limit (max MAX_LINE_CHARS chars).
 * the input is the string line to check.
 * returns TRUE if the length is valid, FALSE otherwise.
 */
boolean checkLineLen(const char *line);

/*
 * isLabelDef func
 * checks if a token string ends with a colon, indicating it is a label definition.
 * the input is the token string to check.
 * returns TRUE if it is a label definition, FALSE otherwise.
 */
boolean isLabelDef(const char *token);

#endif
