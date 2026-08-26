/*
 * the second pass of the assembler
 * rereads the .am file to process .entry directives and mark entry symbols
 * then goes over the code node list to resolve label references
 * calculating branch offsets for conditional branches and finding addresses for jump instructions
 * saves external symbol usage for the .ext
 */
#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include "errors.h"
#include "first_pass.h"
#include "globals.h"
#include "table.h"

/*extrn list */
typedef struct ExtNode {
    char *name;
    int address;
    struct ExtNode *next;
} ExtNode;

/*
 * freeExtNode func
 * frees memory allocated for the extern list
 * the input is the head of the extern list
 * returns void
 */
void freeExtUsage(ExtNode *head);

/*
 * secondPass func
 * the second pass of the assembler
 * the input is the filename, symbol table, code list, a pointer to store external nodes, error list
 * returns true if success and false if not
 */
boolean secondPass(const char *filename, SymbolNode *symbols, CodeNode *codeHead, ExtNode **extUsage,ErrorNode **errorList);

#endif
