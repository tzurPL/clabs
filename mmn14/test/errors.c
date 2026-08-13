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
 * adds new error node to end of error list
 * the input is a pointer to the head of the error list, line num, error type, extra info
 * returns void
 */
void addError(ErrorNode **head, int lineNum, ErrorType type, const char *extraInfo) {
    ErrorNode *newNode = (ErrorNode *)safeMalloc(sizeof(ErrorNode));/*alloc memory for new error*/
    newNode->lineNum = lineNum;/*set line num*/
    newNode->type = type;/*set type*/
    newNode->extraInfo = extraInfo ? strdupp(extraInfo) : NULL;/*dup extra info*/
    newNode->next = NULL;

    if (!*head) {
        *head = newNode;/*if list empty set as head*/
    } else {
        ErrorNode *curr = *head;
        while (curr->next) {
            curr = curr->next;/*find end of list*/
        }
        curr->next = newNode;/*add the new node*/
    }
}

/*
 * swapErrors func
 * swaps contents of two error nodes to sort the errors
 * the input is two error nodes
 * returns void
 */
void swapErrors(ErrorNode *a, ErrorNode *b) {
    int tempLine = a->lineNum;/*save temp line num*/
    ErrorType tempType = a->type;/*save temp type*/
    char *tempInfo = a->extraInfo;/*save temp extra info*/

    /*swap vals*/
    a->lineNum = b->lineNum;
    a->type = b->type;
    a->extraInfo = b->extraInfo;

    b->lineNum = tempLine;
    b->type = tempType;
    b->extraInfo = tempInfo;
}

/*
 * printErrors func
 * sorts and prints all errors in the error list by line num
 * the input is the filename and the head of error list
 * returns void
 */
void printErrors(const char *filename, ErrorNode *head) {
    ErrorNode *i, *j;
    /*sort errors by line num with bubble sort*/
    for (i = head; i != NULL; i = i->next) {
        for (j = i->next; j != NULL; j = j->next) {
            if (i->lineNum > j->lineNum) { swapErrors(i, j);/*swap if out of order*/ }
        }
    }

    while (head) {/*go through new list*/
        printError(filename, head->lineNum, head->type, head->extraInfo);/*print error*/
        head = head->next;/*move to next node*/
    }
}

/*
 * freeErrors func
 * frees memory for all nodes in error list
 * the input is the head of the error list
 * returns void
 */
void freeErrors(ErrorNode *head) {
    while (head) {/*go through list*/
        ErrorNode *temp = head;
        head = head->next;/*move to next node*/
        if (temp->extraInfo) { free(temp->extraInfo);/*free extra info*/ }
        free(temp);/*free node*/
    }
}


/*
 * printError func
 * prints error message to stdout according to the error peramenters
 * the input is the filename, line num, error type, extra info
 * returns void
 */
void printError(const char *filename, int lineNum, ErrorType type, const char *extraInfo) {
    if (filename) {/*if filename found*/
        fprintf(stdout, "[%s", filename);
        if (lineNum > 0) { fprintf(stdout, ":%d", lineNum);/*print line num if good*/ }
        fprintf(stdout, "] ");
    }

    fprintf(stdout, "Error: ");

    /*print error message*/
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
        fprintf(stdout, "Reserved keyword used as label or macro name");
    } else if (type == ERR_UNDEFINED_SYMBOL) {
        fprintf(stdout, "Undefined label '%s'", extraInfo);
    } else if (type == ERR_UNKNOWN_COMMAND) {
        fprintf(stdout, "Unknown command/directive");
    } else if (type == ERR_INVALID_REG) {
        fprintf(stdout, "Invalid register name");
    } else if (type == ERR_INVALID_IMMED) {
        fprintf(stdout, "Invalid immediate val");
    } else if (type == ERR_MISSING_COMMA) {
        fprintf(stdout, "Missing comma between parameters");
    } else if (type == ERR_ILLEGAL_COMMA) {
        fprintf(stdout, "Illegal comma");
    } else if (type == ERR_MULTIPLE_COMMAS) {
        fprintf(stdout, "Multiple commas one after another");
    } else if (type == ERR_EXTRA_TEXT) {
        fprintf(stdout, "extra text");
    } else if (type == ERR_MISSING_OPERAND) {
        fprintf(stdout, "missing operand");
    } else if (type == ERR_MISSING_QUOTE) {
        fprintf(stdout, "Missing end quote for string");
    } else if (type == ERR_MISSING_STRING) {
        fprintf(stdout, "Missing string operand");
    } else if (type == ERR_INVALID_OPERAND_TYPE) {
        fprintf(stdout, "Invalid operand type");
    } else if (type == ERR_ENTRY_NOT_FOUND) {
        fprintf(stdout, "Entry symbol was not found");
    } else if (type == ERR_BRANCH_TOO_FAR) {
        fprintf(stdout, "Branch offset is out of range or illegal");
    } else {
        fprintf(stdout, "Unknown error");
    }

    if (type != ERR_UNDEFINED_SYMBOL && extraInfo) {/*add extra info if needed*/
        fprintf(stdout, ": %s", extraInfo);
    }

    fprintf(stdout, "\n");
}

