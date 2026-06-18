#include "util.h"
#include <string.h>
#include <ctype.h>

static Opcode opcodes[] = {
    {"add", 0, 1, R_TYPE}, {"sub", 0, 2, R_TYPE}, {"and", 0, 3, R_TYPE}, {"or", 0, 4, R_TYPE}, {"nor", 0, 5, R_TYPE},
    {"move", 1, 1, R_TYPE}, {"mvhi", 1, 2, R_TYPE}, {"mvlo", 1, 3, R_TYPE},
    {"addi", 10, 0, I_TYPE}, {"subi", 11, 0, I_TYPE}, {"andi", 12, 0, I_TYPE}, {"ori", 13, 0, I_TYPE}, {"nori", 14, 0, I_TYPE},
    {"bne", 15, 0, I_TYPE}, {"beq", 16, 0, I_TYPE}, {"blt", 17, 0, I_TYPE}, {"bgt", 18, 0, I_TYPE},
    {"lb", 19, 0, I_TYPE}, {"sb", 20, 0, I_TYPE}, {"lw", 21, 0, I_TYPE}, {"sw", 22, 0, I_TYPE}, {"lh", 23, 0, I_TYPE}, {"sh", 24, 0, I_TYPE},
    {"jmp", 30, 0, J_TYPE}, {"la", 31, 0, J_TYPE}, {"call", 32, 0, J_TYPE}, {"stop", 63, 0, J_TYPE}, {"hlt", 63, 0, J_TYPE}
};

Opcode *getOpcode(const char *name) {
    int i;
    for (i = 0; i < (int)(sizeof(opcodes)/sizeof(Opcode)); i++) {
        if (strcmp(opcodes[i].name, name) == 0) return &opcodes[i];
    }
    return NULL;
}

boolean isReservedKeyword(const char *name) {
    if (getOpcode(name)) return TRUE;
    if (strcmp(name, "db") == 0 || strcmp(name, "dh") == 0 || strcmp(name, "dw") == 0 ||
        strcmp(name, "asciz") == 0 || strcmp(name, "entry") == 0 || strcmp(name, "extern") == 0 ||
        strcmp(name, "mcro") == 0 || strcmp(name, "mcroend") == 0) return TRUE;
    return FALSE;
}

void *safeMalloc(size_t size) {
    void *ptr = malloc(size);
    if (!ptr) { printError(NULL, 0, ERR_ALLOC_FAIL, NULL); exit(1); }
    return ptr;
}

void *safeRealloc(void *ptr, size_t size) {
    void *newPtr = realloc(ptr, size);
    if (!newPtr && size > 0) { printError(NULL, 0, ERR_ALLOC_FAIL, NULL); exit(1); }
    return newPtr;
}

char *duplicateString(const char *str) {
    char *dup = (char *)safeMalloc(strlen(str) + 1);
    strcpy(dup, str);
    return dup;
}

void skipSpaces(char **str) {
    while (**str != '\0' && isspace((unsigned char)**str)) {
        (*str)++;
    }
}

boolean isEmptyLine(const char *str) {
    while (*str) {
        if (!isspace((unsigned char)*str)) return FALSE;
        str++;
    }
    return TRUE;
}

boolean isCommentLine(const char *str) {
    const char *p = str;
    while (*p && isspace((unsigned char)*p)) p++;
    return (*p == ';');
}

char *getToken(char **str) {
    char *start, *token;
    int len = 0;
    skipSpaces(str);
    if (**str == '\0' || **str == ';') return NULL;

    start = *str;
    while (**str && !isspace((unsigned char)**str) && **str != ',' && **str != ';') {
        (*str)++;
        len++;
    }

    if (len == 0) return NULL;
    token = (char *)safeMalloc(len + 1);
    strncpy(token, start, len);
    token[len] = '\0';
    return token;
}

int getRegNum(const char *token) {
    if (token && token[0] == '$') {
        int num;
        char *endptr;
        const char *p = token + 1;
        if (!*p) return -1;
        num = (int)strtol(p, &endptr, 10);
        if (*endptr != '\0') return -1;
        if (num >= 0 && num < REG_COUNT) return num;
    }
    return -1;
}

/* Bulletproof parsing helpers */
boolean matchComma(char **ptr, ErrorNode **errorList, int lineNum) {
    skipSpaces(ptr);
    if (**ptr == ',') {
        (*ptr)++;
        return TRUE;
    }
    addError(errorList, lineNum, ERR_MISSING_COMMA, NULL);
    return FALSE;
}

int parseRegOperand(char **ptr, ErrorNode **errorList, int lineNum) {
    int reg;
    char *t = getToken(ptr);
    if (!t) { addError(errorList, lineNum, ERR_EXTRA_TEXT, "missing operand"); return -1; }
    reg = getRegNum(t);
    if (reg == -1) addError(errorList, lineNum, ERR_INVALID_REG, t);
    free(t);
    return reg;
}

short parseImmedOperand(char **ptr, ErrorNode **errorList, int lineNum, boolean *lineError) {
    short val;
    char *t = getToken(ptr);
    if (!t) { addError(errorList, lineNum, ERR_EXTRA_TEXT, "missing operand"); *lineError = TRUE; return 0; }
    val = (short)atoi(t);
    free(t);
    return val;
}

char *parseLabelOperand(char **ptr, ErrorNode **errorList, int lineNum, boolean *lineError) {
    char *t = getToken(ptr);
    if (!t) { addError(errorList, lineNum, ERR_EXTRA_TEXT, "missing operand"); *lineError = TRUE; return NULL; }
    return t;
}

void checkExtraText(char **ptr, ErrorNode **errorList, int lineNum, boolean *lineError) {
    skipSpaces(ptr);
    if (**ptr != '\0' && **ptr != ';' && **ptr != '\n' && **ptr != '\r') {
        addError(errorList, lineNum, ERR_EXTRA_TEXT, *ptr);
        *lineError = TRUE;
    }
}
