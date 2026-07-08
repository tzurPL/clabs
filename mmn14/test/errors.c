/*
 * errors.c
 * mmn14
 * Tzur Pinto Lazar
 */

#include "errors.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * addError func
 * adds a new error node to the end of the error list.
 * the input is a pointer to the head of the error list, line number, error type, and optional extra information string.
 * returns void.
 */
void addError(ErrorNode **head, int lineNum, ErrorType type, const char *extraInfo) {
    ErrorNode *newNode = (ErrorNode *)safeMalloc(sizeof(ErrorNode));/*allocate memory for new error*/
    newNode->lineNum = lineNum;/*set line number*/
    newNode->type = type;/*set error type*/
    newNode->extraInfo = extraInfo ? strdupp(extraInfo) : NULL;/*duplicate extra info if provided*/
    newNode->next = NULL;

    if (!*head) {
        *head = newNode;/*if list is empty, set as head*/
    } else {
        ErrorNode *curr = *head;
        while (curr->next) {
            curr = curr->next;/*find the end of the list*/
        }
        curr->next = newNode;/*append the new error node*/
    }
}

/*
 * freeErrors func
 * frees the memory allocated for all nodes in the error list.
 * the input is the head of the error list.
 * returns void.
 */
void freeErrors(ErrorNode *head) {
    while (head) {/*iterate through the list*/
        ErrorNode *temp = head;
        head = head->next;/*move to next node*/
        if (temp->extraInfo) { free(temp->extraInfo);/*free extra info string*/ }
        free(temp);/*free the node itself*/
    }
}

/*
 * swapErrors func
 * swaps the contents of two error nodes for sorting purposes.
 * the input is two error nodes to be swapped.
 * returns void.
 */
void swapErrors(ErrorNode *a, ErrorNode *b) {
    int tempLine = a->lineNum;/*save temporary line number*/
    ErrorType tempType = a->type;/*save temporary type*/
    char *tempInfo = a->extraInfo;/*save temporary extra info*/

    /*swap values between a and b*/
    a->lineNum = b->lineNum;
    a->type = b->type;
    a->extraInfo = b->extraInfo;

    b->lineNum = tempLine;
    b->type = tempType;
    b->extraInfo = tempInfo;
}

/*
 * printError func
 * prints a single error message to standard output based on its type and details.
 * the input is the filename, line number, error type, and optional extra information.
 * returns void.
 */
void printError(const char *filename, int lineNum, ErrorType type, const char *extraInfo) {
    if (filename) {/*if filename is provided*/
        fprintf(stdout, "[%s", filename);
        if (lineNum > 0) { fprintf(stdout, ":%d", lineNum);/*print line number if valid*/ }
        fprintf(stdout, "] ");
    }

    fprintf(stdout, "Error: ");

    /*print corresponding error message*/
    if (type == ERR_OPEN_FILE) {
        fprintf(stdout, "Could not open file");
    } else if (type == ERR_ALLOC_FAIL) {
        fprintf(stdout, "Not enough memory");
    } else if (type == ERR_LINE_TOO_LONG) {
        fprintf(stdout, "Line is too long (max %d chars)", MAX_AS_LINE_LEN);
    } else if (type == ERR_LABEL_TOO_LONG) {
        fprintf(stdout, "Label is too long (max %d chars)", MAX_LABEL_LEN);
    } else if (type == ERR_INVALID_LABEL_FORMAT) {
        fprintf(stdout, "Invalid label or macro name format");
    } else if (type == ERR_UNDEFINED_MACRO) {
        fprintf(stdout, "Undefined macro");
    } else if (type == ERR_MACRO_REDEFINITION) {
        fprintf(stdout, "Macro redefinition");
    } else if (type == ERR_SYMBOL_REDEFINITION) {
        fprintf(stdout, "Symbol redefinition");
    } else if (type == ERR_RESERVED_KEYWORD) {
        fprintf(stdout, "Reserved keyword used as a label or macro name");
    } else if (type == ERR_UNDEFINED_SYMBOL) {
        fprintf(stdout, "Undefined label '%s'", extraInfo);
    } else if (type == ERR_UNKNOWN_COMMAND) {
        fprintf(stdout, "Unknown command/directive");
    } else if (type == ERR_INVALID_REG) {
        fprintf(stdout, "Invalid register name");
    } else if (type == ERR_INVALID_IMMED) {
        fprintf(stdout, "Invalid immediate value");
    } else if (type == ERR_MISSING_COMMA) {
        fprintf(stdout, "Missing comma between parameters");
    } else if (type == ERR_ILLEGAL_COMMA) {
        fprintf(stdout, "Illegal comma");
    } else if (type == ERR_MULTIPLE_COMMAS) {
        fprintf(stdout, "Multiple consecutive commas");
    } else if (type == ERR_EXTRA_TEXT) {
        fprintf(stdout, "extra text after command");
    } else if (type == ERR_ENTRY_NOT_FOUND) {
        fprintf(stdout, "Entry symbol was not defined");
    } else if (type == ERR_BRANCH_TOO_FAR) {
        fprintf(stdout, "Branch offset is out of range or illegal");
    } else {
        fprintf(stdout, "Unknown error occurred");
    }

    if (type != ERR_UNDEFINED_SYMBOL && extraInfo) {/*append extra info if applicable*/
        fprintf(stdout, ": %s", extraInfo);
    }

    fprintf(stdout, "\n");
}

/*
 * printErrors func
 * sorts and prints all collected errors in the error list by line number.
 * the input is the filename and the head of the error list.
 * returns void.
 */
void printErrors(const char *filename, ErrorNode *head) {
    ErrorNode *i, *j;
    /*sort the errors by line number using bubble sort*/
    for (i = head; i != NULL; i = i->next) {
        for (j = i->next; j != NULL; j = j->next) {
            if (i->lineNum > j->lineNum) { swapErrors(i, j);/*swap if out of order*/ }
        }
    }

    while (head) {/*iterate through sorted list*/
        printError(filename, head->lineNum, head->type, head->extraInfo);/*print each error*/
        head = head->next;/*move to next node*/
    }
}
