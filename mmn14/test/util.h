#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include "errors.h"

typedef struct {
    char *name;
    int opcode;
    int funct;
    InstType type;
} Opcode;

Opcode *getOpcode(const char *name);
boolean isReservedKeyword(const char *name);
boolean isValidLabelFormat(const char *name);

void *safeMalloc(size_t size);
void *safeRealloc(void *ptr, size_t size);
char *duplicateString(const char *str);

/* Using standard C string utils */
void skipSpaces(char **str);
boolean isEmptyLine(const char *str);
boolean isCommentLine(const char *str);
char *getToken(char **str);

int getRegNum(const char *token);

/* Clean parsing helpers */
boolean matchComma(char **ptr, ErrorNode **errorList, int lineNum);
int checkRegOperand(char **ptr, ErrorNode **errorList, int lineNum);
short checkImmedOperand(char **ptr, ErrorNode **errorList, int lineNum, boolean *lineError);
char *checkLabelOperand(char **ptr, ErrorNode **errorList, int lineNum, boolean *lineError);
void checkExtraText(char **ptr, ErrorNode **errorList, int lineNum, boolean *lineError);

boolean checkLineLen(const char *line);
boolean isLabelDef(const char *token);

#endif
