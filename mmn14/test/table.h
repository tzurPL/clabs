/*
 * table.h
 * mmn14
 * Tzur Pinto Lazar
 */

#ifndef TABLE_H
#define TABLE_H

#include "globals.h"

/* Symbol Table */
typedef enum { CODE, DATA, EXTERNAL, ENTRY } Type;
typedef struct SymbolNode {/*linked list */
    char *name;
    int address;
    boolean isCode;
    boolean isData;
    boolean isExternal;
    boolean isEntry;
    struct SymbolNode *next;
} SymbolNode;

/*
 * addSymbol func
 * adds a symbol to the symbol table or updates it if it exists.
 * the input is a pointer to the head of the symbol table, the symbol name, its address, and its type.
 * returns void.
 */
void addSymbol(SymbolNode **head, const char *name, int address, Type type);

/*
 * getSymbol func
 * finds a symbol in the list from its name
 * the input is the head of the symbol table and the name of the symbol to find
 * returns a pointer to the SymbolNode if found or null if not.
 */
SymbolNode *getSymbol(SymbolNode *head, const char *name);

/*
 * freeSymbols func
 * frees the memory allocated for the symbol table
 * the input is the head of the symbol table
 * returns void
 */
void freeSymbols(SymbolNode *head);

/* Macro Table linked list*/
typedef struct MacroNode {
    char *name;
    char *content;
    struct MacroNode *next;
} MacroNode;

/*
 * addMacro func
 * adds a macro name and its content to the macro list
 * the input is a pointer to the head of the macro list, the macro name, and its content
 * returns void
 */
void addMacro(MacroNode **head, const char *name, const char *content);

/*
 * getMacroContent func
 * searches for a macro and returns its things
 * the input is the head of the macro list and the macro name needed to find
 * returns the macro content string if found, or null if no
 */
char *getMacroContent(MacroNode *head, const char *name);

/*
 * freeMacros func
 * frees all the macros in the list.
 * the input is the head of the macro list.
 * returns void
 */
void freeMacros(MacroNode *head);

#endif
