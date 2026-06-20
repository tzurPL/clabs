#include "preproc.h"
#include "util.h"
#include "table.h"
#include "errors.h"
#include <string.h>

/*
 * checks the length of the given line to ensure it doesn't exceed the maximum allowed length.
 * exits with an error message if the line is too long.
 * the input is the line string, filename for error reporting, and the line number.
 * returns 1 if the line length is valid, 0 otherwise.
 */
int checkLineLen(const char *line, const char *filename, int lineNum) {
    int len = strlen(line);
    if (len > 0 && line[len-1] == '\n') len--;
    if (len > 0 && line[len-1] == '\r') len--;
    if (len > 80) {
        printError(filename, lineNum, ERR_LINE_TOO_LONG, NULL);
        return 0;
    }
    return 1;
}

/*
 * checks if the current line is an empty line or a comment line.
 * advances the pointer past any leading spaces.
 * the input is a pointer to the input string pointer.
 * returns 1 if the line is empty or a comment, 0 otherwise.
 */
int checkEmptyOrComment(char **ptr) {
    skipSpaces(ptr);
    if (**ptr == '\0' || **ptr == ';') {
        return 1;
    }
    return 0;
}

/*
 * checks if the first token is a label.
 * if so, saves it, sets the hasLabel flag, and fetches the next token.
 * the input is a pointer to the token string pointer, a pointer to store the label, a pointer to the input string pointer, and a boolean flag.
 * returns 1 if a label was found, 0 otherwise.
 */
int checkLabel(char **token, char **label, char **ptr, boolean *hasLabel) {
    if (*token && (*token)[strlen(*token)-1] == ':') {
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
static void appendToOutput(char **code, size_t *size, const char *str) {
    size_t len = strlen(str);
    *code = (char *)safeRealloc(*code, *size + len);
    strcat(*code, str);
    *size += len;
}

/* preprocess func: runs the preprocessor phase on the input file, expanding macros and removing comments/empty lines. Returns boolean for success/fail and output macros. */
boolean preprocess(const char *filename, MacroNode **outMacros) {
    char asName[MAX_LINE_LENGTH], amName[MAX_LINE_LENGTH];
    FILE *asF, *amF;
    char line[MAX_LINE_LENGTH + 2];
    MacroNode *macros = NULL;
    boolean inMacro = FALSE, error = FALSE;
    char macroName[MAX_LABEL_LENGTH];
    char *macroContent = NULL;
    char *token, *exp, *label;
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

    while (fgets(line, sizeof(line), asF)) {
        char *ptr = line;
        boolean hasLabel = FALSE;

        lineNum++;

        if (!checkLineLen(line, filename, lineNum)) {
            error = TRUE;
        } else if (checkEmptyOrComment(&ptr)) {
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
                        inMacro = TRUE;
                        if (macroDefStatus == -1) error = TRUE;
                        else {
                            if (getMacroContent(macros, macroName)) {
                                printError(filename, lineNum, ERR_MACRO_REDEFINITION, macroName);
                                error = TRUE;
                            }
                        }
                        macroContent = (char *)safeMalloc(1);
                        macroContent[0] = '\0';
                    } else if (strcmp(token, "mcroend") == 0) {
                        addMacro(&macros, macroName, macroContent);
                        free(macroContent);
                        inMacro = FALSE;
                    } else {
                        if (inMacro) {
                            /* Append the original line to the macro content */
                            macroContent = (char *)safeRealloc(macroContent, strlen(macroContent) + strlen(line) + 1);
                            strcat(macroContent, line);
                        } else {
                            exp = getMacroContent(macros, token);
                            if (exp) {
                                if (hasLabel) {
                                    /* Keep the label, but inject the macro expansion */
                                    char temp[MAX_LINE_LENGTH + 2];
                                    sprintf(temp, "%s %s", label, exp);
                                    appendToOutput(&output, &outSize, temp);
                                } else {
                                    appendToOutput(&output, &outSize, exp);
                                }
                            } else {
                                appendToOutput(&output, &outSize, line); /* Print original line */
                            }
                        }
                    }

                    if (token) free(token);
                    if (hasLabel && label) free(label);
                }
            }
        }
    }

    fclose(asF);

    if (!error) {
        amF = fopen(amName, "w");
        if (!amF) {
            printError(filename, 0, ERR_OPEN_FILE, amName);
            error = TRUE;
        } else {
            fprintf(amF, "%s", output);
            fclose(amF);
        }
    }

    free(output);

    if (outMacros) *outMacros = macros;
    else freeMacros(macros);
    return !error;
}
