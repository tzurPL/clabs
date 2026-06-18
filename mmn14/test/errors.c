#include "errors.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void addError(ErrorNode **head, int lineNum, ErrorType type, const char *extraInfo) {
    ErrorNode *newNode = (ErrorNode *)safeMalloc(sizeof(ErrorNode));
    newNode->lineNum = lineNum;
    newNode->type = type;
    newNode->extraInfo = extraInfo ? duplicateString(extraInfo) : NULL;
    newNode->next = NULL;

    if (!*head) *head = newNode;
    else {
        ErrorNode *curr = *head;
        while (curr->next) curr = curr->next;
        curr->next = newNode;
    }
}

void freeErrors(ErrorNode *head) {
    while (head) {
        ErrorNode *temp = head;
        head = head->next;
        if (temp->extraInfo) free(temp->extraInfo);
        free(temp);
    }
}

void swapErrors(ErrorNode *a, ErrorNode *b) {
    int tempLine = a->lineNum;
    ErrorType tempType = a->type;
    char *tempInfo = a->extraInfo;

    a->lineNum = b->lineNum;
    a->type = b->type;
    a->extraInfo = b->extraInfo;

    b->lineNum = tempLine;
    b->type = tempType;
    b->extraInfo = tempInfo;
}

void printError(const char *filename, int lineNum, ErrorType type, const char *extraInfo) {
    if (filename) {
        fprintf(stderr, "[%s", filename);
        if (lineNum > 0) fprintf(stderr, ":%d", lineNum);
        fprintf(stderr, "] ");
    }

    fprintf(stderr, "Error: ");

    if (type == ERR_OPEN_FILE) fprintf(stderr, "Could not open file");
    else if (type == ERR_ALLOC_FAIL) fprintf(stderr, "Not enough memory");
    else if (type == ERR_LINE_TOO_LONG) fprintf(stderr, "Line is too long (max 80 chars)");
    else if (type == ERR_UNDEFINED_MACRO) fprintf(stderr, "Undefined macro");
    else if (type == ERR_MACRO_REDEFINITION) fprintf(stderr, "Macro redefinition");
    else if (type == ERR_SYMBOL_REDEFINITION) fprintf(stderr, "Symbol redefinition");
    else if (type == ERR_RESERVED_KEYWORD) fprintf(stderr, "Reserved keyword used as a label or macro name");
    else if (type == ERR_UNDEFINED_SYMBOL) fprintf(stderr, "Undefined label '%s'", extraInfo);
    else if (type == ERR_UNKNOWN_COMMAND) fprintf(stderr, "Unknown command/directive");
    else if (type == ERR_INVALID_REG) fprintf(stderr, "Invalid register name");
    else if (type == ERR_INVALID_IMMED) fprintf(stderr, "Invalid immediate value");
    else if (type == ERR_MISSING_COMMA) fprintf(stderr, "Missing comma between parameters");
    else if (type == ERR_ILLEGAL_COMMA) fprintf(stderr, "Illegal comma");
    else if (type == ERR_MULTIPLE_COMMAS) fprintf(stderr, "Multiple consecutive commas");
    else if (type == ERR_EXTRA_TEXT) fprintf(stderr, "Extraneous text after command");
    else if (type == ERR_ENTRY_NOT_FOUND) fprintf(stderr, "Entry symbol was not defined");
    else if (type == ERR_BRANCH_TOO_FAR) fprintf(stderr, "Branch offset is out of range or illegal");
    else fprintf(stderr, "Unknown error occurred");

    if (type != ERR_UNDEFINED_SYMBOL && extraInfo) {
        fprintf(stderr, " - %s", extraInfo);
    }

    fprintf(stderr, "\n");
}

void printErrors(const char *filename, ErrorNode *head) {
    ErrorNode *i, *j;
    for (i = head; i != NULL; i = i->next) {
        for (j = i->next; j != NULL; j = j->next) {
            if (i->lineNum > j->lineNum) {
                swapErrors(i, j);
            }
        }
    }

    while (head) {
        printError(filename, head->lineNum, head->type, head->extraInfo);
        head = head->next;
    }
}
