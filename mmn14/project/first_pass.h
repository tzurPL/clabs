#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "errors.h"
#include "globals.h"
#include "table.h"

/*Code node list */
typedef struct CodeNode {
    Instruction inst;
    int address;
    int lineNum;
    char *labelRef;
    struct CodeNode *next;
} CodeNode;

/*Data nodes list */
typedef struct DataNode {
    unsigned char byte;
    int address;
    struct DataNode *next;
} DataNode;

/*
 * firstPass func
 * first pass of the assembler
 * the input is filename, symbols list, code list, data list, IC, DC, error list, macros
 * returns true if the pass was ssuccessful and false if not
 */
boolean firstPass(const char *filename, SymbolNode **symbols, CodeNode **codeHead, DataNode **dataHead, int *IC,
                  int *DC, ErrorNode **errorList, MacroNode *macros);

#endif
