#include "first_pass.h"
#include "util.h"
#include <string.h>

void addCodeNode(CodeNode **head, unsigned int word, int address, int lineNum, char *labelDep) {
    CodeNode *newNode = (CodeNode *)safeMalloc(sizeof(CodeNode));
    newNode->word = word;
    newNode->address = address;
    newNode->lineNum = lineNum;
    newNode->labelDep = labelDep ? strdupp(labelDep) : NULL;
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

boolean checkData(char **ptr, DataNode **dataHead, int *DC, int size, const char *filename, int lineNum, ErrorNode **errorList) {
    char *t;
    long val;
    boolean err = FALSE; /* C90 requires declaration at top */
    boolean loopflag = FALSE;

    skipSpaces(ptr);
    if (**ptr == ',') { addError(errorList, lineNum, ERR_ILLEGAL_COMMA, NULL); return FALSE; }
    while (!loopflag && (t = getToken(ptr))) {
        val = atol(t);
        free(t);
        if (size == 1) addDataNode(dataHead, (unsigned char)val, (*DC)++);
        else if (size == 2) { addDataNode(dataHead, val & ((1UL << 8) - 1), (*DC)++); addDataNode(dataHead, (val >> 8) & ((1UL << 8) - 1), (*DC)++); }
        else if (size == 4) { addDataNode(dataHead, val & ((1UL << 8) - 1), (*DC)++); addDataNode(dataHead, (val >> 8) & ((1UL << 8) - 1), (*DC)++); addDataNode(dataHead, (val >> 16) & ((1UL << 8) - 1), (*DC)++); addDataNode(dataHead, (val >> 24) & ((1UL << 8) - 1), (*DC)++); }

        skipSpaces(ptr);
        if (**ptr == ',') {
            (*ptr)++;
            if (**ptr == ',') { addError(errorList, lineNum, ERR_MULTIPLE_COMMAS, NULL); return FALSE; }
            skipSpaces(ptr);
            if (**ptr == '\0' || **ptr == ';' || **ptr == '\n' || **ptr == '\r') { addError(errorList, lineNum, ERR_ILLEGAL_COMMA, "ends with comma"); return FALSE; }
        } else {
            loopflag = TRUE;
        }
    }

    checkExtraText(ptr, errorList, lineNum, &err);
    return !err;
}

/*
 * checks if the token is a valid label definition.
 * updates the label pointer if a valid label is found and sets lineError if there are issues.
 * the input is the token, label pointer, string pointer, error list, line number, symbols and macros.
 * returns 1 if it is a label definition (valid or invalid), 0 otherwise.
 */
int checkLabelDef(char **token, char **label, char **ptr, ErrorNode **errorList, int lineNum, SymbolNode *symbols, MacroNode *macros, boolean *lineError) {
    if (isLabelDef(*token)) {
        (*token)[strlen(*token)-1] = '\0';
        if (strlen(*token) > 31) { addError(errorList, lineNum, ERR_LABEL_TOO_LONG, *token); *lineError = TRUE; }
        else if (!isValidLabelFormat(*token)) { addError(errorList, lineNum, ERR_INVALID_LABEL_FORMAT, *token); *lineError = TRUE; }
        else if (isReservedKeyword(*token)) { addError(errorList, lineNum, ERR_RESERVED_KEYWORD, NULL); *lineError = TRUE; }
        else if (getSymbol(symbols, *token)) { addError(errorList, lineNum, ERR_SYMBOL_REDEFINITION, *token); *lineError = TRUE; }
        else if (getMacroContent(macros, *token)) { addError(errorList, lineNum, ERR_SYMBOL_REDEFINITION, "label has same name as a macro"); *lineError = TRUE; }
        *label = *token;
        *token = getToken(ptr);
        return 1;
    }
    return 0;
}

/*
 * processes the .asciz directive and extracts the string.
 * adds characters to data image and reports errors if missing quotes.
 * the input is string pointer, data head, DC, error list, line number, and lineError flag.
 * returns void.
 */
void procAsciz(char **ptr, DataNode **dataHead, int *DC, ErrorNode **errorList, int lineNum, boolean *lineError) {
    skipSpaces(ptr);
    if (**ptr == '\"') {
        (*ptr)++;
        while (**ptr && **ptr != '\"') addDataNode(dataHead, *(*ptr)++, (*DC)++);
        if (**ptr == '\"') { (*ptr)++; addDataNode(dataHead, '\0', (*DC)++); }
        else { addError(errorList, lineNum, ERR_EXTRA_TEXT, "missing closing quote"); *lineError = TRUE; }
    } else { addError(errorList, lineNum, ERR_EXTRA_TEXT, "missing string"); *lineError = TRUE; }
    checkExtraText(ptr, errorList, lineNum, lineError);
}

/*
 * processes the .extern directive and adds the symbol to the symbol table.
 * reports errors if the symbol is already defined locally.
 * the input is string pointer, symbol table, error list, line number, and lineError flag.
 * returns void.
 */
void procExt(char **ptr, SymbolNode **symbols, ErrorNode **errorList, int lineNum, boolean *lineError) {
    char *ext = getToken(ptr);
    if (ext) {
        SymbolNode *s = getSymbol(*symbols, ext);
        if (s && !s->isExternal) { addError(errorList, lineNum, ERR_SYMBOL_REDEFINITION, "external symbol already defined locally"); *lineError = TRUE; }
        else addSymbol(symbols, ext, 0, EXTERNAL);
        free(ext);
    } else { addError(errorList, lineNum, ERR_EXTRA_TEXT, "missing extern name"); *lineError = TRUE; }
    checkExtraText(ptr, errorList, lineNum, lineError);
}

/*
 * processes an assembler directive (.db, .dh, .dw, .asciz, .extern, .entry).
 * updates data image and symbol table accordingly.
 * the input is the token, string pointer, label, filename, line number, symbols, data head, DC, and error list.
 * returns TRUE if an error occurred during processing, FALSE otherwise.
 */
boolean procDirective(char *token, char **ptr, char *label, const char *filename, int lineNum, SymbolNode **symbols, DataNode **dataHead, int *DC, ErrorNode **errorList, boolean lineError) {
    if (label && !lineError) addSymbol(symbols, label, *DC, DATA);
    if (strcmp(token, ".db") == 0) { if (!checkData(ptr, dataHead, DC, 1, filename, lineNum, errorList)) lineError = TRUE; }
    else if (strcmp(token, ".dh") == 0) { if (!checkData(ptr, dataHead, DC, 2, filename, lineNum, errorList)) lineError = TRUE; }
    else if (strcmp(token, ".dw") == 0) { if (!checkData(ptr, dataHead, DC, 4, filename, lineNum, errorList)) lineError = TRUE; }
    else if (strcmp(token, ".asciz") == 0) { procAsciz(ptr, dataHead, DC, errorList, lineNum, &lineError); }
    else if (strcmp(token, ".extern") == 0) { procExt(ptr, symbols, errorList, lineNum, &lineError); }
    else if (strcmp(token, ".entry") != 0) { addError(errorList, lineNum, ERR_UNKNOWN_COMMAND, token); lineError = TRUE; }

    return lineError;
}

/*
 * processes an R-type instruction.
 * reads registers and updates the code image.
 * the input is string pointer, opcode, line number, code head, IC, error list, and lineError flag.
 * returns void.
 */
void procRType(char **ptr, Opcode *op, int lineNum, CodeNode **codeHead, int *IC, ErrorNode **errorList, boolean *lineError, unsigned int word) {
    int rs=0, rt=0, rd=0;
    rs = checkRegOperand(ptr, errorList, lineNum);
    if (rs == -1) *lineError = TRUE;

    if (!*lineError && !matchComma(ptr, errorList, lineNum)) *lineError = TRUE;

    if (op->opcode == 0) { /* arithmetic/logic: rs, rt, rd */
        if (!*lineError) { rt = checkRegOperand(ptr, errorList, lineNum); if (rt == -1) *lineError = TRUE; }
        if (!*lineError && !matchComma(ptr, errorList, lineNum)) *lineError = TRUE;
        if (!*lineError) { rd = checkRegOperand(ptr, errorList, lineNum); if (rd == -1) *lineError = TRUE; }
    } else { /* copy: rs, rd */
        if (!*lineError) { rd = checkRegOperand(ptr, errorList, lineNum); if (rd == -1) *lineError = TRUE; }
    }
    if (!*lineError) checkExtraText(ptr, errorList, lineNum, lineError);
    if (!*lineError) { word |= (rs << 21) | (rt << 16) | (rd << 11) | (op->funct << 6); addCodeNode(codeHead, word, *IC, lineNum, NULL); *IC += 4; }
}

/*
 * processes an I-type instruction.
 * reads registers and immediates/labels and updates the code image.
 * the input is string pointer, opcode, line number, code head, IC, error list, and lineError flag.
 * returns void.
 */
void procIType(char **ptr, Opcode *op, int lineNum, CodeNode **codeHead, int *IC, ErrorNode **errorList, boolean *lineError, unsigned int word) {
    int rs=0, rt=0; short immed=0; char *labDep = NULL;
    rs = checkRegOperand(ptr, errorList, lineNum);
    if (rs == -1) *lineError = TRUE;

    if (!*lineError && !matchComma(ptr, errorList, lineNum)) *lineError = TRUE;

    if ((op->opcode >= 10 && op->opcode <= 14) || (op->opcode >= 19 && op->opcode <= 24)) { /* rs, immed, rt */
        if (!*lineError) immed = checkImmedOperand(ptr, errorList, lineNum, lineError);
        if (!*lineError && !matchComma(ptr, errorList, lineNum)) *lineError = TRUE;
        if (!*lineError) { rt = checkRegOperand(ptr, errorList, lineNum); if (rt == -1) *lineError = TRUE; }
    } else if (op->opcode >= 15 && op->opcode <= 18) { /* cond branch: rs, rt, label */
        if (!*lineError) { rt = checkRegOperand(ptr, errorList, lineNum); if (rt == -1) *lineError = TRUE; }
        if (!*lineError && !matchComma(ptr, errorList, lineNum)) *lineError = TRUE;
        if (!*lineError) labDep = checkLabelOperand(ptr, errorList, lineNum, lineError);
    }
    if (!*lineError) checkExtraText(ptr, errorList, lineNum, lineError);
    if (!*lineError) { word |= (rs << 21) | (rt << 16) | (immed & ((1UL << 16) - 1)); addCodeNode(codeHead, word, *IC, lineNum, labDep); *IC += 4; }
    if (labDep) free(labDep);
}

/*
 * processes a J-type instruction.
 * reads the address or label and updates the code image.
 * the input is string pointer, opcode, line number, code head, IC, error list, and lineError flag.
 * returns void.
 */
void procJType(char **ptr, Opcode *op, int lineNum, CodeNode **codeHead, int *IC, ErrorNode **errorList, boolean *lineError, unsigned int word) {
    int regBit = 0, addr = 0; char *labDep = NULL;
    if (op->opcode != 63) { /* jmp, la, call */
        char *t = getToken(ptr);
        if (t) {
            if (t[0] == '$') {
                if (op->opcode == 31 || op->opcode == 32) { addError(errorList, lineNum, ERR_INVALID_IMMED, "la/call takes a label"); *lineError = TRUE; }
                else { regBit = 1; addr = getRegNum(t); if (addr == -1) { addError(errorList, lineNum, ERR_INVALID_REG, t); *lineError = TRUE; } }
            } else {
                if (t[0] >= '0' && t[0] <= '9') { addError(errorList, lineNum, ERR_INVALID_IMMED, "J-type takes label or register"); *lineError = TRUE; }
                else { regBit = 0; labDep = strdupp(t); }
            }
            free(t);
        } else { addError(errorList, lineNum, ERR_EXTRA_TEXT, "missing operand"); *lineError = TRUE; }
    }
    if (!*lineError) checkExtraText(ptr, errorList, lineNum, lineError);
    if (!*lineError) { word |= (regBit << 25) | (addr & ((1UL << 25) - 1)); addCodeNode(codeHead, word, *IC, lineNum, labDep); *IC += 4; }
    if (labDep) free(labDep);
}

/*
 * processes an instruction and delegates to the appropriate format handler.
 * the input is the token, string pointer, label, line number, symbols, code head, IC, error list, and lineError flag.
 * returns TRUE if an error occurred during processing, FALSE otherwise.
 */
boolean procInstruction(char *token, char **ptr, char *label, int lineNum, SymbolNode **symbols, CodeNode **codeHead, int *IC, ErrorNode **errorList, boolean lineError) {
    Opcode *op = getOpcode(token);
    unsigned int word;
    if (op) {
        word = (op->opcode << 26);
        if (label && !lineError) addSymbol(symbols, label, *IC, CODE);

        if (op->type == R_TYPE) {
            procRType(ptr, op, lineNum, codeHead, IC, errorList, &lineError, word);
        }
        else if (op->type == I_TYPE) {
            procIType(ptr, op, lineNum, codeHead, IC, errorList, &lineError, word);
        }
        else if (op->type == J_TYPE) {
            procJType(ptr, op, lineNum, codeHead, IC, errorList, &lineError, word);
        }
    } else {
        addError(errorList, lineNum, ERR_UNKNOWN_COMMAND, token);
        lineError = TRUE;
    }
    return lineError;
}

boolean firstPass(const char *filename, SymbolNode **symbols, CodeNode **codeHead, DataNode **dataHead, int *IC, int *DC, ErrorNode **errorList, MacroNode *macros) {
    char amName[MAX_LINE_LENGTH];
    FILE *fp;
    char line[MAX_LINE_LENGTH + 2];
    int lineNum = 0;
    boolean error = FALSE, lineError;
    char *ptr, *token, *label;
    SymbolNode *currSym;
    DataNode *currData;

    strcpy(amName, filename); strcat(amName, ".am");
    fp = fopen(amName, "r");
    if (!fp) { addError(errorList, 0, ERR_OPEN_FILE, amName); return FALSE; }

    while (fgets(line, sizeof(line), fp)) {
        ptr = line; label = NULL; lineNum++; lineError = FALSE;

        if (!checkLineLen(line)) {
            addError(errorList, lineNum, ERR_LINE_TOO_LONG, NULL);
            error = TRUE;
        } else if (!isEmptyLine(line) && !isCommentLine(line)) {
            token = getToken(&ptr);
            checkLabelDef(&token, &label, &ptr, errorList, lineNum, *symbols, macros, &lineError);

            if (!token) {
                if (label) free(label);
                if (lineError) error = TRUE;
            } else {
                if (token[0] == '.') {
                    lineError = procDirective(token, &ptr, label, filename, lineNum, symbols, dataHead, DC, errorList, lineError);
                } else {
                    lineError = procInstruction(token, &ptr, label, lineNum, symbols, codeHead, IC, errorList, lineError);
                }
                if (label) free(label);
                if (token) free(token);
                if (lineError) error = TRUE;
            }
        }
    }
    currSym = *symbols;
    while (currSym) { if (currSym->isData && !currSym->isExternal) currSym->value += *IC; currSym = currSym->next; }
    currData = *dataHead;
    while (currData) { currData->address += *IC; currData = currData->next; }
    fclose(fp);
    return !error;
}
