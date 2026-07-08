/*
 * second_pass.c
 * mmn14
 * Tzur Pinto Lazar
 */

#include "second_pass.h"
#include "errors.h"
#include "util.h"
#include <string.h>

/*
 * addExtUsage func
 * adds a new external usage node to the external usage list.
 * the input is a pointer to the head of the external usage list, the external symbol name, and the address it was used
 * at. returns void.
 */
void addExtUsage(ExtUsage **head, const char *name, int address) {
    ExtUsage *newNode = (ExtUsage *)safeMalloc(sizeof(ExtUsage));/*allocate memory for new node*/
    newNode->name = strdupp(name);/*duplicate name*/
    newNode->address = address;/*set the address*/
    newNode->next = NULL;
    if (!*head) {
        *head = newNode;/*if list is empty, set as head*/
    } else {
        ExtUsage *curr = *head;
        while (curr->next) {
            curr = curr->next;/*find the end of the list*/
        }
        curr->next = newNode;/*append the new node*/
    }
}

/*
 * freeExtUsage func
 * frees the memory allocated for the external usage list.
 * the input is the head of the external usage list.
 * returns void.
 */
void freeExtUsage(ExtUsage *head) {
    while (head) {/*iterate through the list*/
        ExtUsage *temp = head;
        head = head->next;/*move to next node*/
        free(temp->name);/*free the name string*/
        free(temp);/*free the node itself*/
    }
}

/*
 * procEntry func
 * processes the .entry directive. finds the symbol and marks it as an entry, reports error if not found.
 * the input is string pointer, symbol table, error list, line number, and error flag.
 * returns void.
 */
void procEntry(char **ptr, SymbolNode *symbols, ErrorNode **errorList, int lineNum, boolean *error) {
    char *ent = getToken(ptr);/*get the entry symbol name*/
    if (ent) {/*if an entry symbol was provided*/
        SymbolNode *sym = getSymbol(symbols, ent);/*look up symbol in table*/
        if (sym) {
            sym->isEntry = TRUE;/*mark as entry*/
        } else {
            addError(errorList, lineNum, ERR_ENTRY_NOT_FOUND, ent);/*report error if not found*/
            *error = TRUE;/*set error flag*/
        }
        free(ent);/*free the token*/
    }
}

/*
 * procCodeNode func
 * processes a code node to resolve its label dependency. calculates the immed or address and updates the word.
 * the input is the code node, symbol table, external usage list, error list, and error flag.
 * returns void.
 */
void procCodeNode(CodeNode *curr, SymbolNode *symbols, ExtUsage **extUsage, ErrorNode **errorList, boolean *error) {
    SymbolNode *sym = getSymbol(symbols, curr->labelDep);/*look up the dependent symbol*/
    unsigned int opcode;
    int immed;

    if (sym) {/*if symbol was found*/
        opcode = curr->inst.i.opcode;/*extract opcode*/
        if (opcode >= MIN_BRANCH_OPCODE && opcode <= MAX_BRANCH_OPCODE) {/*if it is a conditional branch*/
            if (sym->isExternal) {/*if branching to an external label*/
                addError(errorList, curr->lineNum, ERR_BRANCH_TOO_FAR,
                         "cannot branch to external label");/*report error*/
                *error = TRUE;/*set error flag*/
            } else {/*local label*/
                immed = sym->address - curr->address;/*calculate offset*/
                if (immed > MAX_IMMED || immed < MIN_IMMED) {/*check offset bounds*/
                    addError(errorList, curr->lineNum, ERR_BRANCH_TOO_FAR,
                             curr->labelDep);/*report error if out of bounds*/
                    *error = TRUE;/*set error flag*/
                }
                curr->inst.i.immed = immed;/*update instruction word*/
            }
        } else if (opcode >= MIN_JMP_OPCODE && opcode <= MAX_JMP_OPCODE) {/*if it is a jump instruction*/
            if (sym->isExternal) {/*if jumping to an external label*/
                curr->inst.j.address = 0;/*clear address bits*/
                addExtUsage(extUsage, sym->name, curr->address);/*record external usage*/
            } else {/*local label*/
                curr->inst.j.address = sym->address;/*encode direct address*/
            }
        }
    } else {/*symbol not found*/
        addError(errorList, curr->lineNum, ERR_UNDEFINED_SYMBOL, curr->labelDep);/*report error*/
        *error = TRUE;/*set error flag*/
    }
}

/*
 * secondPass func
 * executes the second pass of the assembler, resolving label addresses in code nodes and marking entry symbols.
 * the input is the filename, symbol table, code list, a pointer to store external usages, and the error list.
 * returns boolean indicating whether the pass succeeded without errors.
 */
boolean secondPass(const char *filename, SymbolNode *symbols, CodeNode *codeHead, ExtUsage **extUsage,
                   ErrorNode **errorList) {
    char amName[MAX_LINE_LENGTH];
    FILE *fp;
    char line[MAX_LINE_LENGTH + EXTRA_CHARS];
    boolean error = FALSE;
    char *ptr, *token;
    CodeNode *curr;
    int lineNum = 0;

    strcpy(amName, filename);
    stripAsExtension(amName);/*strip .as extension if present*/
    strcat(amName, ".am");/*append .am extension*/
    fp = fopen(amName, "r");/*open the file for reading*/
    if (!fp) { return FALSE; }

    while (fgets(line, sizeof(line), fp)) {/*read line by line*/
        ptr = line;
        lineNum++;
        if (!isEmptyLine(line) && !isCommentLine(line)) {/*ignore empty and comment lines*/
            token = getToken(&ptr);/*get first token*/
            if (token && token[0] != '\0' && token[strlen(token) - 1] == ':') {
                free(token);
                token = getToken(&ptr);
            }/*skip label if present*/
            if (token && strcmp(token, ".entry") == 0) {/*process entry directive*/
                procEntry(&ptr, symbols, errorList, lineNum, &error);
            }
            if (token) { free(token); }
        }
    }
    fclose(fp);/*close the file*/

    curr = codeHead;
    while (curr) {/*iterate through code nodes to resolve dependencies*/
        if (curr->labelDep) { procCodeNode(curr, symbols, extUsage, errorList, &error);/*resolve dependency*/ }
        curr = curr->next;
    }
    return !error;
}
