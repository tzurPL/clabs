/*
 * util.c
 * mmn14
 * Tzur Pinto Lazar
 */

#include "util.h"
#include <ctype.h>
#include <string.h>

Opcode opcodes[] = {{"add", 0, 1, R_TYPE},   {"sub", 0, 2, R_TYPE},   {"and", 0, 3, R_TYPE},   {"or", 0, 4, R_TYPE},
                    {"nor", 0, 5, R_TYPE},   {"move", 1, 1, R_TYPE},  {"mvhi", 1, 2, R_TYPE},  {"mvlo", 1, 3, R_TYPE},
                    {"addi", 10, 0, I_TYPE}, {"subi", 11, 0, I_TYPE}, {"andi", 12, 0, I_TYPE}, {"ori", 13, 0, I_TYPE},
                    {"nori", 14, 0, I_TYPE}, {"bne", 15, 0, I_TYPE},  {"beq", 16, 0, I_TYPE},  {"blt", 17, 0, I_TYPE},
                    {"bgt", 18, 0, I_TYPE},  {"lb", 19, 0, I_TYPE},   {"sb", 20, 0, I_TYPE},   {"lw", 21, 0, I_TYPE},
                    {"sw", 22, 0, I_TYPE},   {"lh", 23, 0, I_TYPE},   {"sh", 24, 0, I_TYPE},   {"jmp", 30, 0, J_TYPE},
                    {"la", 31, 0, J_TYPE},   {"call", 32, 0, J_TYPE}, {"stop", 63, 0, J_TYPE}, {"hlt", 63, 0, J_TYPE}};

/*
 * getOpcode func
 * retrieves the opcode details for a given instruction name.
 * the input is the name of the instruction.
 * returns a pointer to the Opcode struct if found, or NULL otherwise.
 */
Opcode *getOpcode(const char *name) {
    int i;
    for (i = 0; i < (int)(sizeof(opcodes) / sizeof(Opcode)); i++) { /*iterate through opcode table*/
        if (strcmp(opcodes[i].name, name) == 0) { return &opcodes[i]; /*return opcode if matches*/ }
    }
    return NULL; /*return null if not found*/
}

/*
 * isReservedKeyword func
 * checks if a given name is a reserved keyword in the assembler.
 * the input is the name to check.
 * returns TRUE if it is a reserved keyword, FALSE otherwise.
 */
boolean isReservedKeyword(const char *name) {
    if (getOpcode(name)) { return TRUE; /*if name is an opcode, it's reserved*/ }
    /*check against all directive and macro keywords*/
    if (strcmp(name, "db") == 0 || strcmp(name, "dh") == 0 || strcmp(name, "dw") == 0 || strcmp(name, "asciz") == 0 ||
        strcmp(name, "entry") == 0 || strcmp(name, "extern") == 0 || strcmp(name, "mcro") == 0 ||
        strcmp(name, "mcroend") == 0) {
        return TRUE;
    }
    return FALSE; /*not reserved*/
}

/*
 * checkLabelN func
 * checks if a given string follows the valid format for a label.
 * the input is the name of the label.
 * returns TRUE if the format is valid, FALSE otherwise.
 */
boolean checkLabelN(const char *name) {
    int i = 0;
    if (!name || name[0] == '\0') { return FALSE; /*check for empty string*/ }
    if (!isalpha((unsigned char)name[0])) { return FALSE; /*check if first character is alphabetic*/ }
    for (i = 1; name[i]; i++) { /*iterate through characters*/
        if (!isalnum((unsigned char)name[i]) && name[i] != '_') {
            return FALSE; /*check for alphanumeric or underscore*/
        }
    }
    return TRUE;
}

/*
 * safeMalloc func
 * allocates memory safely and exits the program if allocation fails.
 * the input is the size of memory to allocate.
 * returns a pointer to the allocated memory.
 */
void *safeMalloc(size_t size) {
    void *ptr = malloc(size); /*attempt allocation*/
    if (!ptr) {
        printError(NULL, 0, ERR_ALLOC_FAIL, NULL);
        exit(1);
    }           /*print error and exit if failed*/
    return ptr; /*return successful allocation*/
}

/*
 * safeRealloc func
 * reallocates memory safely and exits the program if reallocation fails.
 * the input is a pointer to the previously allocated memory and the new size.
 * returns a pointer to the newly allocated memory.
 */
void *safeRealloc(void *ptr, size_t size) {
    void *newPtr = realloc(ptr, size); /*attempt reallocation*/
    if (!newPtr && size > 0) {
        printError(NULL, 0, ERR_ALLOC_FAIL, NULL);
        exit(1);
    }              /*print error and exit if failed*/
    return newPtr; /*return successful reallocation*/
}

/*
 * strdupp func
 * duplicates a string safely by allocating memory and copying the content.
 * the input is the string to duplicate.
 * returns a pointer to the newly allocated duplicated string.
 */
char *strdupp(const char *s) {
    char *d;
    if (!s) { return NULL; /*check for null input*/ }
    d = (char *)safeMalloc(strlen(s) + 1); /*allocate memory for string + null terminator*/
    strcpy(d, s);                          /*copy string content*/
    return d;                              /*return duplicated string*/
}

/*
 * skipSpaces func
 * advances a string pointer past any leading whitespace characters.
 * the input is a pointer to the string pointer.
 * returns void.
 */
void skipSpaces(char **str) {
    while (**str != '\0' && isspace((unsigned char)**str)) { /*advance past whitespace*/
        (*str)++;
    }
}

/*
 * isEmptyLine func
 * checks if a line consists entirely of whitespace characters.
 * the input is the string to check.
 * returns TRUE if the line is empty or only whitespace, FALSE otherwise.
 */
boolean isEmptyLine(const char *str) {
    while (*str) { /*iterate through characters*/
        if (!isspace((unsigned char)*str)) { return FALSE; /*if non-space found, not empty*/ }
        str++; /*move to next character*/
    }
    return TRUE; /*only spaces found*/
}

/*
 * isCommentLine func
 * checks if a line is a comment, indicated by a leading semicolon.
 * the input is the string to check.
 * returns TRUE if the line is a comment, FALSE otherwise.
 */
boolean isCommentLine(const char *str) {
    const char *p = str;
    while (*p && isspace((unsigned char)*p)) {
        p++; /*skip leading spaces*/
    }
    return (*p == ';'); /*check if first non-space character is semicolon*/
}

/*
 * getToken func
 * extracts the next token from a string, separated by whitespace or commas.
 * the input is a pointer to the string pointer.
 * returns a newly allocated string containing the token, or NULL if no token is found.
 */
char *getToken(char **str) {
    char *start, *token;
    int len = 0;
    skipSpaces(str); /*skip leading spaces*/
    if (**str == '\0' || **str == ';') { return NULL; /*return null if end of string or comment*/ }

    start = *str;
    while (**str && !isspace((unsigned char)**str) && **str != ',' && **str != ';') { /*count token length*/
        (*str)++;
        len++;
    }

    if (len == 0) { return NULL; /*return null if empty*/ }
    token = (char *)safeMalloc(len + 1); /*allocate memory for token*/
    strncpy(token, start, len);          /*copy token characters*/
    token[len] = '\0';                   /*null-terminate string*/
    return token;
}

/*
 * getRegNum func
 * extracts the register number from a register token string.
 * the input is the register token string (e.g. "$4").
 * returns the register number if valid, or -1 otherwise.
 */
int getRegNum(const char *token) {
    if (token && token[0] == '$') { /*check for register prefix*/
        int num;
        char *endptr;
        const char *p = token + 1;
        if (!*p) { return -1; }
        num = (int)strtol(p, &endptr, 10); /*convert remaining string to integer*/
        if (*endptr != '\0') { return -1; /*check for invalid characters*/ }
        if (num >= 0 && num < REG_COUNT) { return num; /*check if register is within bounds*/ }
    }
    return -1; /*return -1 if invalid*/
}

/* Bulletproof parsing helpers */
/*
 * matchComma func
 * checks for and consumes a comma in the string, reporting an error if missing.
 * the input is a pointer to the string pointer, error list, and line number.
 * returns TRUE if a comma was found, FALSE otherwise.
 */
boolean matchComma(char **ptr, ErrorNode **errorList, int lineNum) {
    skipSpaces(ptr);    /*skip leading spaces*/
    if (**ptr == ',') { /*check for comma*/
        (*ptr)++;       /*move past comma*/
        return TRUE;    /*comma found*/
    }
    addError(errorList, lineNum, ERR_MISSING_COMMA, NULL); /*report missing comma error*/
    return FALSE;                                          /*comma not found*/
}

/*
 * checkRegOperand func
 * extracts a register operand from the string and validates it.
 * the input is a pointer to the string pointer, error list, and line number.
 * returns the valid register number, or -1 if invalid or missing.
 */
int checkRegOperand(char **ptr, ErrorNode **errorList, int lineNum) {
    int reg;
    char *t = getToken(ptr); /*get the next token*/
    if (!t) {
        addError(errorList, lineNum, ERR_EXTRA_TEXT, "missing operand");
        return -1;
    }                   /*report missing operand*/
    reg = getRegNum(t); /*extract register number*/
    if (reg == -1) { addError(errorList, lineNum, ERR_INVALID_REG, t); /*report invalid register*/ }
    free(t);    /*free the token*/
    return reg; /*return register number*/
}

/*
 * checkImmedOperand func
 * extracts an immediate value operand from the string.
 * the input is a pointer to the string pointer, error list, line number, and error flag.
 * returns the immediate value, or 0 if missing (sets error flag).
 */
short checkImmedOperand(char **ptr, ErrorNode **errorList, int lineNum, boolean *lineError) {
    short val;
    char *t = getToken(ptr); /*get the next token*/
    if (!t) {
        addError(errorList, lineNum, ERR_EXTRA_TEXT, "missing operand");
        *lineError = TRUE;
        return 0;
    }                     /*report missing operand*/
    val = (short)atoi(t); /*convert token to short integer*/
    free(t);              /*free the token*/
    return val;           /*return immediate value*/
}

/*
 * checkLabelOperand func
 * extracts a label operand from the string.
 * the input is a pointer to the string pointer, error list, line number, and error flag.
 * returns a newly allocated string containing the label, or NULL if missing (sets error flag).
 */
char *checkLabelOperand(char **ptr, ErrorNode **errorList, int lineNum, boolean *lineError) {
    char *t = getToken(ptr); /*get the next token*/
    if (!t) {
        addError(errorList, lineNum, ERR_EXTRA_TEXT, "missing operand");
        *lineError = TRUE;
        return NULL;
    }         /*report missing operand*/
    return t; /*return label token*/
}

/*
 * checkExtraText func
 * checks if there is any extraneous text remaining in the string and reports an error if so.
 * the input is a pointer to the string pointer, error list, line number, and error flag.
 * returns void.
 */
void checkExtraText(char **ptr, ErrorNode **errorList, int lineNum, boolean *lineError) {
    skipSpaces(ptr); /*skip leading spaces*/
    if (**ptr != '\0' && **ptr != ';' && **ptr != '\n' &&
        **ptr != '\r') {                                    /*check if anything besides comments or newlines remain*/
        addError(errorList, lineNum, ERR_EXTRA_TEXT, *ptr); /*report extra text error*/
        *lineError = TRUE;                                  /*set error flag*/
    }
}

/*
 * checkLineLen func
 * checks if the length of a line is within the allowed limit (max 80 chars).
 * the input is the string line to check.
 * returns TRUE if the length is valid, FALSE otherwise.
 */
boolean checkLineLen(const char *line) {
    int len = strlen(line); /*get initial length*/
    if (len > 0 && line[len - 1] == '\n') { len--; /*ignore trailing newline*/ }
    if (len > 0 && line[len - 1] == '\r') { len--; /*ignore trailing carriage return*/ }
    return len <= MAX_AS_LINE_LEN; /*check against maximum allowed length*/
}

/*
 * isLabelDef func
 * checks if a token string ends with a colon, indicating it is a label definition.
 * the input is the token string to check.
 * returns TRUE if it is a label definition, FALSE otherwise.
 */
boolean isLabelDef(const char *token) {
    if (token && token[0] != '\0' && token[strlen(token) - 1] == ':') { /*check if token ends with colon*/
        return TRUE;                                                    /*is label definition*/
    }
    return FALSE; /*not a label definition*/
}
