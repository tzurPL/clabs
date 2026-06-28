/*
 * Final Project - Assembler
 * Tzur Pinto Lazar
 */

#include "globals.h"
#include "util.h"
#include "table.h"
#include "preproc.h"
#include "first_pass.h"
#include "second_pass.h"
#include "errors.h"
#include <string.h>

/* writeOutput func: generates the files after assembly */
void writeOutput(const char *filename, SymbolNode *symbols, CodeNode *codeHead, DataNode *dataHead, ExtUsage *extUsage, int IC, int DC) {
    char name[MAX_LINE_LENGTH];
    FILE *f;
    CodeNode *c;
    DataNode *d;
    SymbolNode *s;
    boolean hasEnt = FALSE;
    int count = 0;

    /* create the OB file */
    strcpy(name, filename); strcat(name, ".ob");
    f = fopen(name, "w");
    if (f) {
        /* Header: total instruction bytes and total data bytes */
        fprintf(f, "%d %d\n", IC - 100, DC);
        c = codeHead;
        while (c) {
            fprintf(f, "%04d %02X %02X %02X %02X\n", c->address, (unsigned int)(c->word & ((1UL << 8) - 1)), (unsigned int)((c->word >> 8) & ((1UL << 8) - 1)), (unsigned int)((c->word >> 16) & ((1UL << 8) - 1)), (unsigned int)((c->word >> 24) & ((1UL << 8) - 1)));
            c = c->next;
        }
        d = dataHead;
        while (d) {
            if (count % 4 == 0) fprintf(f, "%04d", d->address);
            fprintf(f, " %02X", d->byte);
            d = d->next;
            count++;
            if (count % 4 == 0 || !d) fprintf(f, "\n");
        }
        fclose(f);
    }

    /* create the ENT file */
    s = symbols;
    while (s && !hasEnt) { if (s->isEntry) { hasEnt = TRUE; } s = s->next; }
    if (hasEnt) {
        strcpy(name, filename); strcat(name, ".ent");
        f = fopen(name, "w");
        if (f) {
            s = symbols;
            while (s) { if (s->isEntry) fprintf(f, "%s %04d\n", s->name, s->value); s = s->next; }
            fclose(f);
        }
    }

    /* create the EXT file */
    if (extUsage) {
        strcpy(name, filename); strcat(name, ".ext");
        f = fopen(name, "w");
        if (f) {
            while (extUsage) { fprintf(f, "%s %04d\n", extUsage->name, extUsage->address); extUsage = extUsage->next; }
            fclose(f);
        }
    }
}

/* free functions to avoid memory leaks */
void freeCode(CodeNode *head) {
    while (head) {
        CodeNode *temp = head;
        head = head->next;
        if (temp->labelDep) free(temp->labelDep);
        free(temp);
    }
}

void freeData(DataNode *head) {
    while (head) {
        DataNode *temp = head;
        head = head->next;
        free(temp);
    }
}

/* processFile func: orchestrate the whole thing */
void processFile(const char *filename) {
    SymbolNode *symbols = NULL;
    CodeNode *codeHead = NULL;
    DataNode *dataHead = NULL;
    ExtUsage *extUsage = NULL;
    ErrorNode *errorList = NULL;
    MacroNode *macros = NULL;
    int IC = IC_INIT, DC = 0;
    boolean pass1Ok, pass2Ok;

    printf("Processing %s...\n", filename);

    if (!preprocess(filename, &macros)) {
        printf("Failed to preprocess %s. Halting assembly.\n", filename);
        return;
    }

    pass1Ok = firstPass(filename, &symbols, &codeHead, &dataHead, &IC, &DC, &errorList, macros);
    pass2Ok = secondPass(filename, symbols, codeHead, &extUsage, &errorList);

    if (pass1Ok && pass2Ok) {
        writeOutput(filename, symbols, codeHead, dataHead, extUsage, IC, DC);
        printf("Successfully assembled %s\n", filename);
    } else {
        printErrors(filename, errorList);
        printf("Failed to assemble %s\n", filename);
    }

    freeSymbols(symbols);
    freeCode(codeHead);
    freeData(dataHead);
    freeExtUsage(extUsage);
    freeErrors(errorList);
    freeMacros(macros);
}

int main(int argc, char *argv[]) {
    int i;
    if (argc < 2) {
        fprintf(stderr, "Usage: %s file1 ...\n", argv[0]);
        return 1;
    }
    for (i = 1; i < argc; i++) processFile(argv[i]);
    return 0;
}
