/*
 * util.c
 * mmn14
 * Tzur Pinto Lazar
 */

#include "util.h"
#include <ctype.h>
#include <string.h>



/*
 * getOpcode func
 * gets the opcode details for a given instruction.
 * the input is the name of the instruction.
 * returns a pointer to the Opcode struct if found or NULL if not.
 */
Opcode *getOpcode(const char *name) {
    Opcode opcodes[] = {
        {"add", 0, 1, R_TYPE},{"sub", 0, 2, R_TYPE},{"and", 0, 3, R_TYPE},{"or", 0, 4, R_TYPE},
        {"nor", 0, 5, R_TYPE},{"move", 1, 1, R_TYPE},{"mvhi", 1, 2, R_TYPE},{"mvlo", 1, 3, R_TYPE},
        {"addi", 10, 0, I_TYPE},{"subi", 11, 0, I_TYPE},{"andi", 12, 0, I_TYPE},{"ori", 13, 0, I_TYPE},
        {"nori", 14, 0, I_TYPE},{"bne", 15, 0, I_TYPE},{"beq", 16, 0, I_TYPE},{"blt", 17, 0, I_TYPE},
        {"bgt", 18, 0, I_TYPE},{"lb", 19, 0, I_TYPE},{"sb", 20, 0, I_TYPE},{"lw", 21, 0, I_TYPE},
        {"sw", 22, 0, I_TYPE},{"lh", 23, 0, I_TYPE},{"sh", 24, 0, I_TYPE},{"jmp", 30, 0, J_TYPE},
        {"la", 31, 0, J_TYPE},{"call", 32, 0, J_TYPE},{"stop", 63, 0, J_TYPE},{"hlt", 63, 0, J_TYPE}};
    int i;

    for (i = 0; i < (int)(sizeof(opcodes) / sizeof(Opcode)); i++) {/*go over the opcode table*/
        if (strcmp(opcodes[i].name, name) == 0) {/*if the name in the table is the needed name*/
            Opcode *res = (Opcode *)safeMalloc(sizeof(Opcode));
            *res = opcodes[i];
            return res;
        }
    }
    return NULL;
}

/*
 * isReservedKeyword func
 * checks if a name is a reserved keyword
 * the input is the name to check.
 * returns true if it is a reserved keyword false if not.
 */
boolean isReservedKeyword(const char *name) {
    Opcode *op = getOpcode(name);/*get the opcode*/
    if (op) {/*if found*/
        free(op);
        return TRUE;/*if name is an opcode it's reserved*/
    }
    /*check all directive and macro reserved words*/
    if (strcmp(name, "db") == 0 || strcmp(name, "dh") == 0 || strcmp(name, "dw") == 0 || strcmp(name, "asciz") == 0 ||
        strcmp(name, "entry") == 0 || strcmp(name, "extern") == 0 || strcmp(name, "mcro") == 0 ||
        strcmp(name, "mcroend") == 0) {
        return TRUE;
    }
    return FALSE;/*not reserved*/
}

/*
 * checkLabelN func
 * checks if a name follows the valid format for label
 * the input is the name of the label
 * returns true if the format is valid false if not.
 */
boolean checkLabelN(const char *name) {
    int i = 0;
    if (!name || name[0] == '\0') { return FALSE;/*check for empty string*/ }
    if (!isalpha((unsigned char)name[0])) { return FALSE;/*check if first character is in the alphabet*/ }

    for (i = 1; name[i]; i++) {/*go through characters*/
        if (!isalnum((unsigned char)name[i]) && name[i] != '_') {
            return FALSE;/*check for number or letter or _*/
        }
    }
    return TRUE;
}

/*
 * safeMalloc func
 * allocates memory in a good way and exits the program if failed.
 * the input is the size of memory to allocate
 * returns a pointer to the allocated memory
 */
void *safeMalloc(size_t size) {
    void *ptr = malloc(size);/*try to allocate*/
    if (!ptr) {/*if failed*/
        printError(NULL, 0, ERR_ALLOC_FAIL, NULL);
        exit(1);
    }/*print error and exit if failed*/
    return ptr;/*return allocated*/
}

/*
 * safeRealloc func
 * reallocates memory and exits the program if failed.
 * the input is a pointer to the old allocated memory and the new size
 * returns a pointer to the new allocated space
 */
void *safeRealloc(void *ptr, size_t size) {
    void *newPtr = realloc(ptr, size);/*try to realloc*/
    if (!newPtr && size > 0) {
        printError(NULL, 0, ERR_ALLOC_FAIL, NULL);
        exit(1);
    }/*print error and exit if failed*/
    return newPtr;/*return the new pointer with allocated memory*/
}

/*
 * strdupp func
 * duplicates a string by allocating memory and copying it.
 * the input is the string
 * returns a pointer to the new duplicated string
 */
char *strdupp(const char *s) {
    char *d;
    if (!s){return NULL;}/*check for null input*/
    d = (char *)safeMalloc(strlen(s) + 1);/*allocate memory for string and its \0*/
    strcpy(d, s);/*copy string*/
    return d;/*return duplicated string*/
}

/*
 * skipSpaces func
 * move a pointer over a string and skip spaces.
 * the input is a pointer to the string pointer.
 * returns void.
 */
void skipSpaces(char **str) {
    while (**str != '\0' && isspace((unsigned char)**str)) {/*skip whitespace*/
        (*str)++;
    }
}

/*
 * isEmptyLine func
 * checks if a line has only whitespaces.
 * the input is a string pointer
 * returns true if the line is empty false if not
 */
boolean isEmptyLine(const char *str) {
    while (*str) {/*go through characters*/
        if (!isspace((unsigned char)*str)) { return FALSE;/*if non-space found, not empty*/ }
        str++;/*move to next character*/
    }
    return TRUE;/*only spaces found*/
}

/*
 * isCommentLine func
 * checks if a line is a comment by checking if it starts with ;
 * the input is a string pointer
 * returns true if the line is a comment flase if not
 */
boolean isCommentLine(const char *str) {
    const char *p = str;
    while (*p && isspace((unsigned char)*p)) {p++;}/*skip spaces*/

    return (*p == ';');/*check if first non space character is ;*/
}

/*
 * getToken func
 * extracts the next token from a string separated by whitespace or ,
 * the input is a pointer to the string pointer
 * returns a new string with the token or null if it didnt find a token
 */
char *getToken(char **str) {
    char *start, *token;
    int len = 0;

    skipSpaces(str);/*skip spaces*/
    /*return null if end of string or comment*/
    if (**str == '\0' || **str == ';') { return NULL; }

    start = *str;/*start of the token*/
    while (**str && !isspace((unsigned char)**str) && **str != ',' && **str != ';') {/*count token len*/
        (*str)++;
        len++;
    }

    if (len == 0){ return NULL; }/*return null if empty*/
    token = (char *)safeMalloc(len + 1);/*allocate memory token*/
    strncpy(token, start, len);/*copy chars to token*/
    token[len] = '\0';/*add end*/
    return token;
}

/*
 * getRegNum func
 * extracts the register num from a string
 * the input is the register token string
 * returns the register number if valid or -1 if not
 */
int getRegNum(const char *token) {
    if (token && token[0] == '$') {/*check for register mark*/
        int num;
        char *endptr;
        const char *p = token + 1;

        if (!*p){return -1;}/*if not exist */
        num = (int)strtol(p,&endptr,10);/*convert string to int*/
        if (*endptr != '\0') { return -1;}/*check if there invalid chars*/
        /*check if register is valid num for register*/
        if (num >= 0 && num < REG_COUNT){return num;}
    }
    return -1;/*return -1 if not good*/
}

/*
 * matchComma func
 * checks for comma in string and gives out error if missing.
 * the input is a pointer to the string pointer, error list, and line number
 * returns true if comma found false if not
 */
boolean matchComma(char **ptr, ErrorNode **errorList, int lineNum) {
    skipSpaces(ptr);/*skip spaces*/
    if (**ptr == ',') {/*check for comma*/
        (*ptr)++;/*move after comma*/
        return TRUE;/*comma found*/
    }
    addError(errorList, lineNum, ERR_MISSING_COMMA, NULL);/*save missing comma error*/
    return FALSE;/*comma not found*/
}

/*
 * checkRegOperand func
 * extracts a register op from the string and checks it
 * the input is a pointer to the string pointer, error list, and line num
 * returns the valid register num or -1 if not valid or missing.
 */
int checkRegOperand(char **ptr, ErrorNode **errorList, int lineNum) {
    int reg;
    char *t = getToken(ptr);/*get the next token*/
    if (!t) {
        addError(errorList, lineNum, ERR_MISSING_OPERAND, NULL);
        return -1;
    }/*report missing operand*/
    reg = getRegNum(t);/*extract register number*/
    if (reg == -1) { addError(errorList, lineNum, ERR_INVALID_REG, t);/*report invalid register*/ }
    free(t);/*free the token*/
    return reg;/*return register number*/
}

/*
 * checkImmedOperand func
 * extracts an immed value from the string
 * the input is a pointer to the string pointer, error list, line number, and error flag.
 * returns the immediate value, or 0 if missing (sets error flag).
 */
short checkImmedOperand(char **ptr, ErrorNode **errorList, int lineNum, boolean *lineError) {
    long val;
    char *t = getToken(ptr);/*get the next token*/
    if (!t) {
        addError(errorList, lineNum, ERR_MISSING_OPERAND, NULL);
        *lineError = TRUE;
        return 0;
    }/*report missing operand*/
    val = atol(t);/*convert token to long integer*/
    if (val < MIN_IMMED || val > MAX_IMMED) {/*check bounds*/
        addError(errorList, lineNum, ERR_INVALID_IMMED, t);/*report out of bounds*/
        *lineError = TRUE;/*set error flag*/
    }
    free(t);/*free the token*/
    return (short)val;/*return immediate value*/
}

/*
 * checkLabelOperand func
 * extracts a label operand from the string.
 * the input is a pointer to the string pointer, error list, line number, and error flag.
 * returns a newly allocated string containing the label, or NULL if missing (sets error flag).
 */
char *checkLabelOperand(char **ptr, ErrorNode **errorList, int lineNum, boolean *lineError) {
    char *t = getToken(ptr);/*get the next token*/
    if (!t) {
        addError(errorList, lineNum, ERR_MISSING_OPERAND, NULL);
        *lineError = TRUE;
        return NULL;
    }/*report missing operand*/
    return t;/*return label token*/
}

/*
 * checkExtraText func
 * checks if there is any extra text remaining in the string and reports an error if so.
 * the input is a pointer to the string pointer, error list, line number, and error flag.
 * returns void.
 */
void checkExtraText(char **ptr, ErrorNode **errorList, int lineNum, boolean *lineError) {
    skipSpaces(ptr);/*skip leading spaces*/
    if (**ptr != '\0' && **ptr != ';' && **ptr != '\n' &&
        **ptr != '\r') {/*check if anything besides comments or newlines remain*/
        addError(errorList, lineNum, ERR_EXTRA_TEXT, *ptr);/*report extra text error*/
        *lineError = TRUE;/*set error flag*/
    }
}

/*
 * checkLineLen func
 * checks if the length of a line is within the allowed limit (max 80 chars).
 * the input is the string line to check.
 * returns TRUE if the length is valid, FALSE if not.
 */
boolean checkLineLen(const char *line) {
    int len = strlen(line);/*get initial length*/
    if (len > 0 && line[len - 1] == '\n') { len--;/*ignore trailing newline*/ }
    if (len > 0 && line[len - 1] == '\r') { len--;/*ignore trailing carriage return*/ }
    return len <= MAX_AS_LINE_LEN;/*check against maximum allowed length*/
}

/*
 * isLabelDef func
 * checks if a token string ends with a colon, indicating it is a label definition.
 * the input is the token string to check.
 * returns TRUE if it is a label definition, FALSE if not.
 */
boolean isLabelDef(const char *token) {
    if (token && token[0] != '\0' && token[strlen(token) - 1] == ':') {/*check if token ends with colon*/
        return TRUE;/*is label definition*/
    }
    return FALSE;/*not a label definition*/
}

/*
 * stripAsExtension func
 * strips the .as extension from a filename if it exists.
 * the input is the string to strip the extension from.
 * returns void.
 */
void stripAsExtension(char *filename) {
    char *dot;
    if (!filename) return;
    dot = strrchr(filename, '.');
    if (dot && strcmp(dot, ".as") == 0) {
        *dot = '\0';/*strip .as extension if present*/
    }
}
