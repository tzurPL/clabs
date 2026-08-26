/*
 * the second pass of the assembler
 * rereads the .am file to process .entry directives and mark entry symbols
 * then goes over the code node list to resolve label references
 * calculating branch offsets for conditional branches and finding addresses for jump instructions
 * saves external symbol usage for the .ext
 */
#include "second_pass.h"
#include "errors.h"
#include "util.h"
#include <string.h>

/*
 * addExtNode func
 * adds a new external node to the extern list
 * the input is a pointer to the head of the extern list, the external symbol name, and it's address
 * returns void.
 */
void addExtNode(ExtNode **head, const char *name, int address) {
    ExtNode *newNode = (ExtNode *)safeMalloc(sizeof(ExtNode));/*alloc memory for new node*/
    newNode->name = strdupp(name);/*dup name*/
    newNode->address = address;/*set address*/
    newNode->next = NULL;
    if (!*head) {
        *head = newNode;/*if list is empty set as head*/
    } else {
        ExtNode *curr = *head;
        while (curr->next) {
            curr = curr->next;/*find end of list*/
        }
        curr->next = newNode;/*add new node*/
    }
}

/*
 * freeExtNode func
 * frees memory allocated for the extern list
 * the input is the head of the extern list
 * returns void
 */
void freeExtNode(ExtNode *head) {
    while (head) {/*go through list*/
        ExtNode *temp = head;
        head = head->next;/*move to next node*/
        free(temp->name);/*free name*/
        free(temp);/*free node*/
    }
}

/*
 * procEntry func
 * processes .entry, finds the symbol and marks it as an entry, save error if not found
 * the input is string pointer, symbol table, error list, line num,error flag
 * returns void
 */
void procEntry(char **ptr, SymbolNode *symbols, ErrorNode **errorList, int lineNum, boolean *error) {
    char *ent = getToken(ptr);/*get entry symbol name*/
    if (ent) {/*if entry symbol found*/
        SymbolNode *sym = getSymbol(symbols, ent);/*search symbol in table*/
        if (sym) {
            sym->isEntry = TRUE;/*mark as entry*/
        } else {
            addError(errorList, lineNum, ERR_ENTRY_NOT_FOUND, ent);/*save error if not found*/
            *error = TRUE;/*set error flag*/
        }
        free(ent);/*free token*/
    }
}

/*
 * procCodeNode func
 * processes a code node to try and find the label reference missing info
 * the input is the code node, symbol table, extern list, error list, error flag
 * returns void
 */
void procCodeNode(CodeNode *curr, SymbolNode *symbols, ExtNode **extNode, ErrorNode **errorList, boolean *error) {
    SymbolNode *sym = getSymbol(symbols, curr->labelRef);/*look up symbol*/
    unsigned int opcode;
    int immed;

    if (sym) {/*if symbol found*/
        opcode = curr->inst.i.opcode;/*get opcode*/
        if (opcode >= MIN_BRANCH_OPCODE && opcode <= MAX_BRANCH_OPCODE) {/*if is a conditional branch*/
            if (sym->isExternal) {/*if branching to an extern label*/
                addError(errorList, curr->lineNum, ERR_BRANCH_TOO_FAR,
                         "cannot branch to external label");/*save error*/
                *error = TRUE;/*set error flag*/
            } else {/*local label*/
                immed = sym->address - curr->address;/*calc immed val*/
                if (immed > MAX_IMMED || immed < MIN_IMMED) {/*check if it is in bounds*/
                    addError(errorList, curr->lineNum, ERR_BRANCH_TOO_FAR,
                             curr->labelRef);/*save error if out of bounds*/
                    *error = TRUE;/*set error flag*/
                }
                curr->inst.i.immed = immed;/*update instruction*/
            }
        } else if (opcode >= MIN_JMP_OPCODE && opcode <= MAX_JMP_OPCODE) {/*if it is a jump intruction*/
            if (sym->isExternal) {/*if jumping to an extern label*/
                curr->inst.j.address = 0;/*clear address*/
                addExtNode(extNode, sym->name, curr->address);/*save to external node*/
            } else {/*local label*/
                curr->inst.j.address = sym->address;/*save address*/
            }
        }
    } else {/*symbol not found*/
        addError(errorList, curr->lineNum, ERR_UNDEFINED_SYMBOL, curr->labelRef);/*save error*/
        *error = TRUE;/*set error flag*/
    }
}

/*
 * freeExtUsage func
 * frees memory allocated for the extern list
 * the input is the head of the extern list
 * returns void
 */
void freeExtUsage(ExtNode *head) {
    while (head) {
        ExtNode *temp = head;
        head = head->next;
        if (temp->name) { free(temp->name); }
        free(temp);
    }
}

/*
 * secondPass func
 * the second pass of the assembler
 * the input is the filename, symbol table, code list, a pointer to store external nodes, error list
 * returns true if success and false if not
 */
boolean secondPass(const char *filename, SymbolNode *symbols, CodeNode *codeHead, ExtNode **extNode,
                   ErrorNode **errorList) {
    char amName[MAX_LINE_LENGTH];
    FILE *fp;
    char line[MAX_LINE_LENGTH + 2];/* +2 for the \n and \0 and other \ ends */
    boolean error = FALSE;
    char *ptr, *token;
    CodeNode *curr;
    int lineNum = 0;

    strcpy(amName, filename);
    remAsExtension(amName);/*remove .as*/
    strcat(amName, ".am");/*add .am*/
    fp = fopen(amName, "r");/*open file for reading*/
    if (!fp) { return FALSE; }

    while (fgets(line, sizeof(line), fp)) {/*read line by line*/
        ptr = line;
        lineNum++;
        if (!isEmptyLine(line) && !isCommentLine(line)) {/*skip empty and comments*/
            token = getToken(&ptr);/*get token*/
            if (token && token[0] != '\0' && token[strlen(token) - 1] == ':') {
                free(token);
                token = getToken(&ptr);
            }/*skip label if exsists*/

            if (token && strcmp(token, ".entry") == 0) {/*proc entry*/
                procEntry(&ptr, symbols, errorList, lineNum, &error);
            }
            if (token) { free(token); }
        }
    }
    fclose(fp);/*close file*/

    curr = codeHead;
    while (curr) {/*go through code nodes to fill mising info*/
        if (curr->labelRef) {
            procCodeNode(curr, symbols, extNode, errorList, &error);/*find missing info for label refrences*/
        }
        curr = curr->next;
    }
    return !error;
}
