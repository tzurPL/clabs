/*
 * preproc.c
 * mmn14
 * Tzur Pinto Lazar
 */

#include "preproc.h"
#include "util.h"
#include "table.h"
#include "errors.h"
#include <string.h>



/*
 * checkLabel func
 * checks if the first token is a label. if so, saves it, sets the hasLabel flag, and fetches the next token.
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
 * checkMacroDef func
 * checks if the token indicates the start of a macro definition ("mcro"). validates and extracts the macro name if it is a macro definition.
 * the input is the token, a pointer to the input string pointer, a code to store the macro name, the filename, and the line number.
 * returns 1 if a valid macro definition was found, 0 if not a macro definition, and -1 if there was an error.
 */
int checkMacroDef(const char *token, char **ptr, char *macroName, const char *filename, int lineNum) {
    char *exp;
    if (strcmp(token, "mcro") == 0) {/*check if token is a macro definition*/
        exp = getToken(ptr);/*get the macro name*/
        if (exp) {/*if macro name was provided*/
            if (isReservedKeyword(exp)) {/*check if name is a reserved keyword*/
                printError(filename, lineNum, ERR_RESERVED_KEYWORD, exp);/*report error*/
                free(exp);/*free the token*/
                return -1;
            }
            if (!isValidLabelFormat(exp)) {/*check if name is a valid label format*/
                printError(filename, lineNum, ERR_INVALID_LABEL_FORMAT, exp);/*report error*/
                free(exp);/*free the token*/
                return -1;
            }
            strcpy(macroName, exp);/*copy the valid macro name*/
            free(exp);/*free the token*/
            return 1;/*success*/
        }
        printError(filename, lineNum, ERR_EXTRA_TEXT, "Missing macro name");/*report missing name*/
        return -1;
    }
    return 0;/*not a macro definition*/
}

/*
 * appendToOutput func
 * appends a string to the output code buffer, dynamically reallocating memory as needed.
 * the input is a pointer to the code buffer, a pointer to its size, and the string to append.
 * returns void.
 */
void appendToOutput(char **code, size_t *size, const char *str) {
    size_t len = strlen(str);
    *code = (char *)safeRealloc(*code, *size + len);
    strcat(*code, str);
    *size += len;
}

/*
 * procMacroDef func
 * processes a macro definition start. updates the macro state and reports errors if redefinition occurs.
 * the input is macro definition status, inMacro flag, error flag, filename, line number, macros, macro name, and macro content pointer.
 * returns void.
 */
void procMacroDef(int macroDefStatus, boolean *inMacro, boolean *error, const char *filename, int lineNum, MacroNode *macros, char *macroName, char **macroContent) {
    *inMacro = TRUE;/*set flag to true to indicate we are inside a macro*/
    if (macroDefStatus == -1) *error = TRUE;/*if macro definition had an error, set flag*/
    else {
        if (getMacroContent(macros, macroName)) {/*check if macro already exists*/
            printError(filename, lineNum, ERR_MACRO_REDEFINITION, macroName);/*report redefinition error*/
            *error = TRUE;/*set error flag*/
        }
    }
    *macroContent = (char *)safeMalloc(1);/*allocate initial memory for macro content*/
    (*macroContent)[0] = '\0';/*initialize with null terminator*/
}

/*
 * writeAm func
 * writes the output buffer to the .am file if there were no errors during preprocessing.
 * the input is the error flag, filename, .am filename, and the output string.
 * returns void.
 */
void writeAm(boolean *error, const char *filename, const char *amName, const char *output) {
    FILE *amF;
    if (!*error) {/*if there were no errors during preprocessing*/
        amF = fopen(amName, "w");/*open .am file for writing*/
        if (!amF) {/*if file failed to open*/
            printError(filename, 0, ERR_OPEN_FILE, amName);/*report error*/
            *error = TRUE;/*set error flag*/
        } else {
            fprintf(amF, "%s", output);/*write the output to the file*/
            fclose(amF);/*close the file*/
        }
    }
}

/*
 * procLine func
 * processes a regular line or a macro expansion. appends to macro content if in a macro, otherwise expands or appends the original line.
 * the input is inMacro flag, macros, token, line, macro content pointer, label, hasLabel flag, output string pointer, and output size.
 * returns void.
 */
void procLine(boolean inMacro, MacroNode *macros, char *token, char *line, char **macroContent, char *label, boolean hasLabel, char **output, size_t *outSize) {
    char *exp;
    if (inMacro) {/*if we are currently inside a macro definition*/
        /*append the original line to the macro content*/
        *macroContent = (char *)safeRealloc(*macroContent, strlen(*macroContent) + strlen(line) + 1);/*reallocate memory*/
        strcat(*macroContent, line);/*concatenate the line*/
    } else {/*not in a macro definition*/
        exp = getMacroContent(macros, token);/*check if token is a macro call*/
        if (exp) {/*if it is a macro call*/
            if (hasLabel) {/*if there is a label preceding the macro call*/
                /*keep the label, but inject the macro expansion*/
                char temp[MAX_LINE_LENGTH + 2];
                sprintf(temp, "%s %s", label, exp);/*format label and expansion*/
                appendToOutput(output, outSize, temp);/*append to output*/
            } else {/*no label preceding*/
                appendToOutput(output, outSize, exp);/*append expansion to output*/
            }
        } else {/*not a macro call*/
            appendToOutput(output, outSize, line);/*print original line*/
        }
    }
}

/*
 * preprocess func
 * runs the preprocessor phase on the input file, expanding macros and removing comments/empty lines.
 * the input is the filename and a pointer to store the generated macro list.
 * returns a boolean indicating whether the preprocessing succeeded without errors.
 */
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

    strcpy(asName, filename); strcat(asName, ".as");/*append .as extension*/
    strcpy(amName, filename); strcat(amName, ".am");/*append .am extension*/

    asF = fopen(asName, "r");/*open the file for reading*/
    if (!asF) {/*return false if file open failed*/
        printError(filename, 0, ERR_OPEN_FILE, asName);
        free(output);
        return FALSE;
    }

    /*go through the file line by line */
    while (fgets(line, sizeof(line), asF)) {/*read line by line*/
        char *ptr = line;
        boolean hasLabel = FALSE;

        lineNum++;

        if (!checkLineLen(line)) {/*check if line is too long*/
            printError(filename, lineNum, ERR_LINE_TOO_LONG, NULL);
            error = TRUE;
        } else if (isEmptyLine(line) || isCommentLine(line)) {/*ignore empty and comment lines*/
            if (!inMacro) appendToOutput(&output, &outSize, line);
        } else {
            token = getToken(&ptr);/*get first token*/
            if (!token) {
                if (!inMacro) appendToOutput(&output, &outSize, line);
            } else {
                label = NULL;
                /*if the first token is a label, save it and check the next token*/
                checkLabel(&token, &label, &ptr, &hasLabel);

                if (!token) {
                    if (!inMacro) appendToOutput(&output, &outSize, line);
                    if (hasLabel) free(label);
                } else {
                    macroDefStatus = checkMacroDef(token, &ptr, macroName, filename, lineNum);/*check if token is a macro definition*/
                    if (macroDefStatus != 0) {
                        procMacroDef(macroDefStatus, &inMacro, &error, filename, lineNum, macros, macroName, &macroContent);
                    } else if (strcmp(token, "mcroend") == 0) {/*check if token is macro end*/
                        addMacro(&macros, macroName, macroContent);/*save macro*/
                        free(macroContent);
                        inMacro = FALSE;
                    } else {
                        procLine(inMacro, macros, token, line, &macroContent, label, hasLabel, &output, &outSize);/*process regular line*/
                    }

                    if (token) free(token);
                    if (hasLabel && label) free(label);
                }
            }
        }
    }

    fclose(asF);/*close the file*/

    writeAm(&error, filename, amName, output);/*write the output to .am file*/

    free(output);

    if (outMacros) *outMacros = macros;
    else freeMacros(macros);
    return !error;
}
