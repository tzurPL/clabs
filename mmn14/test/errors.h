#ifndef ERRORS_H
#define ERRORS_H

#include "globals.h"

/* error types for better reporting */
typedef enum {
    ERR_OPEN_FILE,
    ERR_ALLOC_FAIL,
    ERR_LINE_TOO_LONG,
    ERR_UNDEFINED_MACRO,
    ERR_MACRO_REDEFINITION,
    ERR_SYMBOL_REDEFINITION,
    ERR_RESERVED_KEYWORD,
    ERR_UNDEFINED_SYMBOL,
    ERR_UNKNOWN_COMMAND,
    ERR_INVALID_REG,
    ERR_INVALID_IMMED,
    ERR_MISSING_COMMA,
    ERR_ILLEGAL_COMMA,
    ERR_MULTIPLE_COMMAS,
    ERR_EXTRA_TEXT,
    ERR_ENTRY_NOT_FOUND,
    ERR_BRANCH_TOO_FAR
} ErrorType;

typedef struct ErrorNode {
    int lineNum;
    ErrorType type;
    char *extraInfo;
    struct ErrorNode *next;
} ErrorNode;

/* addError func: collects an error into a list to be printed later. */
void addError(ErrorNode **head, int lineNum, ErrorType type, const char *extraInfo);

/* printErrors func: sorts and prints all collected errors. */
void printErrors(const char *filename, ErrorNode *head);

/* freeErrors func: cleans up the error list. */
void freeErrors(ErrorNode *head);

/* printError for critical/simple errors that shouldn't be collected */
void printError(const char *filename, int lineNum, ErrorType type, const char *extraInfo);

#endif
