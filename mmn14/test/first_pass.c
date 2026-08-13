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
 * adds new code node to end of code list
 * the input is a pointer to the head of the code list, instruction, address, line num,  label reference
 * returns void
 */
void addCodeNode(CodeNode **head, Instruction inst, int address, int lineNum, char *labelRef) {
    CodeNode *newNode = (CodeNode *)safeMalloc(sizeof(CodeNode));/*alloc memory for new node*/
    /*add details to node */
    newNode->inst = inst;
    newNode->address = address;
    newNode->lineNum = lineNum;
    newNode->labelRef = labelRef ? strdupp(labelRef) : NULL;/*copy label reference if exists*/
    newNode->next = NULL;
    if (!*head) {
        *head = newNode;/*if list empty set as head*/
    } else {
        CodeNode *curr = *head;
        while (curr->next) {
            curr = curr->next;/*find the end of list*/
        }
        curr->next = newNode;/*add new node*/
    }
}

/*
 * addDataNode func
 * adds a new data node to the end of the data list
 * the input is a pointer to the head of the data list, the byte value,  address
 * returns void
 */
void addDataNode(DataNode **head, unsigned char byte, int address) {
    DataNode *newNode = (DataNode *)safeMalloc(sizeof(DataNode));/*alloc memory for new node*/
    newNode->byte = byte;/*set data byte*/
    newNode->address = address;/*set address*/
    newNode->next = NULL;
    if (!*head) {
        *head = newNode;/*if list empty set as head*/
    } else {
        DataNode *curr = *head;
        while (curr->next) {
            curr = curr->next;/*find end of list*/
        }
        curr->next = newNode;/*add new node*/
    }
}

/*
 * checkData func
 * checks data values from a directive and adds them to data list
 * the input is string pointer, data head, DC, size of data elements, filename, line num, error list
 * returns true if succeded false if not
 */
boolean checkData(char **ptr, DataNode **dataHead, int *DC, int size, const char *filename, int lineNum,
                  ErrorNode **errorList) {
    char *t;
    long val;
    char *endptr;
    boolean err = FALSE;
    boolean loopflag = FALSE;
    /*disect the line */
    skipSpaces(ptr);
    if (**ptr == ',') {
        addError(errorList, lineNum, ERR_ILLEGAL_COMMA, NULL);
        return FALSE;
    }/*check for comma*/
    while (!loopflag && (t = getToken(ptr))) {/*go through tokens*/
        val = strtol(t, &endptr, 10);/*convert token to int*/
        if (*endptr != '\0') {/*check for invalid chars*/
            addError(errorList, lineNum, ERR_INVALID_IMMED, t);
            free(t);
            return FALSE;
        }
        free(t);
        /*add data nodes by checking sizes*/
        if (size == 1) {/*db */
            addDataNode(dataHead, (unsigned char)val, (*DC)++);/*add node and increase count */
        } else if (size == (NUM_BYTES_WORD / 2)) {/*dh */
            mCode mc;/*machine code node */
            mc.rawCode = val;/*copy it to it */
            /*each data node can hold 1byte so a 2byte data chunck is split to two */
            addDataNode(dataHead, mc.bytes.b0, (*DC)++);
            addDataNode(dataHead, mc.bytes.b1, (*DC)++);
        } else if (size == NUM_BYTES_WORD) {/*dw */
            mCode mc;
            mc.rawCode = val;
            /*split the 4byte chunck to 4 data nodes */
            addDataNode(dataHead, mc.bytes.b0, (*DC)++);
            addDataNode(dataHead, mc.bytes.b1, (*DC)++);
            addDataNode(dataHead, mc.bytes.b2, (*DC)++);
            addDataNode(dataHead, mc.bytes.b3, (*DC)++);
        }

        skipSpaces(ptr);
        if (**ptr == ',') {/*if comma found process next data*/
            (*ptr)++;
            if (**ptr == ',') {
                addError(errorList, lineNum, ERR_MULTIPLE_COMMAS, NULL);
                return FALSE;
            }/*check for multiple commas*/
            skipSpaces(ptr);
            if (**ptr == '\0' || **ptr == ';' || **ptr == '\n' || **ptr == '\r') {
                addError(errorList, lineNum, ERR_ILLEGAL_COMMA, "ends with comma");
                return FALSE;
            }/*check for comma at end*/
        } else {
            loopflag = TRUE;/*no comma found & end of data list*/
        }
    }

    checkExtraText(ptr, errorList, lineNum, &err);
    return !err;
}

/*
 * checkLabelDef func
 * checks if token is a valid label definition
 * the input is the token, label pointer, string pointer, error list, line num,symbols and macros
 * returns 1 if it is a label definition 0 if not.
 */
int checkLabelDef(char **token, char **label, char **ptr, ErrorNode **errorList, int lineNum, SymbolNode *symbols,MacroNode *macros, boolean *lineError) {
    if (isLabelDef(*token)) {/*check if token ends with a , */
        (*token)[strlen(*token) - 1] = '\0';/*remove , */

        /*check label format and save errors if found*/
        if (strlen(*token) > MAX_LABEL_LEN) {/*if label is longer than size needed for label*/
            addError(errorList, lineNum, ERR_LABEL_TOO_LONG, *token);
            *lineError = TRUE;
        }
        else if (!checkLabelN(*token)) {/*check defenition of the label name*/
            addError(errorList, lineNum, ERR_INVALID_LABEL_FORMAT, *token);
            *lineError = TRUE;
        } else if (isReservedKeyword(*token)) {/*check if label name is reserved keyword */
            addError(errorList, lineNum, ERR_RESERVED_KEYWORD, NULL);
            *lineError = TRUE;
        } else if (getSymbol(symbols, *token)) {/*if label is already defined*/
            addError(errorList, lineNum, ERR_SYMBOL_REDEFINITION, *token);
            *lineError = TRUE;
        } else if (getMacroContent(macros, *token)) {/*check if label is also macro name */
            addError(errorList, lineNum, ERR_SYMBOL_REDEFINITION, "label has same name as a macro");
            *lineError = TRUE;
        }
        *label = *token;/*save valid label*/
        *token = getToken(ptr);/*get next token*/
        return 1;
    }
    return 0;
}

/*
 * procAsciz func
 * processes the .asciz dirc and gets the string, adds chars to data list and reports errors if needed
 * the input is string pointer, data head, DC, error list, line num,lineError flag
 * returns void
 */
void procAsciz(char **ptr, DataNode **dataHead, int *DC, ErrorNode **errorList, int lineNum, boolean *lineError) {
    skipSpaces(ptr);

    if (**ptr == '\"') {/*check for " */
        (*ptr)++;/*move past " */
        while (**ptr && **ptr != '\"') {
            addDataNode(dataHead, *(*ptr)++, (*DC)++);/*add each char to data list*/
        }
        if (**ptr == '\"') {
            (*ptr)++;
            addDataNode(dataHead, '\0', (*DC)++);
        }/*add \0 to string*/
        else {
            addError(errorList, lineNum, ERR_MISSING_QUOTE, NULL);
            *lineError = TRUE;
        }/*report missing  " */
    } else {
        addError(errorList, lineNum, ERR_MISSING_STRING, NULL);
        *lineError = TRUE;
    }/*report missing string*/
    checkExtraText(ptr, errorList, lineNum, lineError);/*check for extra text*/
}

/*
 * procExt func
 * processes .extern dirc and adds the symbol to the symbol table, reports errors if symbol is already defined
 * the input is string pointer, symbol table, error list, line num,lineError flag
 * returns void
 */
void procExt(char **ptr, SymbolNode **symbols, ErrorNode **errorList, int lineNum, boolean *lineError) {
    char *ext = getToken(ptr);/*get external symbol name*/
    if (ext) {/*if name was found*/
        SymbolNode *s = getSymbol(*symbols, ext);/*check if symbol exists*/
        if (s && !s->isExternal) {
            addError(errorList, lineNum, ERR_SYMBOL_REDEFINITION, "external symbol already defined locally");
            *lineError = TRUE;
        }/*if symbol is found with external def and local def report error*/
        else {
            addSymbol(symbols, ext, 0, EXTERNAL);/*add external symbol*/
        }
        free(ext);/*free token*/
    } else {
        addError(errorList, lineNum, ERR_MISSING_OPERAND, "missing extern name");
        *lineError = TRUE;
    }/*report missing name*/
    checkExtraText(ptr, errorList, lineNum, lineError);/*check for extra text*/
}

/*
 * procDirective func
 * processes regular directive and updates data image and symbol table
 * the input is the token, string pointer, label, filename, line num, symbols, data head, DC,errorlist
 * returns true if error found if not false
 */
boolean procDirective(char *token, char **ptr, char *label, const char *filename, int lineNum, SymbolNode **symbols,DataNode **dataHead, int *DC, ErrorNode **errorList, boolean lineError) {

    if (label && !lineError) { addSymbol(symbols, label, *DC, DATA);/*add label to symbol table if exists*/ }
    /*process data directives based on type*/
    if (strcmp(token, ".db") == 0) {
        if (!checkData(ptr, dataHead, DC, 1, filename, lineNum, errorList)) { lineError = TRUE; }
    } else if (strcmp(token, ".dh") == 0) {
        if (!checkData(ptr, dataHead, DC, (NUM_BYTES_WORD / 2), filename, lineNum, errorList)) { lineError = TRUE; }
    } else if (strcmp(token, ".dw") == 0) {
        if (!checkData(ptr, dataHead, DC, NUM_BYTES_WORD, filename, lineNum, errorList)) { lineError = TRUE; }
    } else if (strcmp(token, ".asciz") == 0) {
        procAsciz(ptr, dataHead, DC, errorList, lineNum, &lineError);
    } else if (strcmp(token, ".extern") == 0) {
        procExt(ptr, symbols, errorList, lineNum, &lineError);
    } else if (strcmp(token, ".entry") != 0) {
        addError(errorList, lineNum, ERR_UNKNOWN_COMMAND, token);
        lineError = TRUE;
    }/*report unknown dir*/

    return lineError;/*return if errors found*/
}

/*
 * procRType func
 * processes an R type instructions and updates the code list
 * the input is string pointer, opcode, line num, code head, IC, error list, lineError flag
 * returns void
 */
void procRType(char **ptr, Opcode *op, int lineNum, CodeNode **codeHead, int *IC, ErrorNode **errorList,boolean *lineError, Instruction inst) {

    int rs = 0, rt = 0, rd = 0;
    rs = getReg(ptr, errorList, lineNum);/*get first register*/
    if (rs == -1) { *lineError = TRUE;/*set error flag if invalid*/ }

    if (!*lineError && !matchComma(ptr, errorList, lineNum)) { *lineError = TRUE;/*check comma separates operands*/ }

    if (op->opcode == 0) {/*if the operation is arethmetic then it uses all the registers*/
        if (!*lineError) {
            rt = getReg(ptr, errorList, lineNum);
            if (rt == -1) { *lineError = TRUE; }
        }/*get rt*/
        if (!*lineError && !matchComma(ptr, errorList, lineNum)) { *lineError = TRUE;/*check for , */ }
        if (!*lineError) {
            rd = getReg(ptr, errorList, lineNum);
            if (rd == -1) { *lineError = TRUE; }
        }/*get rd*/
    } else {/*operation only uses 2 registers, the rs was already taken so only one more register is in use*/
        if (!*lineError) {
            rd = getReg(ptr, errorList, lineNum);
            if (rd == -1) { *lineError = TRUE; }
        }/*get rd after rs*/
    }
    if (!*lineError) { checkExtraText(ptr, errorList, lineNum, lineError);/*check for extra text*/ }
    if (!*lineError) {/*write instruction */
        inst.r.rs = rs;
        inst.r.rt = rt;
        inst.r.rd = rd;
        inst.r.funct = op->funct;
        addCodeNode(codeHead, inst, *IC, lineNum, NULL);/*save it */
        *IC += NUM_BYTES_WORD;/*increase counter */
    }
}

/*
 * procIType func
 * processes I type instruction and updates the code list
 * the input is string pointer, opcode, line num, code head, IC, error list, lineError flag
 * returns void
 */
void procIType(char **ptr, Opcode *op, int lineNum, CodeNode **codeHead, int *IC, ErrorNode **errorList,boolean *lineError, Instruction inst) {

    int rs = 0, rt = 0;
    short immed = 0;/*//////*/
    char *labelRef = NULL;
    rs = getReg(ptr, errorList, lineNum);/*get first register*/
    if (rs == -1) { *lineError = TRUE;/*check if valid*/ }

    if (!*lineError && !matchComma(ptr, errorList, lineNum)) { *lineError = TRUE;/*check for comma between operands*/ }

    if ((op->opcode >= MIN_ARITH_OPCODE && op->opcode <= MAX_ARITH_OPCODE) ||
        (op->opcode >= MIN_MEM_OPCODE && op->opcode <= MAX_MEM_OPCODE)) {
        /*if op is I type or memory instruction it needs to get rs,immed,rt*/
        if (!*lineError) { immed = getImmed(ptr, errorList, lineNum, lineError);/*get immed value*/ }
        if (!*lineError && !matchComma(ptr, errorList, lineNum)) { *lineError = TRUE;/*check for ,*/ }

        if (!*lineError) {
            rt = getReg(ptr, errorList, lineNum);
            if (rt == -1) { *lineError = TRUE; }
        }/*get rt reg*/

    } else if (op->opcode >= MIN_BRANCH_OPCODE && op->opcode <= MAX_BRANCH_OPCODE) {
        /*if is a conditional branch it gets rs,rt,label*/

        if (!*lineError) {
            rt = getReg(ptr, errorList, lineNum);
            if (rt == -1) { *lineError = TRUE; }
        }/*get rt register*/

        if (!*lineError && !matchComma(ptr, errorList, lineNum)) { *lineError = TRUE;/*check for , */ }
        if (!*lineError) {
            labelRef = getLabel(ptr, errorList, lineNum, lineError);/*get label ref*/
        }
    }
    if (!*lineError) { checkExtraText(ptr, errorList, lineNum, lineError);/*check for extra text*/ }

    if (!*lineError) {/*save codenode */
        inst.i.rs = rs;
        inst.i.rt = rt;
        inst.i.immed = immed;
        addCodeNode(codeHead, inst, *IC, lineNum, labelRef);
        *IC += NUM_BYTES_WORD;
    }

    if (labelRef) { free(labelRef);/*free label*/ }
}

/*
 * procJType func
 * processes J type instruction and updates code list
 * the input is string pointer, opcode, line num, code head, IC, error list, lineError flag
 * returns void
 */
void procJType(char **ptr, Opcode *op, int lineNum, CodeNode **codeHead, int *IC, ErrorNode **errorList,boolean *lineError, Instruction inst) {

    int regBit = 0, addr = 0;
    char *labelRef = NULL;

    if (op->opcode != STOP_OPCODE) {/*if is a J type (jmp, la, call)*/

        char *t = getToken(ptr);/*get operand token*/
        if (t) {/*if operand exists*/
            if (t[0] == '$') {/*check if operand is a reg*/
                if (op->opcode == LA_OPCODE || op->opcode == CALL_OPCODE) {
                    addError(errorList, lineNum, ERR_INVALID_OPERAND_TYPE, "la/call takes a label");
                    *lineError = TRUE;
                }/*checks operand type*/

                else {
                    regBit = 1;
                    addr = getRegNum(t);
                    if (addr == -1) {
                        addError(errorList, lineNum, ERR_INVALID_REG, t);
                        *lineError = TRUE;
                    }
                }/*get reg*/
            } else {/*operand is label*/
                if (t[0] >= '0' && t[0] <= '9') {
                    addError(errorList, lineNum, ERR_INVALID_OPERAND_TYPE, "J-type takes label or register");
                    *lineError = TRUE;
                }/*check operand type*/

                else {
                    regBit = 0;
                    labelRef = strdupp(t);
                }/*save label reference*/
            }

            free(t);/*free token*/
        } else {/*if operand missing*/
            addError(errorList, lineNum, ERR_MISSING_OPERAND, NULL);
            *lineError = TRUE;
        }/*error missing operand*/
    }
    if (!*lineError) { checkExtraText(ptr, errorList, lineNum, lineError);/*check for extra text*/ }

    if (!*lineError) {/*save codenode*/
        inst.j.reg = regBit;
        inst.j.address = addr;
        addCodeNode(codeHead, inst, *IC, lineNum, labelRef);
        *IC += NUM_BYTES_WORD;
    }

    if (labelRef) { free(labelRef);/*free label*/ }
}

/*
 * procInstruction func
 * proc instruction and call right proc function
 * the input is the token, string pointer, label, line num, symbols, code head, IC, error list, lineError flag
 * return true if an error was found and false if not
 */
boolean procInstruction(char *token, char **ptr, char *label, int lineNum, SymbolNode **symbols, CodeNode **codeHead,int *IC, ErrorNode **errorList, boolean lineError) {

    Opcode *op = getOpcode(token);/*get instruction opcode*/
    Instruction inst;

    if (op) {/*if valid instruction*/
        inst.rawCode = 0;/*init raw code with 0*/
        inst.r.opcode = op->opcode;/*set opcode*/
        if (label && !lineError) { addSymbol(symbols, label, *IC, CODE);/*add label to symbol table if found*/ }

        /*call the needed proc func for the type*/
        if (op->type == R_TYPE) {
            procRType(ptr, op, lineNum, codeHead, IC, errorList, &lineError, inst);
        } else if (op->type == I_TYPE) {
            procIType(ptr, op, lineNum, codeHead, IC, errorList, &lineError, inst);
        } else if (op->type == J_TYPE) {
            procJType(ptr, op, lineNum, codeHead, IC, errorList, &lineError, inst);
        }

        free(op);/*free opcode*/

    } else {/*invalid instruction*/
        addError(errorList, lineNum, ERR_UNKNOWN_COMMAND, token);/*add error unknown command*/
        lineError = TRUE;/*set error flag*/
    }
    return lineError;/*return if errors found*/
}

/*
 * firstPass func
 * first pass of the assembler
 * the input is filename, symbols list, code list, data list, IC, DC, error list, macros
 * returns true if the pass was ssuccessful and false if not
 */
boolean firstPass(const char *filename, SymbolNode **symbols, CodeNode **codeHead, DataNode **dataHead, int *IC,int *DC, ErrorNode **errorList, MacroNode *macros) {

    char amName[MAX_LINE_LENGTH];
    FILE *fp;
    char line[MAX_LINE_LENGTH + EXTRA_CHARS];
    int lineNum = 0;
    boolean error = FALSE, lineError;
    char *ptr, *token, *label;
    SymbolNode *currSym;
    DataNode *currData;

    strcpy(amName, filename);
    remAsExtension(amName);/*remove .as if found*/
    strcat(amName, ".am");/*add .am*/
    fp = fopen(amName, "r");/*open file for reading*/
    if (!fp) {
        addError(errorList, 0, ERR_OPEN_FILE, amName);
        return FALSE;
    }/*return false if file open failed*/

    while (fgets(line, sizeof(line), fp)) {/*read line by line*/
        ptr = line;
        label = NULL;
        lineNum++;
        lineError = FALSE;

        if (!checkLineLen(line)) {/*check if line too long*/
            addError(errorList, lineNum, ERR_LINE_TOO_LONG, NULL);
            error = TRUE;

        } else if (!isEmptyLine(line) && !isCommentLine(line)) {/*ignore empty and comment lines*/
            token = getToken(&ptr);/*get first token*/
            checkLabelDef(&token, &label, &ptr, errorList, lineNum, *symbols, macros,&lineError);/*check if is label ref*/

            if (!token) {
                if (label) { free(label); }
                if (lineError) { error = TRUE; }
            } else {
                if (token[0] == '.') {/*if is directive*/
                    lineError = procDirective(token, &ptr, label, filename, lineNum, symbols, dataHead, DC, errorList,
                                              lineError);
                } else {/*if is instruction*/
                    lineError =
                        procInstruction(token, &ptr, label, lineNum, symbols, codeHead, IC, errorList, lineError);
                }

                /*free*/
                if (label) { free(label); }
                if (token) { free(token); }
                if (lineError) { error = TRUE; }
            }
        }
    }

    currSym = *symbols;
    while (currSym) {
        if (currSym->isData && !currSym->isExternal) { currSym->address += *IC; }
        currSym = currSym->next;
    }/*update data symbol addresses*/

    currData = *dataHead;
    while (currData) {
        currData->address += *IC;
        currData = currData->next;
    }/*update data node addresses*/

    fclose(fp);/*close file*/
    return !error;
}
