#include "second_pass.h"
#include "util.h"
#include "errors.h"
#include <string.h>

void addExtUsage(ExtUsage **head, const char *name, int address) {
    ExtUsage *newNode = (ExtUsage *)safeMalloc(sizeof(ExtUsage));
    newNode->name = strdupp(name);
    newNode->address = address;
    newNode->next = NULL;
    if (!*head) *head = newNode;
    else {
        ExtUsage *curr = *head;
        while (curr->next) curr = curr->next;
        curr->next = newNode;
    }
}

void freeExtUsage(ExtUsage *head) {
    while (head) {
        ExtUsage *temp = head;
        head = head->next;
        free(temp->name);
        free(temp);
    }
}

/*
 * processes the .entry directive.
 * finds the symbol and marks it as an entry, reports error if not found.
 * the input is string pointer, symbol table, error list, line number, and error flag.
 * returns void.
 */
void procEntry(char **ptr, SymbolNode *symbols, ErrorNode **errorList, int lineNum, boolean *error) {
    char *ent = getToken(ptr);
    if (ent) {
        SymbolNode *sym = getSymbol(symbols, ent);
        if (sym) sym->isEntry = TRUE;
        else {
            addError(errorList, lineNum, ERR_ENTRY_NOT_FOUND, ent);
            *error = TRUE;
        }
        free(ent);
    }
}

/*
 * processes a code node to resolve its label dependency.
 * calculates the immed or address and updates the word.
 * the input is the code node, symbol table, external usage list, error list, and error flag.
 * returns void.
 */
void procCodeNode(CodeNode *curr, SymbolNode *symbols, ExtUsage **extUsage, ErrorNode **errorList, boolean *error) {
    SymbolNode *sym = getSymbol(symbols, curr->labelDep);
    unsigned int opcode;
    int immed;

    if (sym) {
        opcode = (curr->word >> 26) & 0x3F;
        if (opcode >= 15 && opcode <= 18) {
            if (sym->isExternal) {
                addError(errorList, curr->lineNum, ERR_BRANCH_TOO_FAR, "cannot branch to external label");
                *error = TRUE;
            } else {
                immed = sym->value - curr->address;
                if (immed > 32767 || immed < -32768) {
                    addError(errorList, curr->lineNum, ERR_BRANCH_TOO_FAR, curr->labelDep);
                    *error = TRUE;
                }
                curr->word = (curr->word & 0xFFFF0000) | (immed & 0xFFFF);
            }
        } else if (opcode >= 30 && opcode <= 32) {
            if (sym->isExternal) {
                curr->word = (curr->word & 0xFE000000);
                addExtUsage(extUsage, sym->name, curr->address);
            } else {
                curr->word = (curr->word & 0xFE000000) | (sym->value & 0x1FFFFFF);
            }
        }
    } else {
        addError(errorList, curr->lineNum, ERR_UNDEFINED_SYMBOL, curr->labelDep);
        *error = TRUE;
    }
}

boolean secondPass(const char *filename, SymbolNode *symbols, CodeNode *codeHead, ExtUsage **extUsage, ErrorNode **errorList) {
    char amName[MAX_LINE_LENGTH];
    FILE *fp;
    char line[MAX_LINE_LENGTH + 2];
    boolean error = FALSE;
    char *ptr, *token;
    CodeNode *curr;
    int lineNum = 0;

    strcpy(amName, filename); strcat(amName, ".am");
    fp = fopen(amName, "r");
    if (!fp) return FALSE;

    while (fgets(line, sizeof(line), fp)) {
        ptr = line; lineNum++;
        if (!isEmptyLine(line) && !isCommentLine(line)) {
            token = getToken(&ptr);
            if (token && token[0] != '\0' && token[strlen(token)-1] == ':') { free(token); token = getToken(&ptr); }
            if (token && strcmp(token, ".entry") == 0) {
                procEntry(&ptr, symbols, errorList, lineNum, &error);
            }
            if (token) free(token);
        }
    }
    fclose(fp);

    curr = codeHead;
    while (curr) {
        if (curr->labelDep) {
            procCodeNode(curr, symbols, extUsage, errorList, &error);
        }
        curr = curr->next;
    }
    return !error;
}
