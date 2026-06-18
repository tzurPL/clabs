#include "table.h"
#include "util.h"
#include <string.h>

/* addSymbol func: adds a symbol to the symbol table or updates its attributes if it exists. */
void addSymbol(SymbolNode **head, const char *name, int value, Attribute attr) {
    SymbolNode *node = getSymbol(*head, name);
    if (!node) {
        node = (SymbolNode *)safeMalloc(sizeof(SymbolNode));
        node->name = duplicateString(name);
        node->value = value;
        node->isCode = node->isData = node->isExternal = node->isEntry = FALSE;
        node->next = *head;
        *head = node;
    }

    if (attr == ATTR_CODE) node->isCode = TRUE;
    else if (attr == ATTR_DATA) node->isData = TRUE;
    else if (attr == ATTR_EXTERNAL) { node->isExternal = TRUE; node->value = 0; }
    else if (attr == ATTR_ENTRY) node->isEntry = TRUE;

    if (attr != ATTR_ENTRY && attr != ATTR_EXTERNAL) node->value = value;
}

/* getSymbol func: finds a symbol in the list by its name. */
SymbolNode *getSymbol(SymbolNode *head, const char *name) {
    while (head) {
        if (strcmp(head->name, name) == 0) return head;
        head = head->next;
    }
    return NULL;
}

/* freeSymbols func: frees the memory of the symbol table list. */
void freeSymbols(SymbolNode *head) {
    while (head) {
        SymbolNode *temp = head;
        head = head->next;
        free(temp->name);
        free(temp);
    }
}

/* addMacro func: adds a macro name and its content to the macro list. */
void addMacro(MacroNode **head, const char *name, const char *content) {
    MacroNode *newNode = (MacroNode *)safeMalloc(sizeof(MacroNode));
    newNode->name = duplicateString(name);
    newNode->content = duplicateString(content);
    newNode->next = *head;
    *head = newNode;
}

/* getMacroContent func: searches for a macro and returns its content. */
char *getMacroContent(MacroNode *head, const char *name) {
    while (head) {
        if (strcmp(head->name, name) == 0) return head->content;
        head = head->next;
    }
    return NULL;
}

/* freeMacros func: frees all the macros in the list. */
void freeMacros(MacroNode *head) {
    while (head) {
        MacroNode *temp = head;
        head = head->next;
        free(temp->name);
        free(temp->content);
        free(temp);
    }
}
