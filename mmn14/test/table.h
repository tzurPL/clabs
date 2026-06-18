#ifndef TABLE_H
#define TABLE_H

#include "globals.h"

/* Symbol Table */
typedef enum { ATTR_CODE, ATTR_DATA, ATTR_EXTERNAL, ATTR_ENTRY } Attribute;

typedef struct SymbolNode {
    char *name;
    int value;
    boolean isCode;
    boolean isData;
    boolean isExternal;
    boolean isEntry;
    struct SymbolNode *next;
} SymbolNode;

void addSymbol(SymbolNode **head, const char *name, int value, Attribute attr);
SymbolNode *getSymbol(SymbolNode *head, const char *name);
void freeSymbols(SymbolNode *head);

/* Macro Table */
typedef struct MacroNode {
    char *name;
    char *content;
    struct MacroNode *next;
} MacroNode;

void addMacro(MacroNode **head, const char *name, const char *content);
char *getMacroContent(MacroNode *head, const char *name);
void freeMacros(MacroNode *head);

#endif
