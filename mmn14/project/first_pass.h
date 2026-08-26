/*
 * first pass of the assembler
 * reads the .am file line by line and checks syntax and builds the symbol table
 * and translates instructions to code nodes and data directives to data nodes
 * labels that reference other labels are saved as strings for the second pass to complete and find
 * after processing all lines it updates data addresses to come after the code part
 */
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
