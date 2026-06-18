#include "preproc.h"
#include "util.h"
#include "table.h"
#include "errors.h"
#include <string.h>

boolean preprocess(const char *filename) {
    char asName[MAX_LINE_LENGTH], amName[MAX_LINE_LENGTH];
    FILE *asF, *amF;
    char line[MAX_LINE_LENGTH + 2];
    MacroNode *macros = NULL;
    boolean inMacro = FALSE, error = FALSE;
    char macroName[MAX_LABEL_LENGTH];
    char *macroContent = NULL;
    char *token, *exp, *label;
    int lineNum = 0, len;

    strcpy(asName, filename); strcat(asName, ".as");
    strcpy(amName, filename); strcat(amName, ".am");

    asF = fopen(asName, "r");
    if (!asF) {
        printError(filename, 0, ERR_OPEN_FILE, asName);
        return FALSE;
    }
    amF = fopen(amName, "w");
    if (!amF) {
        printError(filename, 0, ERR_OPEN_FILE, amName);
        fclose(asF);
        return FALSE;
    }

    while (fgets(line, sizeof(line), asF)) {
        char *ptr = line;
        boolean hasLabel = FALSE;

        lineNum++;

        len = strlen(line);
        if (len > 0 && line[len-1] == '\n') len--;
        if (len > 0 && line[len-1] == '\r') len--;
        if (len > 80) {
            printError(filename, lineNum, ERR_LINE_TOO_LONG, NULL);
            error = TRUE;
            continue;
        }

        skipSpaces(&ptr);
        if (*ptr == '\0' || *ptr == ';') {
            if (!inMacro) fprintf(amF, "%s", line);
            continue;
        }

        token = getToken(&ptr);
        if (!token) {
            if (!inMacro) fprintf(amF, "%s", line);
            continue;
        }

        label = NULL;
        /* If the first token is a label, save it and check the NEXT token */
        if (token[strlen(token)-1] == ':') {
            hasLabel = TRUE;
            label = token;
            token = getToken(&ptr);
        }

        if (!token) {
            if (!inMacro) fprintf(amF, "%s", line);
            if (hasLabel) free(label);
            continue;
        }

        if (strcmp(token, "mcro") == 0) {
            inMacro = TRUE;
            exp = getToken(&ptr);
            if (exp) {
                if (isReservedKeyword(exp)) {
                    printError(filename, lineNum, ERR_RESERVED_KEYWORD, exp);
                    error = TRUE;
                }
                strcpy(macroName, exp);
                free(exp);
            } else {
                printError(filename, lineNum, ERR_EXTRA_TEXT, "Missing macro name");
                error = TRUE;
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
                        fprintf(amF, "%s %s", label, exp);
                    } else {
                        fprintf(amF, "%s", exp);
                    }
                } else {
                    fprintf(amF, "%s", line); /* Print original line */
                }
            }
        }

        if (token) free(token);
        if (hasLabel && label) free(label);
    }

    fclose(asF);
    fclose(amF);
    freeMacros(macros);
    return !error;
}
