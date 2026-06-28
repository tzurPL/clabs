#include "preproc.h"
#include "util.h"
#include "table.h"
#include "errors.h"
#include <string.h>



/*
 * checks if the first token is a label.
 * if so, saves it, sets the hasLabel flag, and fetches the next token.
 * the input is a pointer to the token string pointer, a pointer to store the label, a pointer to the input string pointer, and a boolean flag.
 * returns 1 if a label was found, 0 otherwise.
 */
int checkLabel(char **token, char **label, char **ptr, boolean *hasLabel) {
    if (isLabelDef(*token)) {
        *hasLabel = TRUE;
        *label = *token;
        *token = getToken(ptr);
        return 1;
    }
    return 0;
}

/*
 * checks if the token indicates the start of a macro definition ("mcro").
 * validates and extracts the macro name if it is a macro definition.
 * the input is the token, a pointer to the input string pointer, a code to store the macro name, the filename, and the line number.
 * returns 1 if a valid macro definition was found, 0 if not a macro definition, and -1 if there was an error.
 */
int checkMacroDef(const char *token, char **ptr, char *macroName, const char *filename, int lineNum) {
    char *exp;
    if (strcmp(token, "mcro") == 0) {
        exp = getToken(ptr);
        if (exp) {
            if (isReservedKeyword(exp)) {
                printError(filename, lineNum, ERR_RESERVED_KEYWORD, exp);
                free(exp);
                return -1;
            }
            if (!isValidLabelFormat(exp)) {
                printError(filename, lineNum, ERR_INVALID_LABEL_FORMAT, exp);
                free(exp);
                return -1;
            }
            strcpy(macroName, exp);
            free(exp);
            return 1;
        }
        printError(filename, lineNum, ERR_EXTRA_TEXT, "Missing macro name");
        return -1;
    }
    return 0;
}

/* Helper function to append to our output code */
void appendToOutput(char **code, size_t *size, const char *str) {
    size_t len = strlen(str);
    *code = (char *)safeRealloc(*code, *size + len);
    strcat(*code, str);
    *size += len;
}

/*
 * processes a macro definition start.
 * updates the macro state and reports errors if redefinition occurs.
 * the input is macro definition status, inMacro flag, error flag, filename, line number, macros, macro name, and macro content pointer.
 * returns void.
 */
void procMacroDef(int macroDefStatus, boolean *inMacro, boolean *error, const char *filename, int lineNum, MacroNode *macros, char *macroName, char **macroContent) {
    *inMacro = TRUE;
    if (macroDefStatus == -1) *error = TRUE;
    else {
        if (getMacroContent(macros, macroName)) {
            printError(filename, lineNum, ERR_MACRO_REDEFINITION, macroName);
            *error = TRUE;
        }
    }
    *macroContent = (char *)safeMalloc(1);
    (*macroContent)[0] = '\0';
}

/*
 * writes the output to the .am file if there were no errors.
 */
void writeAm(boolean *error, const char *filename, const char *amName, const char *output) {
    FILE *amF;
    if (!*error) {
        amF = fopen(amName, "w");
        if (!amF) {
            printError(filename, 0, ERR_OPEN_FILE, amName);
            *error = TRUE;
        } else {
            fprintf(amF, "%s", output);
            fclose(amF);
        }
    }
}

/*
 * processes a regular line or a macro expansion.
 * appends to macro content if in a macro, otherwise expands or appends the original line.
 * the input is inMacro flag, macros, token, line, macro content pointer, label, hasLabel flag, output string pointer, and output size.
 * returns void.
 */
void procLine(boolean inMacro, MacroNode *macros, char *token, char *line, char **macroContent, char *label, boolean hasLabel, char **output, size_t *outSize) {
    char *exp;
    if (inMacro) {
        /* Append the original line to the macro content */
        *macroContent = (char *)safeRealloc(*macroContent, strlen(*macroContent) + strlen(line) + 1);
        strcat(*macroContent, line);
    } else {
        exp = getMacroContent(macros, token);
        if (exp) {
            if (hasLabel) {
                /* Keep the label, but inject the macro expansion */
                char temp[MAX_LINE_LENGTH + 2];
                sprintf(temp, "%s %s", label, exp);
                appendToOutput(output, outSize, temp);
            } else {
                appendToOutput(output, outSize, exp);
            }
        } else {
            appendToOutput(output, outSize, line); /* Print original line */
        }
    }
}

/* preprocess func: runs the preprocessor phase on the input file, expanding macros and removing comments/empty lines. Returns boolean for success/fail and output macros. */
boolean preprocess(const char *filename, MacroNode **outMacros) {
    char asName[MAX_LINE_LENGTH], amName[MAX_LINE_LENGTH];/* */
    FILE *asF;
    char line[MAX_LINE_LENGTH + 2];
    MacroNode *macros = NULL;
    boolean inMacro = FALSE, error = FALSE;
    char macroName[MAX_LABEL_LENGTH];
    char *macroContent = NULL;
    char *token, *label;
    int lineNum = 0, macroDefStatus;

    char *output = (char *)safeMalloc(1);
    size_t outSize = 1;
    output[0] = '\0';

    strcpy(asName, filename); strcat(asName, ".as");
    strcpy(amName, filename); strcat(amName, ".am");

    asF = fopen(asName, "r");
    if (!asF) {
        printError(filename, 0, ERR_OPEN_FILE, asName);
        free(output);
        return FALSE;
    }

    /*go through the file line by linee */
    while (fgets(line, sizeof(line), asF)) {
        char *ptr = line;
        boolean hasLabel = FALSE;

        lineNum++;

        if (!checkLineLen(line)) {
            printError(filename, lineNum, ERR_LINE_TOO_LONG, NULL);
            error = TRUE;
        } else if (isEmptyLine(line) || isCommentLine(line)) {
            if (!inMacro) appendToOutput(&output, &outSize, line);
        } else {
            token = getToken(&ptr);
            if (!token) {
                if (!inMacro) appendToOutput(&output, &outSize, line);
            } else {
                label = NULL;
                /* If the first token is a label, save it and check the NEXT token */
                checkLabel(&token, &label, &ptr, &hasLabel);

                if (!token) {
                    if (!inMacro) appendToOutput(&output, &outSize, line);
                    if (hasLabel) free(label);
                } else {
                    macroDefStatus = checkMacroDef(token, &ptr, macroName, filename, lineNum);
                    if (macroDefStatus != 0) {
                        procMacroDef(macroDefStatus, &inMacro, &error, filename, lineNum, macros, macroName, &macroContent);
                    } else if (strcmp(token, "mcroend") == 0) {
                        addMacro(&macros, macroName, macroContent);
                        free(macroContent);
                        inMacro = FALSE;
                    } else {
                        procLine(inMacro, macros, token, line, &macroContent, label, hasLabel, &output, &outSize);
                    }

                    if (token) free(token);
                    if (hasLabel && label) free(label);
                }
            }
        }
    }

    fclose(asF);

    writeAm(&error, filename, amName, output);

    free(output);

    if (outMacros) *outMacros = macros;
    else freeMacros(macros);
    return !error;
}
