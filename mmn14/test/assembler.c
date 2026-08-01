/*
 * assembler.c
 * mmn14
 * Tzur Pinto Lazar
 */

#include "errors.h"
#include "first_pass.h"
#include "globals.h"
#include "preproc.h"
#include "second_pass.h"
#include "table.h"
#include "util.h"
#include <string.h>

/*
 * writeOutput func
 * generates the output files
 * the input is filename, symbols list, code list, data list, extern usage list, instruction counter, and data counter
 * returns void
 */
void writeOutput(const char *filename, SymbolNode *symbols, CodeNode *codeHead, DataNode *dataHead, ExtUsage *extUsage,
                 int IC, int DC) {
    char name[MAX_LINE_LENGTH];
    FILE *f;
    CodeNode *c;
    DataNode *d;
    SymbolNode *s;
    boolean hasEnt = FALSE;
    int count = 0;

    /*create the ob file*/
    strcpy(name, filename);
    remAsExtension(name);/*remove the .as extension if present*/
    strcat(name, ".ob");/*append .ob to the file*/
    f = fopen(name, "w");/*open file*/
    if (f) {/*if file opened*/
        /*put the header of the file with the total instruction bytes and total data bytes*/
        fprintf(f, "%d %d\n", IC - IC_INIT, DC);
        c = codeHead;
        while (c) {/*go through code nodes and print them as machine code in hex according to the guide for the project*/
            /*The process of printing the code nodes as machine code is done via bitfields */
            mCode mc;
            mc.rawCode = c->inst.rawCode;/*copy the raw code for disection */
            /*print the instruction as hex via spliting it to bitfields for every byte and then translate it to hex */
            fprintf(f, "%04d %02X %02X %02X %02X\n", c->address, mc.bytes.b0, mc.bytes.b1, mc.bytes.b2, mc.bytes.b3);
            c = c->next;/*move to next code node */
        }
        d = dataHead;
        while (d) {/*iterate through data nodes*/
            if (count % NUM_BYTES_WORD == 0) {
                fprintf(f, "%04d", d->address);/*print address for a new line of data*/
            }
            fprintf(f, " %02X", d->byte);/*print data byte*/
            d = d->next;
            count++;
            if (count % NUM_BYTES_WORD == 0 || !d) { fprintf(f, "\n");/*newline after 4 bytes or at end*/ }
        }
        fclose(f);/*close the file*/
    }

    /*create the ENT file*/
    s = symbols;
    while (s && !hasEnt) {
        if (s->isEntry) { hasEnt = TRUE; }
        s = s->next;
    }/*check if any entry symbols exist*/
    if (hasEnt) {/*if there are entry symbols*/
        strcpy(name, filename);
        remAsExtension(name);/*strip .as extension if present*/
        strcat(name, ".ent");/*append .ent extension*/
        f = fopen(name, "w");/*open for writing*/
        if (f) {
            s = symbols;
            while (s) {
                if (s->isEntry) { fprintf(f, "%s %04d\n", s->name, s->address); }
                s = s->next;
            }/*write entry symbol details*/
            fclose(f);/*close the file*/
        }
    }

    /*create the EXT file*/
    if (extUsage) {/*if external symbols were used*/
        strcpy(name, filename);
        remAsExtension(name);/*strip .as extension if present*/
        strcat(name, ".ext");/*append .ext extension*/
        f = fopen(name, "w");/*open for writing*/
        if (f) {
            while (extUsage) {
                fprintf(f, "%s %04d\n", extUsage->name, extUsage->address);
                extUsage = extUsage->next;
            }/*write external usage details*/
            fclose(f);/*close the file*/
        }
    }
}

/*
 * freeCode func
 * frees the memory allocated for the code list to avoid memory leaks.
 * the input is head of the CodeNode list.
 * returns void.
 */
void freeCode(CodeNode *head) {
    while (head) {/*iterate through the list*/
        CodeNode *temp = head;
        head = head->next;/*move to next node*/
        if (temp->labelDep) { free(temp->labelDep);/*free label dependency string if exists*/ }
        free(temp);/*free the node itself*/
    }
}

/*
 * freeData func
 * frees the memory allocated for the data list to avoid memory leaks.
 * the input is head of the DataNode list.
 * returns void.
 */
void freeData(DataNode *head) {
    while (head) {/*iterate through the list*/
        DataNode *temp = head;
        head = head->next;/*move to next node*/
        free(temp);/*free the node itself*/
    }
}

/*
 * processFile func
 * orchestrates the entire assembly process for a single file, including preprocessing, first pass, second pass, and
 * output generation. the input is the name of the file to process. returns void.
 */
void processFile(const char *filename) {
    SymbolNode *symbols = NULL;/*initialize symbol table*/
    CodeNode *codeHead = NULL;/*initialize code list*/
    DataNode *dataHead = NULL;/*initialize data list*/
    ExtUsage *extUsage = NULL;/*initialize external usage list*/
    ErrorNode *errorList = NULL;/*initialize error list*/
    MacroNode *macros = NULL;/*initialize macro list*/
    int IC = IC_INIT, DC = 0;/*initialize counters*/
    boolean pass1,pass2;

    printf("Processing %s...\n", filename);

    if (!preprocess(filename, &macros)) {/*run preprocessor*/
        printf("Failed to preprocess %s. Halting assembly.\n", filename);
        return;/*stop if preprocessing fails*/
    }

    pass1 = firstPass(filename, &symbols, &codeHead, &dataHead, &IC, &DC, &errorList, macros);/*run first pass*/
    pass2 = secondPass(filename, symbols, codeHead, &extUsage, &errorList);/*run second pass*/

    if (pass1 && pass2) {/*if both passes succeeded*/
        writeOutput(filename, symbols, codeHead, dataHead, extUsage, IC, DC);/*generate output files*/
        printf("Successfully assembled %s\n", filename);
    } else {
        printErrors(filename, errorList);/*print all collected errors*/
        printf("Failed to assemble %s\n", filename);
    }

    /*free all allocated resources*/
    freeSymbols(symbols);
    freeCode(codeHead);
    freeData(dataHead);
    freeExtUsage(extUsage);
    freeErrors(errorList);
    freeMacros(macros);
}

/*
 * main func
 * the entry point of the assembler program. processes each file provided as command line arguments.
 * the input is argument count and argument vector containing filenames.
 * returns 0 on success, or 1 on usage error.
 */
int main(int argc, char *argv[]) {
    int i;
    if (argc < MIN_ARGS) {/*check if arguments were provided*/
        fprintf(stderr, "Usage: %s file1 ...\n", argv[0]);
        return 1;
    }
    for (i = 1; i < argc; i++) {
        processFile(argv[i]);/*process each file*/
    }
    return 0;
}
