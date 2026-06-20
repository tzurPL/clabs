#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "globals.h"
#include "table.h"
#include "errors.h"

typedef struct CodeNode {
    unsigned int word;
    int address;
    int lineNum;
    char *labelDep;
    struct CodeNode *next;
} CodeNode;

typedef struct DataNode {
    unsigned char byte;
    int address;
    struct DataNode *next;
} DataNode;

boolean firstPass(const char *filename, SymbolNode **symbols, CodeNode **codeHead, DataNode **dataHead, int *IC, int *DC, ErrorNode **errorList, MacroNode *macros);

#endif
