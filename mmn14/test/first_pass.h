/*
 * first_pass.h
 * mmn14
 * Tzur Pinto Lazar
 */

#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "errors.h"
#include "globals.h"
#include "table.h"

typedef struct CodeNode {
    Instruction inst;
    int address;
    int lineNum;
    char *labelRef;
    struct CodeNode *next;
} CodeNode;

typedef struct DataNode {
    unsigned char byte;
    int address;
    struct DataNode *next;
} DataNode;

/*
 * firstPass func
 * executes the first pass of the assembler, processing labels, directives, and instructions to build the symbol table
 * and data/code images. the input is filename, symbols list, code list, data list, IC, DC, error list, and macros.
 * returns boolean indicating whether the pass was completely successful.
 */
boolean firstPass(const char *filename, SymbolNode **symbols, CodeNode **codeHead, DataNode **dataHead, int *IC,
                  int *DC, ErrorNode **errorList, MacroNode *macros);

#endif
