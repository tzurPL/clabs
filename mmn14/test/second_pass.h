#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include "globals.h"
#include "table.h"
#include "first_pass.h"
#include "errors.h"

typedef struct ExtUsage {
    char *name;
    int address;
    struct ExtUsage *next;
} ExtUsage;

boolean secondPass(const char *filename, SymbolNode *symbols, CodeNode *codeHead, ExtUsage **extUsage, ErrorNode **errorList);
void freeExtUsage(ExtUsage *head);

#endif
