/*
 * table.h
 * mmn14
 * Tzur Pinto Lazar
*/

#ifndef TABLE_H
#define TABLE_H

#include "globals.h"

/* Symbol Table */
typedef enum { CODE, DATA, EXTERNAL, ENTRY } Attribute;

typedef struct SymbolNode {
    char *name;
    int value;
    boolean isCode;
    boolean isData;
    boolean isExternal;
    boolean isEntry;
    struct SymbolNode *next;
} SymbolNode;

/*
 * addSymbol func
 * adds a symbol to the symbol table or updates its attributes if it exists.
 * the input is a pointer to the head of the symbol table, the symbol name, its value, and its attribute type.
 * returns void.
 */
void addSymbol(SymbolNode **head, const char *name, int value, Attribute attr);

/*
 * getSymbol func
 * finds a symbol in the list by its name.
 * the input is the head of the symbol table and the name of the symbol to find.
 * returns a pointer to the SymbolNode if found, or NULL otherwise.
 */
SymbolNode *getSymbol(SymbolNode *head, const char *name);

/*
 * freeSymbols func
 * frees the memory allocated for the symbol table list.
 * the input is the head of the symbol table.
 * returns void.
 */
void freeSymbols(SymbolNode *head);

/* Macro Table */
typedef struct MacroNode {
    char *name;
    char *content;
    struct MacroNode *next;
} MacroNode;

/*
 * addMacro func
 * adds a macro name and its content to the macro list.
 * the input is a pointer to the head of the macro list, the macro name, and its content.
 * returns void.
 */
void addMacro(MacroNode **head, const char *name, const char *content);

/*
 * getMacroContent func
 * searches for a macro and returns its content.
 * the input is the head of the macro list and the macro name to search for.
 * returns the macro content string if found, or NULL otherwise.
 */
char *getMacroContent(MacroNode *head, const char *name);

/*
 * freeMacros func
 * frees the memory allocated for all the macros in the list.
 * the input is the head of the macro list.
 * returns void.
 */
void freeMacros(MacroNode *head);

#endif
