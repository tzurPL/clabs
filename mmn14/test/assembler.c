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
            /*printing process of the data section: */
            if (count % NUM_BYTES_WORD == 0) {
                /*if it is the start of a new row print the address */
                fprintf(f, "%04d", d->address);/*print address for a new line of data*/
            }
            fprintf(f, " %02X", d->byte);/*print data byte*/
            d = d->next;/*go to the next data node */
            count++;/*mark that that byte was printed */
            if (count % NUM_BYTES_WORD == 0 || !d) { fprintf(f, "\n");/*print newline after 4 bytes were printed or at end of list*/ }
        }
        fclose(f);/*close file*/
    }

    /*create ENT file*/
    s = symbols;/*symbol table */
    while (s && !hasEnt) {/*go over the list until entry found */
        if (s->isEntry) { hasEnt = TRUE; }
        s = s->next;
    }
    if (hasEnt) {/*if there are entry symbols*/
        strcpy(name, filename);
        remAsExtension(name);/*remove .as extension if exists*/
        strcat(name, ".ent");/*add .ent extension*/
        f = fopen(name, "w");/*open for writing*/
        if (f) {
            s = symbols;
            while (s) {
                /*if symbol is entry print it and continue */
                if (s->isEntry) { fprintf(f, "%s %04d\n", s->name, s->address); }
                s = s->next;
            }/*write entry symbol deets*/
            fclose(f);/*close file*/
        }
    }

    /*create EXT file*/
    if (extUsage) {/*if extern symbols were used*/
        strcpy(name, filename);
        remAsExtension(name);/*remove .as extension if exists*/
        strcat(name, ".ext");/*add .ext extension*/
        f = fopen(name, "w");/*open for writing*/
        if (f) {
            while (extUsage) {
                fprintf(f, "%s %04d\n", extUsage->name, extUsage->address);
                extUsage = extUsage->next;
            }/*write extern deets*/
            fclose(f);/*close file*/
        }
    }
}

/*
 * freeCode func
 * frees memory allocated for the code list
 * the input is head of the CodeNode list
 * returns void
 */
void freeCode(CodeNode *head) {
    while (head) {/*go through the list*/
        CodeNode *temp = head;
        head = head->next;/*move to next node*/
        if (temp->labelRef) { free(temp->labelRef);/*free label reference if exists*/ }
        free(temp);/*free node*/
    }
}

/*
 * freeData func
 * frees memory allocated for the data list
 * the input is head of the DataNode list
 * returns void
 */
void freeData(DataNode *head){
    while (head) {/*go through the list*/
        DataNode *temp = head;
        head = head->next;/*move to next node*/
        free(temp);/*free node*/
    }
}

/*
 * processFile func
 * this func does the assembly process for the file
 * the input is the name of the file to process
 * returns void
 */
void processFile(const char *filename) {
    SymbolNode *symbols = NULL;/*init symbol table*/
    CodeNode *codeHead = NULL;/*init code list*/
    DataNode *dataHead = NULL;/*init data list*/
    ExtUsage *extUsage = NULL;/*init extern list*/
    ErrorNode *errorList = NULL;/*init error list*/
    MacroNode *macros = NULL;/*init macro list*/
    int IC = IC_INIT, DC = 0;/*init counters*/
    boolean pass1,pass2;

    printf("Processing %s...\n", filename);

    if (!preprocess(filename, &macros)) {/*run preproc*/
        printf("Failed to preprocess %s\n Stopping assembly.\n", filename);
        return;/*stop if preproc fails*/
    }

    pass1 = firstPass(filename, &symbols, &codeHead, &dataHead, &IC, &DC, &errorList, macros);/*run first pass*/
    pass2 = secondPass(filename, symbols, codeHead, &extUsage, &errorList);/*run second pass*/

    if (pass1 && pass2) {/*if passes succeeded*/
        writeOutput(filename, symbols, codeHead, dataHead, extUsage, IC, DC);/*write output files*/
        printf("Successfully assembled %s\n", filename);
    } else {
        printErrors(filename, errorList);/*print all errors*/
        printf("Failed to assemble %s\n", filename);
    }

    /*free all stuff*/
    freeSymbols(symbols);
    freeCode(codeHead);
    freeData(dataHead);
    freeExtUsage(extUsage);
    freeErrors(errorList);
    freeMacros(macros);
}


int main(int argc, char *argv[]) {
    int i;
    if (argc < MIN_ARGS) {/*check if args exists*/
        fprintf(stderr, "Usage: %s file1 ...\n", argv[0]);
        return 1;
    }
    for (i = 1; i < argc; i++) {
        processFile(argv[i]);/*proc each file*/
    }
    return 0;
}
