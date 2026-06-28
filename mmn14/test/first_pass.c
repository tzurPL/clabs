/*
 * first_pass.c
 * mmn14
 * Tzur Pinto Lazar
 */

#include "first_pass.h"
#include "util.h"
#include <string.h>

/*
 * addCodeNode func
 * adds a new code node to the end of the code list.
 * the input is a pointer to the head of the code list, the machine word, address, line number, and optional label dependency.
 * returns void.
 */
void addCodeNode(CodeNode **head, unsigned int word, int address, int lineNum, char *labelDep) {
    CodeNode *newNode = (CodeNode *)safeMalloc(sizeof(CodeNode));/*allocate memory for new node*/
    newNode->word = word;/*set the machine word*/
    newNode->address = address;/*set the address*/
    newNode->lineNum = lineNum;/*set the line number*/
    newNode->labelDep = labelDep ? strdupp(labelDep) : NULL;/*copy label dependency if exists*/
    newNode->next = NULL;
    if (!*head) *head = newNode;/*if list is empty, set as head*/
    else {
        CodeNode *curr = *head;
        while (curr->next) curr = curr->next;/*find the end of the list*/
        curr->next = newNode;/*append the new node*/
    }
}

/*
 * addDataNode func
 * adds a new data node to the end of the data list.
 * the input is a pointer to the head of the data list, the byte value, and address.
 * returns void.
 */
void addDataNode(DataNode **head, unsigned char byte, int address) {
    DataNode *newNode = (DataNode *)safeMalloc(sizeof(DataNode));/*allocate memory for new node*/
    newNode->byte = byte;/*set the data byte*/
    newNode->address = address;/*set the address*/
    newNode->next = NULL;
    if (!*head) *head = newNode;/*if list is empty, set as head*/
    else {
        DataNode *curr = *head;
        while (curr->next) curr = curr->next;/*find the end of the list*/
        curr->next = newNode;/*append the new node*/
    }
}

/*
 * checkData func
 * parses and validates comma-separated data values from a directive and adds them to the data image.
 * the input is string pointer, data head, DC, size of data elements, filename, line number, and error list.
 * returns boolean indicating whether the parsing was successful without errors.
 */
boolean checkData(char **ptr, DataNode **dataHead, int *DC, int size, const char *filename, int lineNum, ErrorNode **errorList) {
    char *t;
    long val;
    boolean err = FALSE; /* C90 requires declaration at top */
    boolean loopflag = FALSE;

    skipSpaces(ptr);
    if (**ptr == ',') { addError(errorList, lineNum, ERR_ILLEGAL_COMMA, NULL); return FALSE; }/*check for leading comma*/
    while (!loopflag && (t = getToken(ptr))) {/*iterate through tokens*/
        val = atol(t);/*convert token to integer*/
        free(t);
        /*add data nodes depending on the size of the elements*/
        if (size == 1) addDataNode(dataHead, (unsigned char)val, (*DC)++);
        else if (size == 2) { addDataNode(dataHead, val & ((1UL << 8) - 1), (*DC)++); addDataNode(dataHead, (val >> 8) & ((1UL << 8) - 1), (*DC)++); }
        else if (size == 4) { addDataNode(dataHead, val & ((1UL << 8) - 1), (*DC)++); addDataNode(dataHead, (val >> 8) & ((1UL << 8) - 1), (*DC)++); addDataNode(dataHead, (val >> 16) & ((1UL << 8) - 1), (*DC)++); addDataNode(dataHead, (val >> 24) & ((1UL << 8) - 1), (*DC)++); }

        skipSpaces(ptr);
        if (**ptr == ',') {/*if comma found, process next element*/
            (*ptr)++;
            if (**ptr == ',') { addError(errorList, lineNum, ERR_MULTIPLE_COMMAS, NULL); return FALSE; }/*check for multiple commas*/
            skipSpaces(ptr);
            if (**ptr == '\0' || **ptr == ';' || **ptr == '\n' || **ptr == '\r') { addError(errorList, lineNum, ERR_ILLEGAL_COMMA, "ends with comma"); return FALSE; }/*check for trailing comma*/
        } else {
            loopflag = TRUE;/*no comma found, end of data list*/
        }
    }

    checkExtraText(ptr, errorList, lineNum, &err);
    return !err;
}

/*
 * checkLabelDef func
 * checks if the token is a valid label definition. updates the label pointer if a valid label is found and sets lineError if there are issues.
 * the input is the token, label pointer, string pointer, error list, line number, symbols and macros.
 * returns 1 if it is a label definition (valid or invalid), 0 otherwise.
 */
int checkLabelDef(char **token, char **label, char **ptr, ErrorNode **errorList, int lineNum, SymbolNode *symbols, MacroNode *macros, boolean *lineError) {
    if (isLabelDef(*token)) {/*check if token ends with a colon*/
        (*token)[strlen(*token)-1] = '\0';/*remove colon*/
        /*validate label format and report errors if necessary*/
        if (strlen(*token) > 31) { addError(errorList, lineNum, ERR_LABEL_TOO_LONG, *token); *lineError = TRUE; }
        else if (!checkLabelN(*token)) { addError(errorList, lineNum, ERR_INVALID_LABEL_FORMAT, *token); *lineError = TRUE; }
        else if (isReservedKeyword(*token)) { addError(errorList, lineNum, ERR_RESERVED_KEYWORD, NULL); *lineError = TRUE; }
        else if (getSymbol(symbols, *token)) { addError(errorList, lineNum, ERR_SYMBOL_REDEFINITION, *token); *lineError = TRUE; }
        else if (getMacroContent(macros, *token)) { addError(errorList, lineNum, ERR_SYMBOL_REDEFINITION, "label has same name as a macro"); *lineError = TRUE; }
        *label = *token;/*save the valid label*/
        *token = getToken(ptr);/*fetch the next token*/
        return 1;
    }
    return 0;
}

/*
 * procAsciz func
 * processes the .asciz directive and extracts the string. adds characters to data image and reports errors if missing quotes.
 * the input is string pointer, data head, DC, error list, line number, and lineError flag.
 * returns void.
 */
void procAsciz(char **ptr, DataNode **dataHead, int *DC, ErrorNode **errorList, int lineNum, boolean *lineError) {
    skipSpaces(ptr);/*skip leading spaces*/
    if (**ptr == '\"') {/*check for opening quote*/
        (*ptr)++;/*move past opening quote*/
        while (**ptr && **ptr != '\"') addDataNode(dataHead, *(*ptr)++, (*DC)++);/*add each character to data list*/
        if (**ptr == '\"') { (*ptr)++; addDataNode(dataHead, '\0', (*DC)++); }/*null-terminate the string*/
        else { addError(errorList, lineNum, ERR_EXTRA_TEXT, "missing closing quote"); *lineError = TRUE; }/*report missing quote*/
    } else { addError(errorList, lineNum, ERR_EXTRA_TEXT, "missing string"); *lineError = TRUE; }/*report missing string*/
    checkExtraText(ptr, errorList, lineNum, lineError);/*check for extra text*/
}

/*
 * procExt func
 * processes the .extern directive and adds the symbol to the symbol table. reports errors if the symbol is already defined locally.
 * the input is string pointer, symbol table, error list, line number, and lineError flag.
 * returns void.
 */
void procExt(char **ptr, SymbolNode **symbols, ErrorNode **errorList, int lineNum, boolean *lineError) {
    char *ext = getToken(ptr);/*get the external symbol name*/
    if (ext) {/*if name was provided*/
        SymbolNode *s = getSymbol(*symbols, ext);/*check if symbol exists*/
        if (s && !s->isExternal) { addError(errorList, lineNum, ERR_SYMBOL_REDEFINITION, "external symbol already defined locally"); *lineError = TRUE; }/*report conflict*/
        else addSymbol(symbols, ext, 0, EXTERNAL);/*add external symbol*/
        free(ext);/*free the token*/
    } else { addError(errorList, lineNum, ERR_EXTRA_TEXT, "missing extern name"); *lineError = TRUE; }/*report missing name*/
    checkExtraText(ptr, errorList, lineNum, lineError);/*check for extra text*/
}

/*
 * procDirective func
 * processes an assembler directive (.db, .dh, .dw, .asciz, .extern, .entry). updates data image and symbol table accordingly.
 * the input is the token, string pointer, label, filename, line number, symbols, data head, DC, and error list.
 * returns TRUE if an error occurred during processing, FALSE otherwise.
 */
boolean procDirective(char *token, char **ptr, char *label, const char *filename, int lineNum, SymbolNode **symbols, DataNode **dataHead, int *DC, ErrorNode **errorList, boolean lineError) {
    if (label && !lineError) addSymbol(symbols, label, *DC, DATA);/*add label to symbol table if present*/
    /*process specific data directives based on type*/
    if (strcmp(token, ".db") == 0) { if (!checkData(ptr, dataHead, DC, 1, filename, lineNum, errorList)) lineError = TRUE; }
    else if (strcmp(token, ".dh") == 0) { if (!checkData(ptr, dataHead, DC, 2, filename, lineNum, errorList)) lineError = TRUE; }
    else if (strcmp(token, ".dw") == 0) { if (!checkData(ptr, dataHead, DC, 4, filename, lineNum, errorList)) lineError = TRUE; }
    else if (strcmp(token, ".asciz") == 0) { procAsciz(ptr, dataHead, DC, errorList, lineNum, &lineError); }
    else if (strcmp(token, ".extern") == 0) { procExt(ptr, symbols, errorList, lineNum, &lineError); }
    else if (strcmp(token, ".entry") != 0) { addError(errorList, lineNum, ERR_UNKNOWN_COMMAND, token); lineError = TRUE; }/*report unknown directive*/

    return lineError;/*return accumulated error status*/
}

/*
 * procRType func
 * processes an R-type instruction. reads registers and updates the code image.
 * the input is string pointer, opcode, line number, code head, IC, error list, and lineError flag.
 * returns void.
 */
void procRType(char **ptr, Opcode *op, int lineNum, CodeNode **codeHead, int *IC, ErrorNode **errorList, boolean *lineError, unsigned int word) {
    int rs=0, rt=0, rd=0;
    rs = checkRegOperand(ptr, errorList, lineNum);/*extract first register (rs)*/
    if (rs == -1) *lineError = TRUE;/*set error flag if invalid*/

    if (!*lineError && !matchComma(ptr, errorList, lineNum)) *lineError = TRUE;/*ensure comma separates operands*/

    if (op->opcode == 0) { /*arithmetic/logic: rs, rt, rd*/
        if (!*lineError) { rt = checkRegOperand(ptr, errorList, lineNum); if (rt == -1) *lineError = TRUE; }/*extract rt*/
        if (!*lineError && !matchComma(ptr, errorList, lineNum)) *lineError = TRUE;/*ensure comma*/
        if (!*lineError) { rd = checkRegOperand(ptr, errorList, lineNum); if (rd == -1) *lineError = TRUE; }/*extract rd*/
    } else { /*copy: rs, rd*/
        if (!*lineError) { rd = checkRegOperand(ptr, errorList, lineNum); if (rd == -1) *lineError = TRUE; }/*extract rd directly after rs*/
    }
    if (!*lineError) checkExtraText(ptr, errorList, lineNum, lineError);/*check for extraneous text*/
    if (!*lineError) { word |= (rs << 21) | (rt << 16) | (rd << 11) | (op->funct << 6); addCodeNode(codeHead, word, *IC, lineNum, NULL); *IC += 4; }/*construct and append code node*/
}

/*
 * procIType func
 * processes an I-type instruction. reads registers and immediates/labels and updates the code image.
 * the input is string pointer, opcode, line number, code head, IC, error list, and lineError flag.
 * returns void.
 */
void procIType(char **ptr, Opcode *op, int lineNum, CodeNode **codeHead, int *IC, ErrorNode **errorList, boolean *lineError, unsigned int word) {
    int rs=0, rt=0; short immed=0; char *labDep = NULL;
    rs = checkRegOperand(ptr, errorList, lineNum);/*extract first register (rs)*/
    if (rs == -1) *lineError = TRUE;/*set error flag if invalid*/

    if (!*lineError && !matchComma(ptr, errorList, lineNum)) *lineError = TRUE;/*ensure comma separates operands*/

    if ((op->opcode >= 10 && op->opcode <= 14) || (op->opcode >= 19 && op->opcode <= 24)) { /*rs, immed, rt*/
        if (!*lineError) immed = checkImmedOperand(ptr, errorList, lineNum, lineError);/*extract immediate value*/
        if (!*lineError && !matchComma(ptr, errorList, lineNum)) *lineError = TRUE;/*ensure comma*/
        if (!*lineError) { rt = checkRegOperand(ptr, errorList, lineNum); if (rt == -1) *lineError = TRUE; }/*extract target register (rt)*/
    } else if (op->opcode >= 15 && op->opcode <= 18) { /*cond branch: rs, rt, label*/
        if (!*lineError) { rt = checkRegOperand(ptr, errorList, lineNum); if (rt == -1) *lineError = TRUE; }/*extract second register (rt)*/
        if (!*lineError && !matchComma(ptr, errorList, lineNum)) *lineError = TRUE;/*ensure comma*/
        if (!*lineError) labDep = checkLabelOperand(ptr, errorList, lineNum, lineError);/*extract label dependency*/
    }
    if (!*lineError) checkExtraText(ptr, errorList, lineNum, lineError);/*check for extraneous text*/
    if (!*lineError) { word |= (rs << 21) | (rt << 16) | (immed & ((1UL << 16) - 1)); addCodeNode(codeHead, word, *IC, lineNum, labDep); *IC += 4; }/*construct and append code node*/
    if (labDep) free(labDep);/*free the label string if allocated*/
}

/*
 * procJType func
 * processes a J-type instruction. reads the address or label and updates the code image.
 * the input is string pointer, opcode, line number, code head, IC, error list, and lineError flag.
 * returns void.
 */
void procJType(char **ptr, Opcode *op, int lineNum, CodeNode **codeHead, int *IC, ErrorNode **errorList, boolean *lineError, unsigned int word) {
    int regBit = 0, addr = 0; char *labDep = NULL;
    if (op->opcode != 63) { /*jmp, la, call*/
        char *t = getToken(ptr);/*get the single operand token*/
        if (t) {/*if operand exists*/
            if (t[0] == '$') {/*check if operand is a register*/
                if (op->opcode == 31 || op->opcode == 32) { addError(errorList, lineNum, ERR_INVALID_IMMED, "la/call takes a label"); *lineError = TRUE; }/*validate operand type*/
                else { regBit = 1; addr = getRegNum(t); if (addr == -1) { addError(errorList, lineNum, ERR_INVALID_REG, t); *lineError = TRUE; } }/*extract register*/
            } else {/*operand is a label*/
                if (t[0] >= '0' && t[0] <= '9') { addError(errorList, lineNum, ERR_INVALID_IMMED, "J-type takes label or register"); *lineError = TRUE; }/*validate operand type*/
                else { regBit = 0; labDep = strdupp(t); }/*record label dependency*/
            }
            free(t);/*free the token*/
        } else { addError(errorList, lineNum, ERR_EXTRA_TEXT, "missing operand"); *lineError = TRUE; }/*report missing operand*/
    }
    if (!*lineError) checkExtraText(ptr, errorList, lineNum, lineError);/*check for extraneous text*/
    if (!*lineError) { word |= (regBit << 25) | (addr & ((1UL << 25) - 1)); addCodeNode(codeHead, word, *IC, lineNum, labDep); *IC += 4; }/*construct and append code node*/
    if (labDep) free(labDep);/*free the label string if allocated*/
}

/*
 * procInstruction func
 * processes an instruction and delegates to the appropriate format handler.
 * the input is the token, string pointer, label, line number, symbols, code head, IC, error list, and lineError flag.
 * returns TRUE if an error occurred during processing, FALSE otherwise.
 */
boolean procInstruction(char *token, char **ptr, char *label, int lineNum, SymbolNode **symbols, CodeNode **codeHead, int *IC, ErrorNode **errorList, boolean lineError) {
    Opcode *op = getOpcode(token);/*look up the instruction opcode*/
    unsigned int word;
    if (op) {/*if valid instruction*/
        word = (op->opcode << 26);/*initialize the word with the opcode*/
        if (label && !lineError) addSymbol(symbols, label, *IC, CODE);/*add label to symbol table if present*/

        /*dispatch to the appropriate format handler*/
        if (op->type == R_TYPE) {
            procRType(ptr, op, lineNum, codeHead, IC, errorList, &lineError, word);
        }
        else if (op->type == I_TYPE) {
            procIType(ptr, op, lineNum, codeHead, IC, errorList, &lineError, word);
        }
        else if (op->type == J_TYPE) {
            procJType(ptr, op, lineNum, codeHead, IC, errorList, &lineError, word);
        }
    } else {/*invalid instruction*/
        addError(errorList, lineNum, ERR_UNKNOWN_COMMAND, token);/*report unknown command*/
        lineError = TRUE;/*set error flag*/
    }
    return lineError;/*return accumulated error status*/
}

/*
 * firstPass func
 * executes the first pass of the assembler, processing labels, directives, and instructions to build the symbol table and data/code images.
 * the input is filename, symbols list, code list, data list, IC, DC, error list, and macros.
 * returns boolean indicating whether the pass was completely successful.
 */
boolean firstPass(const char *filename, SymbolNode **symbols, CodeNode **codeHead, DataNode **dataHead, int *IC, int *DC, ErrorNode **errorList, MacroNode *macros) {
    char amName[MAX_LINE_LENGTH];
    FILE *fp;
    char line[MAX_LINE_LENGTH + 2];
    int lineNum = 0;
    boolean error = FALSE, lineError;
    char *ptr, *token, *label;
    SymbolNode *currSym;
    DataNode *currData;

    strcpy(amName, filename); strcat(amName, ".am");/*append .am extension*/
    fp = fopen(amName, "r");/*open the file for reading*/
    if (!fp) { addError(errorList, 0, ERR_OPEN_FILE, amName); return FALSE; }/*return false if file open failed*/

    while (fgets(line, sizeof(line), fp)) {/*read line by line*/
        ptr = line; label = NULL; lineNum++; lineError = FALSE;

        if (!checkLineLen(line)) {/*check if line is too long*/
            addError(errorList, lineNum, ERR_LINE_TOO_LONG, NULL);
            error = TRUE;
        } else if (!isEmptyLine(line) && !isCommentLine(line)) {/*ignore empty and comment lines*/
            token = getToken(&ptr);/*get first token*/
            checkLabelDef(&token, &label, &ptr, errorList, lineNum, *symbols, macros, &lineError);/*check if it is a label definition*/

            if (!token) {
                if (label) free(label);
                if (lineError) error = TRUE;
            } else {
                if (token[0] == '.') {/*it is a directive*/
                    lineError = procDirective(token, &ptr, label, filename, lineNum, symbols, dataHead, DC, errorList, lineError);
                } else {/*it is an instruction*/
                    lineError = procInstruction(token, &ptr, label, lineNum, symbols, codeHead, IC, errorList, lineError);
                }
                if (label) free(label);
                if (token) free(token);
                if (lineError) error = TRUE;
            }
        }
    }
    currSym = *symbols;
    while (currSym) { if (currSym->isData && !currSym->isExternal) currSym->value += *IC; currSym = currSym->next; }/*update data symbol addresses*/
    currData = *dataHead;
    while (currData) { currData->address += *IC; currData = currData->next; }/*update data node addresses*/
    fclose(fp);/*close the file*/
    return !error;
}
