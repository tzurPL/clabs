/*
 * table.c
 * mmn14
 * Tzur Pinto Lazar
 */

#include "table.h"
#include "util.h"
#include <string.h>

/*
 * addSymbol func
 * adds a symbol to the symbol table or updates it if it exists.
 * the input is a pointer to the head of the symbol table, the symbol name, its address, and its type.
 * returns void.
 */
void addSymbol(SymbolNode **head, const char *name, int address, Type type) {
    SymbolNode *node = getSymbol(*head, name);
    if (!node) {/*if symbol doesn't exist create it*/
        node = (SymbolNode *)safeMalloc(sizeof(SymbolNode));
        node->name = strdupp(name);
        node->address = address;
        node->isCode = node->isData = node->isExternal = node->isEntry = FALSE;
        node->next = *head;
        *head = node;/*add to head of list*/
    }

    /*update the type of the symbol*/
    if (type == CODE) {
        node->isCode = TRUE;
    } else if (type == DATA) {
        node->isData = TRUE;
    } else if (type == EXTERNAL) {
        node->isExternal = TRUE;
        node->address = 0;
    } else if (type == ENTRY) {
        node->isEntry = TRUE;
    }

    if (type != ENTRY && type != EXTERNAL) { node->address = address;/*update address if not entry/external*/ }
}

/*
 * getSymbol func
 * finds a symbol in the list by its name.
 * the input is the head of the symbol table and the name of the symbol to find.
 * returns a pointer to the SymbolNode if found or NULL if not.
 */
SymbolNode *getSymbol(SymbolNode *head, const char *name) {
    while (head) {/*go through the list*/
        if (strcmp(head->name, name) == 0) { return head;/*return symbol if found*/ }
        head = head->next;/*move to next symbol*/
    }
    return NULL;/*return null if not found*/
}

/*
 * freeSymbols func
 * frees the memory allocated for the symbol table.
 * the input is the head of the symbol table.
 * returns void.
 */
void freeSymbols(SymbolNode *head) {
    while (head) {/*go through the list*/
        SymbolNode *temp = head;
        head = head->next;/*move to next node*/
        free(temp->name);/*free the name string*/
        free(temp);/*free the node*/
    }
}

/*
 * addMacro func
 * adds a macro name and its content to the macro list.
 * the input is a pointer to the head of the macro list, the macro name, and its content.
 * returns void.
 */
void addMacro(MacroNode **head, const char *name, const char *content) {
    MacroNode *newNode = (MacroNode *)safeMalloc(sizeof(MacroNode));/*allocate memory for new node*/
    newNode->name = strdupp(name);
    newNode->content = strdupp(content);
    newNode->next = *head;
    *head = newNode;/*add to head of list*/
}

/*
 * getMacroContent func
 * searches for a macro and returns its things.
 * the input is the head of the macro list and the macro name needed to find.
 * returns the macro content string if found, or null if not.
 */
char *getMacroContent(MacroNode *head, const char *name) {
    while (head) {/*go through the list*/
        if (strcmp(head->name, name) == 0) { return head->content;/*return content if found*/ }
        head = head->next;/*move to next node*/
    }
    return NULL;/*return null if not found*/
}

/*
 * freeMacros func
 * frees the memory allocated for all the macros in the list.
 * the input is the head of the macro list.
 * returns void
 */
void freeMacros(MacroNode *head) {
    while (head) {/*go through the list*/
        MacroNode *temp = head;
        head = head->next;/*move to next node*/
        free(temp->name);/*free the name string*/
        free(temp->content);/*free the content string*/
        free(temp);/*free the node*/
    }
}
