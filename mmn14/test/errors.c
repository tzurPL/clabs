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

static void swapErrors(ErrorNode *a, ErrorNode *b) {
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

    switch (type) {
        case ERR_OPEN_FILE: fprintf(stderr, "Could not open file"); break;
        case ERR_ALLOC_FAIL: fprintf(stderr, "Not enough memory"); break;
        case ERR_LINE_TOO_LONG: fprintf(stderr, "Line is too long (max 80 chars)"); break;
        case ERR_UNDEFINED_MACRO: fprintf(stderr, "Undefined macro"); break;
        case ERR_MACRO_REDEFINITION: fprintf(stderr, "Macro redefinition"); break;
        case ERR_SYMBOL_REDEFINITION: fprintf(stderr, "Symbol redefinition"); break;
        case ERR_RESERVED_KEYWORD: fprintf(stderr, "Reserved keyword used as a label or macro name"); break;
        case ERR_UNDEFINED_SYMBOL: fprintf(stderr, "Undefined label '%s' (caught in Pass 2)", extraInfo); break;
        case ERR_UNKNOWN_COMMAND: fprintf(stderr, "Unknown command/directive"); break;
        case ERR_INVALID_REG: fprintf(stderr, "Invalid register name"); break;
        case ERR_INVALID_IMMED: fprintf(stderr, "Invalid immediate value"); break;
        case ERR_MISSING_COMMA: fprintf(stderr, "Missing comma between parameters"); break;
        case ERR_ILLEGAL_COMMA: fprintf(stderr, "Illegal comma"); break;
        case ERR_MULTIPLE_COMMAS: fprintf(stderr, "Multiple consecutive commas"); break;
        case ERR_EXTRA_TEXT: fprintf(stderr, "Extraneous text after command"); break;
        case ERR_ENTRY_NOT_FOUND: fprintf(stderr, "Entry symbol was not defined"); break;
        case ERR_BRANCH_TOO_FAR: fprintf(stderr, "Branch offset is out of range or illegal"); break;
        default: fprintf(stderr, "Unknown error occurred");
    }

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
