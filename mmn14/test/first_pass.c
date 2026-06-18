#include "first_pass.h"
#include "util.h"
#include <string.h>

void addCodeNode(CodeNode **head, unsigned int word, int address, int lineNum, char *labelDep) {
    CodeNode *newNode = (CodeNode *)safeMalloc(sizeof(CodeNode));
    newNode->word = word;
    newNode->address = address;
    newNode->lineNum = lineNum;
    newNode->labelDep = labelDep ? duplicateString(labelDep) : NULL;
    newNode->next = NULL;
    if (!*head) *head = newNode;
    else {
        CodeNode *curr = *head;
        while (curr->next) curr = curr->next;
        curr->next = newNode;
    }
}

void addDataNode(DataNode **head, unsigned char byte, int address) {
    DataNode *newNode = (DataNode *)safeMalloc(sizeof(DataNode));
    newNode->byte = byte;
    newNode->address = address;
    newNode->next = NULL;
    if (!*head) *head = newNode;
    else {
        DataNode *curr = *head;
        while (curr->next) curr = curr->next;
        curr->next = newNode;
    }
}

static boolean parseData(char **ptr, DataNode **dataHead, int *DC, int size, const char *filename, int lineNum, ErrorNode **errorList) {
    char *t;
    long val;
    boolean err = FALSE; /* C90 requires declaration at top */

    skipSpaces(ptr);
    if (**ptr == ',') { addError(errorList, lineNum, ERR_ILLEGAL_COMMA, NULL); return FALSE; }
    while ((t = getToken(ptr))) {
        val = atol(t);
        free(t);
        if (size == 1) addDataNode(dataHead, (unsigned char)val, (*DC)++);
        else if (size == 2) { addDataNode(dataHead, val & 0xFF, (*DC)++); addDataNode(dataHead, (val >> 8) & 0xFF, (*DC)++); }
        else if (size == 4) { addDataNode(dataHead, val & 0xFF, (*DC)++); addDataNode(dataHead, (val >> 8) & 0xFF, (*DC)++); addDataNode(dataHead, (val >> 16) & 0xFF, (*DC)++); addDataNode(dataHead, (val >> 24) & 0xFF, (*DC)++); }

        skipSpaces(ptr);
        if (**ptr == ',') {
            (*ptr)++;
            if (**ptr == ',') { addError(errorList, lineNum, ERR_MULTIPLE_COMMAS, NULL); return FALSE; }
            skipSpaces(ptr);
            if (**ptr == '\0' || **ptr == ';' || **ptr == '\n' || **ptr == '\r') { addError(errorList, lineNum, ERR_ILLEGAL_COMMA, "ends with comma"); return FALSE; }
        } else break;
    }

    checkExtraText(ptr, errorList, lineNum, &err);
    return !err;
}

boolean firstPass(const char *filename, SymbolNode **symbols, CodeNode **codeHead, DataNode **dataHead, int *IC, int *DC, ErrorNode **errorList) {
    char amName[MAX_LINE_LENGTH];
    FILE *fp;
    char line[MAX_LINE_LENGTH + 2];
    int lineNum = 0, len;
    boolean error = FALSE, lineError;
    char *ptr, *token, *label, *ext, *labDep;
    SymbolNode *currSym;
    DataNode *currData;
    Opcode *op;
    unsigned int word;
    int rs, rt, rd;
    short immed;

    strcpy(amName, filename); strcat(amName, ".am");
    fp = fopen(amName, "r");
    if (!fp) { addError(errorList, 0, ERR_OPEN_FILE, amName); return FALSE; }

    while (fgets(line, sizeof(line), fp)) {
        ptr = line; label = NULL; lineNum++; lineError = FALSE;

        len = strlen(line);
        if (len > 0 && line[len-1] == '\n') len--;
        if (len > 0 && line[len-1] == '\r') len--;
        if (len > 80) { addError(errorList, lineNum, ERR_LINE_TOO_LONG, NULL); error = TRUE; continue; }

        if (isEmptyLine(line) || isCommentLine(line)) continue;

        token = getToken(&ptr);
        if (token && token[0] != '\0' && token[strlen(token)-1] == ':') {
            token[strlen(token)-1] = '\0';
            if (isReservedKeyword(token)) { addError(errorList, lineNum, ERR_RESERVED_KEYWORD, NULL); lineError = TRUE; }
            else if (getSymbol(*symbols, token)) { addError(errorList, lineNum, ERR_SYMBOL_REDEFINITION, token); lineError = TRUE; }
            label = token;
            token = getToken(&ptr);
        }
        if (!token) { if (label) free(label); if (lineError) error = TRUE; continue; }

        if (token[0] == '.') {
            if (label && !lineError) addSymbol(symbols, label, *DC, ATTR_DATA);
            if (strcmp(token, ".db") == 0) { if (!parseData(&ptr, dataHead, DC, 1, filename, lineNum, errorList)) lineError = TRUE; }
            else if (strcmp(token, ".dh") == 0) { if (!parseData(&ptr, dataHead, DC, 2, filename, lineNum, errorList)) lineError = TRUE; }
            else if (strcmp(token, ".dw") == 0) { if (!parseData(&ptr, dataHead, DC, 4, filename, lineNum, errorList)) lineError = TRUE; }
            else if (strcmp(token, ".asciz") == 0) {
                skipSpaces(&ptr);
                if (*ptr == '\"') {
                    ptr++;
                    while (*ptr && *ptr != '\"') addDataNode(dataHead, *ptr++, (*DC)++);
                    if (*ptr == '\"') { ptr++; addDataNode(dataHead, '\0', (*DC)++); }
                    else { addError(errorList, lineNum, ERR_EXTRA_TEXT, "missing closing quote"); lineError = TRUE; }
                } else { addError(errorList, lineNum, ERR_EXTRA_TEXT, "missing string"); lineError = TRUE; }
                checkExtraText(&ptr, errorList, lineNum, &lineError);
            } else if (strcmp(token, ".extern") == 0) {
                ext = getToken(&ptr);
                if (ext) {
                    SymbolNode *s = getSymbol(*symbols, ext);
                    if (s && !s->isExternal) { addError(errorList, lineNum, ERR_SYMBOL_REDEFINITION, "external symbol already defined locally"); lineError = TRUE; }
                    else addSymbol(symbols, ext, 0, ATTR_EXTERNAL);
                    free(ext);
                } else { addError(errorList, lineNum, ERR_EXTRA_TEXT, "missing extern name"); lineError = TRUE; }
                checkExtraText(&ptr, errorList, lineNum, &lineError);
            } else if (strcmp(token, ".entry") != 0) { addError(errorList, lineNum, ERR_UNKNOWN_COMMAND, token); lineError = TRUE; }
        } else {
            op = getOpcode(token);
            if (op) {
                word = (op->opcode << 26);
                if (label && !lineError) addSymbol(symbols, label, *IC, ATTR_CODE);

                if (op->type == R_TYPE) {
                    rs=rt=rd=0;
                    rs = parseRegOperand(&ptr, errorList, lineNum);
                    if (rs == -1) lineError = TRUE;

                    if (!lineError && !matchComma(&ptr, errorList, lineNum)) lineError = TRUE;

                    if (op->opcode == 0) { /* arithmetic/logic: rs, rt, rd */
                        if (!lineError) { rt = parseRegOperand(&ptr, errorList, lineNum); if (rt == -1) lineError = TRUE; }
                        if (!lineError && !matchComma(&ptr, errorList, lineNum)) lineError = TRUE;
                        if (!lineError) { rd = parseRegOperand(&ptr, errorList, lineNum); if (rd == -1) lineError = TRUE; }
                    } else { /* copy: rs, rd */
                        if (!lineError) { rd = parseRegOperand(&ptr, errorList, lineNum); if (rd == -1) lineError = TRUE; }
                    }
                    if (!lineError) checkExtraText(&ptr, errorList, lineNum, &lineError);
                    if (!lineError) { word |= (rs << 21) | (rt << 16) | (rd << 11) | (op->funct << 6); addCodeNode(codeHead, word, *IC, lineNum, NULL); *IC += 4; }
                }
                else if (op->type == I_TYPE) {
                    rs=rt=0; immed=0; labDep = NULL;
                    rs = parseRegOperand(&ptr, errorList, lineNum);
                    if (rs == -1) lineError = TRUE;

                    if (!lineError && !matchComma(&ptr, errorList, lineNum)) lineError = TRUE;

                    if ((op->opcode >= 10 && op->opcode <= 14) || (op->opcode >= 19 && op->opcode <= 24)) { /* rs, immed, rt */
                        if (!lineError) immed = parseImmedOperand(&ptr, errorList, lineNum, &lineError);
                        if (!lineError && !matchComma(&ptr, errorList, lineNum)) lineError = TRUE;
                        if (!lineError) { rt = parseRegOperand(&ptr, errorList, lineNum); if (rt == -1) lineError = TRUE; }
                    } else if (op->opcode >= 15 && op->opcode <= 18) { /* cond branch: rs, rt, label */
                        if (!lineError) { rt = parseRegOperand(&ptr, errorList, lineNum); if (rt == -1) lineError = TRUE; }
                        if (!lineError && !matchComma(&ptr, errorList, lineNum)) lineError = TRUE;
                        if (!lineError) labDep = parseLabelOperand(&ptr, errorList, lineNum, &lineError);
                    }
                    if (!lineError) checkExtraText(&ptr, errorList, lineNum, &lineError);
                    if (!lineError) { word |= (rs << 21) | (rt << 16) | (immed & 0xFFFF); addCodeNode(codeHead, word, *IC, lineNum, labDep); *IC += 4; }
                    if (labDep) free(labDep);
                }
                else if (op->type == J_TYPE) {
                    int regBit = 0, addr = 0; labDep = NULL;
                    if (op->opcode != 63) { /* jmp, la, call */
                        char *t = getToken(&ptr);
                        if (t) {
                            if (t[0] == '$') {
                                if (op->opcode == 31 || op->opcode == 32) { addError(errorList, lineNum, ERR_INVALID_IMMED, "la/call takes a label"); lineError = TRUE; }
                                else { regBit = 1; addr = getRegNum(t); if (addr == -1) { addError(errorList, lineNum, ERR_INVALID_REG, t); lineError = TRUE; } }
                            } else {
                                if (t[0] >= '0' && t[0] <= '9') { addError(errorList, lineNum, ERR_INVALID_IMMED, "J-type takes label or register"); lineError = TRUE; }
                                else { regBit = 0; labDep = duplicateString(t); }
                            }
                            free(t);
                        } else { addError(errorList, lineNum, ERR_EXTRA_TEXT, "missing operand"); lineError = TRUE; }
                    }
                    if (!lineError) checkExtraText(&ptr, errorList, lineNum, &lineError);
                    if (!lineError) { word |= (regBit << 25) | (addr & 0x1FFFFFF); addCodeNode(codeHead, word, *IC, lineNum, labDep); *IC += 4; }
                    if (labDep) free(labDep);
                }
            } else { addError(errorList, lineNum, ERR_UNKNOWN_COMMAND, token); lineError = TRUE; }
        }
        if (label) free(label);
        if (token) free(token);
        if (lineError) error = TRUE;
    }
    currSym = *symbols;
    while (currSym) { if (currSym->isData && !currSym->isExternal) currSym->value += *IC; currSym = currSym->next; }
    currData = *dataHead;
    while (currData) { currData->address += *IC; currData = currData->next; }
    fclose(fp);
    return !error;
}
