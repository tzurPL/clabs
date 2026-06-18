#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include "globals.h"
#include "table.h"
#include "first_pass.h"
#include "errors.h"

typedef struct ExternalUsage {
    char *name;
    int address;
    struct ExternalUsage *next;
} ExternalUsage;

boolean secondPass(const char *filename, SymbolNode *symbols, CodeNode *codeHead, ExternalUsage **extUsage, ErrorNode **errorList);
void freeExtUsage(ExternalUsage *head);

#endif
