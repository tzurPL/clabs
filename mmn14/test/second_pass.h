/*
 * second_pass.h
 * mmn14
 * Tzur Pinto Lazar
 */

#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include "errors.h"
#include "first_pass.h"
#include "globals.h"
#include "table.h"

typedef struct ExtUsage {
    char *name;
    int address;
    struct ExtUsage *next;
} ExtUsage;

/*
 * secondPass func
 * executes the second pass of the assembler, resolving label addresses in code nodes and marking entry symbols.
 * the input is the filename, symbol table, code list, a pointer to store external usages, and the error list.
 * returns boolean indicating whether the pass succeeded without errors.
 */
boolean secondPass(const char *filename, SymbolNode *symbols, CodeNode *codeHead, ExtUsage **extUsage,
                   ErrorNode **errorList);

/*
 * freeExtUsage func
 * frees the memory allocated for the external usage list.
 * the input is the head of the external usage list.
 * returns void.
 */
void freeExtUsage(ExtUsage *head);

#endif
