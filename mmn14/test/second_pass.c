#include "second_pass.h"
#include "util.h"
#include "errors.h"
#include <string.h>

void addExtUsage(ExternalUsage **head, const char *name, int address) {
    ExternalUsage *newNode = (ExternalUsage *)safeMalloc(sizeof(ExternalUsage));
    newNode->name = duplicateString(name);
    newNode->address = address;
    newNode->next = NULL;
    if (!*head) *head = newNode;
    else {
        ExternalUsage *curr = *head;
        while (curr->next) curr = curr->next;
        curr->next = newNode;
    }
}

void freeExtUsage(ExternalUsage *head) {
    while (head) {
        ExternalUsage *temp = head;
        head = head->next;
        free(temp->name);
        free(temp);
    }
}

boolean secondPass(const char *filename, SymbolNode *symbols, CodeNode *codeHead, ExternalUsage **extUsage, ErrorNode **errorList) {
    char amName[MAX_LINE_LENGTH];
    FILE *fp;
    char line[MAX_LINE_LENGTH + 2];
    boolean error = FALSE;
    char *ptr, *token, *ent;
    CodeNode *curr;
    SymbolNode *sym;
    unsigned int opcode;
    int offset, lineNum = 0;

    strcpy(amName, filename); strcat(amName, ".am");
    fp = fopen(amName, "r");
    if (!fp) return FALSE;

    while (fgets(line, sizeof(line), fp)) {
        ptr = line; lineNum++;
        if (!isEmptyLine(line) && !isCommentLine(line)) {
            token = getToken(&ptr);
            if (token && token[0] != '\0' && token[strlen(token)-1] == ':') { free(token); token = getToken(&ptr); }
            if (token && strcmp(token, ".entry") == 0) {
                ent = getToken(&ptr);
                if (ent) {
                    sym = getSymbol(symbols, ent);
                    if (sym) sym->isEntry = TRUE;
                    else {
                        addError(errorList, lineNum, ERR_ENTRY_NOT_FOUND, ent);
                        error = TRUE;
                    }
                    free(ent);
                }
            }
            if (token) free(token);
        }
    }
    fclose(fp);

    curr = codeHead;
    while (curr) {
        if (curr->labelDep) {
            sym = getSymbol(symbols, curr->labelDep);
            if (sym) {
                opcode = (curr->word >> 26) & 0x3F;
                if (opcode >= 15 && opcode <= 18) {
                    if (sym->isExternal) {
                        addError(errorList, curr->lineNum, ERR_BRANCH_TOO_FAR, "cannot branch to external label");
                        error = TRUE;
                    } else {
                        offset = sym->value - curr->address;
                        if (offset > 32767 || offset < -32768) {
                            addError(errorList, curr->lineNum, ERR_BRANCH_TOO_FAR, curr->labelDep);
                            error = TRUE;
                        }
                        curr->word = (curr->word & 0xFFFF0000) | (offset & 0xFFFF);
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
                error = TRUE;
            }
        }
        curr = curr->next;
    }
    return !error;
}
