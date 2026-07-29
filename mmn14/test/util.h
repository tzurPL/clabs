/*
 * util.h
 * mmn14
 * Tzur Pinto Lazar
 */

#ifndef UTIL_H
#define UTIL_H

#include "errors.h"
#include "globals.h"
#include <stdio.h>
#include <stdlib.h>

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
 * getReg func
 * extracts a register op from the string
 * the input is a pointer to the string pointer, error list, and line num
 * returns the valid register num or -1 if not valid or missing.
 */
int getReg(char **ptr, ErrorNode **errorList, int lineNum);

/*
 * getImmed func
 * extracts immed val from string
 * the input is a pointer to the string pointer, error list, line number, and error flag
 * returns immed val or 0 if missing and turn on the err flag
 */
short getImmed(char **ptr, ErrorNode **errorList, int lineNum, boolean *lineError);

/*
 * getLabel func
 * extracts a label operand from the string
 * the input is a pointer to the string pointer, error list, line number, and error flag
 * returns a new string with the label or null if missing and put error flag
 */
char *getLabel(char **ptr, ErrorNode **errorList, int lineNum, boolean *lineError);

/*
 * checkExtraText func
 * checks if there is any extra text in the string and put an error if yes
 * the input is a pointer to the string pointer, error list, line number, and error flag
 * returns void
 */
void checkExtraText(char **ptr, ErrorNode **errorList, int lineNum, boolean *lineError);

/*
 * checkLineLen func
 * checks if the length of a line is max 80 chars
 * the input is the string line to check
 * returns true if the length is valid false if no
 */
boolean checkLineLen(const char *line);

/*
 * isLabelDef func
 * checks if a token string ends with a : indicating it is a label def
 * the input is the token string to check
 * returns true if it is a label def false if no
 */
boolean isLabelDef(const char *token);

/*
 * remAsExtension func
 * removes the .as from a filename if exists
 * the input is the string to where the extension from
 * returns void.
 */
void remAsExtension(char *filename);

#endif
