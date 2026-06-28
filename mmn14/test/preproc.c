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
 * checks if the first token is a label, if yes saves it, sets the hasLabel flag and gets the next token.
 * the input is a pointer to the token string pointer,a pointer to store the label,a pointer to the input string pointer,and a boolean flag.
 * returns 1 if a label was found 0 if not.
 */
int checkLabel(char **token, char **label, char **ptr, boolean *hasLabel) {
    if (isLabelDef(*token)) {/*check if the label is defined good*/
        *hasLabel = TRUE;
        *label = *token;
        *token = getToken(ptr);/*get the next token into the pointer that goes over the line*/
        return 1;
    }
    return 0;
}

/*
 * checkMacroDef func
 * checks if the token is the start of a macro, validates and extracts the macro name if it is a macro.
 * the input is the token, a pointer to the input string pointer, a char to store the macro name, the filename, and the line num.
 * returns 1 if a valid macro was found, 0 if not a macro, and -1 if there was an error.
 */
int checkMacroDef(const char *token, char **ptr, char *macroName, const char *filename, int lineNum) {
    char *exp;
    if (strcmp(token, "mcro") == 0) {/*check if token is a macro word*/
        exp = getToken(ptr);/*get the macro name*/
        if (exp) {/*if macro name was found*/
            if (isReservedKeyword(exp)) {/*check if name is a reserved keyword*/
                printError(filename, lineNum, ERR_RESERVED_KEYWORD, exp);/*save error*/
                free(exp);/*free token*/
                return -1;
            }
            if (!checkLabelN(exp)) {/*check if name is a valid label*/
                printError(filename, lineNum, ERR_INVALID_LABEL_FORMAT, exp);/*save error*/
                free(exp);/*free token*/
                return -1;
            }
            strcpy(macroName, exp);/*copy the valid macro name*/
            free(exp);/*free token*/
            return 1;
        }
        printError(filename, lineNum, ERR_EXTRA_TEXT, "Missing macro name");/*report missing name*/
        return -1;
    }
    return 0;/*not a macro*/
}

/*
 * appendToOutput func
 * appends a string to the output temp string, reallocates memory if needs.
 * the input is a pointer to the temp out string, a pointer to its size, and the string to append.
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
 * checks if a macro is defined good. updates the macro state and reports errors.
 * the input is macro definition status, inMacro flag, error flag, filename, line number, macros, macro name, and macro content pointer.
 * returns void.
 */
void procMacroDef(int macroDefStatus, boolean *inMacro, boolean *error, const char *filename, int lineNum, MacroNode *macros, char *macroName, char **macroContent) {
    *inMacro = TRUE;/*set flag to true 'cause we are inside a macro*/
    if (macroDefStatus == -1) *error = TRUE;/*if macro definition had an error set flag*/
    else {
        if (getMacroContent(macros, macroName)) {/*check if macro already exists*/
            printError(filename, lineNum, ERR_MACRO_REDEFINITION, macroName);/*report error*/
            *error = TRUE;
        }
    }
    *macroContent = (char *)safeMalloc(1);/*allocate initial memory for macro content*/
    (*macroContent)[0] = '\0';/*init with \0*/
}

/*
 * writeAm func
 * writes the output temp string to the .am file if there were no errors.
 * the input is the error flag, filename, .am filename, and the output string.
 * returns void.
 */
void writeAm(boolean *error, const char *filename, const char *amName, const char *output) {
    FILE *amF;
    if (!*error) {/*if there were no errors*/
        amF = fopen(amName, "w");/*open .am file for writing*/
        if (!amF) {/*if file failed to open*/
            printError(filename, 0, ERR_OPEN_FILE, amName);/*save error*/
            *error = TRUE;/*set error flag*/
        } else {
            fprintf(amF, "%s", output);/*write the output to the file*/
            fclose(amF);/*close the file*/
        }
    }
}

/*
 * procLine func
 * processes a line or a macro inline. appends to macro content if in a macro, if not in macro expands or appends the line.
 * the input is inMacro flag, macros, token, line, macro content pointer, label, hasLabel flag, output string pointer, and output size.
 * returns void.
 */
void procLine(boolean inMacro, MacroNode *macros, char *token, char *line, char **macroContent, char *label, boolean hasLabel, char **output, size_t *outSize) {
    char *exp;/*a place to keep an expanded macro*/
    if (inMacro) {/*if we are inside a macro*/
        /*append the original line to the macro content*/
        *macroContent = (char *)safeRealloc(*macroContent, strlen(*macroContent) + strlen(line) + 1);/*reallocate memory*/
        strcat(*macroContent, line);/*attach the line*/
    } else {/*not in a macro*/
        exp = getMacroContent(macros, token);/*check if token is a macro call*/
        if (exp) {/*if it is a macro call*/
            if (hasLabel) {/*if there is a label before the macro call*/
                /*keep the label and extand the macro*/
                char temp[MAX_LINE_LENGTH + 2];/*place to hold the new temp line created*/
                sprintf(temp, "%s %s", label, exp);/*save to the temp line the line afte expantion*/
                appendToOutput(output, outSize, temp);/*append to output*/
            } else {/*no label before*/
                appendToOutput(output, outSize, exp);/*append expanded macro to output*/
            }
        } else {/*not a macro call*/
            appendToOutput(output, outSize, line);/*print og line*/
        }
    }
}

/*
 * preprocess func
 * runs the preprocessor on the input file expanding macros and removing comments and empty lines.
 * the input is the filename and a pointer to store the generated macro list.
 * returns a boolean indicating whether the preprocessing succeeded without errors.
 */
boolean preprocess(const char *filename, MacroNode **outMacros) {
    char asName[MAX_LINE_LENGTH], amName[MAX_LINE_LENGTH];/*a place for the name of the file*/
    FILE *asF;/*a file pointer for the og .as file*/
    char line[MAX_LINE_LENGTH + 2];/*a place to save the line including the \n and \0*/
    MacroNode *macros = NULL;/*list for the macros and their contents*/
    boolean inMacro = FALSE, error = FALSE;/*flags for if error and if it is in a macro*/
    char macroName[MAX_LABEL_LENGTH];
    char *macroContent = NULL;
    char *token, *label;/*a string to save the current token in the worked on line, a string to save label*/
    int lineNum = 0, macroDefStatus;

    char *output = (char *)safeMalloc(1);/*a place to store the output that will go to the file*/
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
        boolean hasLabel = FALSE;/*label flag*/

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
            } else {/*if found token*/
                label = NULL;
                /*if the first token is a label save it and check the next token*/
                checkLabel(&token, &label, &ptr, &hasLabel);

                if (!token) {/*if the line is empty*/
                    if (!inMacro) appendToOutput(&output, &outSize, line);
                    if (hasLabel) free(label);
                } else {
                    macroDefStatus = checkMacroDef(token, &ptr, macroName, filename, lineNum);/*check if token is a macro start*/
                    if (macroDefStatus != 0) {/*if it is a macro/was an error*/
                        procMacroDef(macroDefStatus, &inMacro, &error, filename, lineNum, macros, macroName, &macroContent);/*try to process it*/
                    } else if (strcmp(token, "mcroend") == 0) {/*check if token is macro end*/
                        addMacro(&macros, macroName, macroContent);/*save macro*/
                        free(macroContent);
                        inMacro = FALSE;
                    } else {/*not a macro*/
                        procLine(inMacro, macros, token, line, &macroContent, label, hasLabel, &output, &outSize);/*process line*/
                    }

                    /*free*/
                    if (token) free(token);
                    if (hasLabel && label) free(label);
                }
            }
        }
    }

    fclose(asF);/*close the file*/

    writeAm(&error, filename, amName, output);/*write the output to .am file*/

    /*free*/
    free(output);

    if (outMacros) *outMacros = macros;/*output the macros*/
    else freeMacros(macros);/*free macros*/
    return !error;
}
