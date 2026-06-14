/*
 * maman22 - Tzur Pinto Lazar
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "complex.h"

#ifndef MAX_COMMAND_LEN
#define MAX_COMMAND_LEN 50
#endif
#ifndef MAX_INPUT_LEN
#define MAX_INPUT_LEN 1024
#endif
#ifndef NUM_OF_VAR
#define NUM_OF_VAR 6
#endif

complex* getVar(char name, complex *variables) {
    if (name >= 'A' && name <= 'F') return &variables[name - 'A'];
    return NULL;
}

/* Helper function ONLY for checking multiple consecutive commas.
   It enforces left-to-right precedence by aborting the check if it
   hits an invalid character (like 'h' or 'a') before finding the commas. */
int check_multiple_commas(char *args) {
    int comma_found = 0;
    while (*args) {
        if (*args == ',') {
            if (comma_found) return 1; /* Found consecutive commas */
            comma_found = 1;
        } else if (isspace((unsigned char)*args)) {
            /* skip spaces */
        } else if ((*args >= 'A' && *args <= 'F') ||
                   isdigit((unsigned char)*args) ||
                   *args == '-' || *args == '.' || *args == '+') {
            comma_found = 0; /* Reset flag when a valid parameter char is found */
        } else {
            /* If we hit an invalid character (like 'a', 'h', etc.), left-to-right
               precedence dictates that this error comes BEFORE any consecutive commas
               that might be further down the string. Stop checking for commas here. */
            return 0;
        }
        args++;
    }
    return 0;
}

void taskRead(char *args, complex *variables) {
    char var, extra[2]; /*2 chars to see if there is extra after command even a single thing*/
    double r, i;

    /* get from the command the needed vars and keep count of them
    (if there is extra that isnt needed then the count will include the extra var meaning it needs to return with error*/
    int res = sscanf(args, " %c , %lf , %lf %1s", &var, &r, &i, extra);

    /* Enforce left-to-right: check if the first parameter is an undefined variable first */
    if (res >= 1 && !getVar(var, variables)) {
        fprintf(stderr, "Undefined complex variable\n");
        return;
    }

    /* Then check for multiple commas */
    if (check_multiple_commas(args)) {
        fprintf(stderr, "Multiple consecutive commas\n");
        return;
    }

    if (res == 3) {/*if there is no extra after command*/
        complex *v = getVar(var, variables);/*check for the inputted var in var list*/
        if (v) { v->real = r; v->imag = i; }/*if it exists run the proper command*/
        else fprintf(stderr, "Undefined complex variable\n");
    } else if (res == 4) fprintf(stderr, "Extraneous text after end of command\n");/*check if there is extra text after the command*/
    else fprintf(stderr, "Invalid parameters\n");
}

void taskPrint(char *args, complex *variables) {
    char var, extra[2];/*2 chars to see if there is extra after command even a single thing*/

    /* get from the command the needed vars and keep count of them
    (if there is extra that isnt needed then the count will include the extra var meaning it needs to return with error*/
    int res = sscanf(args, " %c %1s", &var, extra);

    /* Enforce left-to-right: check variable first */
    if (res >= 1 && !getVar(var, variables)) {
        fprintf(stderr, "Undefined complex variable\n");
        return;
    }

    /* Then check for multiple commas */
    if (check_multiple_commas(args)) {
        fprintf(stderr, "Multiple consecutive commas\n");
        return;
    }

    if (res == 1) {/*if there is no extra after command*/
        complex *v = getVar(var, variables);/*check for the inputted var in var list*/
        if (v) printComp(v);/*if it exists run the proper command*/
        else fprintf(stderr, "Undefined complex variable\n");
    } else if (res == 2) fprintf(stderr, "Extraneous text after end of command\n");/*check if there is extra text after the command*/
    else fprintf(stderr, "Invalid parameters\n");
}

void taskAdd(char *args, complex *variables) {
    char n1, n2, extra[2];/*2 chars to see if there is extra after command even a single thing*/

    /* get from the command the needed vars and keep count of them
    (if there is extra that isnt needed then the count will include the extra var meaning it needs to return with error*/
    int res = sscanf(args, " %c , %c %1s", &n1, &n2, extra);

    if (res >= 1 && !getVar(n1, variables)) {
        fprintf(stderr, "Undefined complex variable\n");
        return;
    }

    if (check_multiple_commas(args)) {
        fprintf(stderr, "Multiple consecutive commas\n");
        return;
    }

    if (res == 2) {/*if there is no extra after command*/
        complex *v1 = getVar(n1, variables), *v2 = getVar(n2, variables);/*check for the inputted vars in var list*/
        if (v1 && v2) { complex res_c = addComp(v1, v2); printComp(&res_c); }/*if they exist run the proper commands*/
        else fprintf(stderr, "Undefined complex variable\n");
    } else if (res == 3) fprintf(stderr, "Extraneous text after end of command\n");/*check if there is extra text after the command*/
    else fprintf(stderr, "Invalid parameters\n");
}

void taskSub(char *args, complex *variables) {
    char n1, n2, extra[2];/*2 chars to see if there is extra after command even a single thing*/

    /* get from the command the needed vars and keep count of them
    (if there is extra that isnt needed then the count will include the extra var meaning it needs to return with error*/
    int res = sscanf(args, " %c , %c %1s", &n1, &n2, extra);

    if (res >= 1 && !getVar(n1, variables)) {
        fprintf(stderr, "Undefined complex variable\n");
        return;
    }

    if (check_multiple_commas(args)) {
        fprintf(stderr, "Multiple consecutive commas\n");
        return;
    }

    if (res == 2) {/*if there is no extra after command*/
        complex *v1 = getVar(n1, variables), *v2 = getVar(n2, variables);/*check for the inputted vars in var list*/
        if (v1 && v2) { complex res_c = subComp(v1, v2); printComp(&res_c); }/*if they exist run the proper commands*/
        else fprintf(stderr, "Undefined complex variable\n");
    } else if (res == 3) fprintf(stderr, "Extraneous text after end of command\n");/*check if there is extra text after the command*/
    else fprintf(stderr, "Invalid parameters\n");
}

void taskMultReal(char *args, complex *variables) {
    char var, extra[2];/*2 chars to see if there is extra after command even a single thing*/
    double num;

    /* get from the command the needed vars and keep count of them
    (if there is extra that isnt needed then the count will include the extra var meaning it needs to return with error*/
    int res = sscanf(args, " %c , %lf %1s", &var, &num, extra);

    if (res >= 1 && !getVar(var, variables)) {
        fprintf(stderr, "Undefined complex variable\n");
        return;
    }

    if (check_multiple_commas(args)) {
        fprintf(stderr, "Multiple consecutive commas\n");
        return;
    }

    if (res == 2) {/*if there is no extra after command*/
        complex *v = getVar(var, variables);/*check for the inputted var in var list*/
        if (v) { complex res_c = multReal(v, num); printComp(&res_c); }/*if they exist run the proper commands*/
        else fprintf(stderr, "Undefined complex variable\n");
    } else if (res == 3) fprintf(stderr, "Extraneous text after end of command\n");/*check if there is extra text after the command*/
    else fprintf(stderr, "Invalid parameters\n");
}

void taskMultImg(char *args, complex *variables) {
    char var, extra[2];/*2 chars to see if there is extra after command even a single thing*/
    double num;

    /* get from the command the needed vars and keep count of them
    (if there is extra that isnt needed then the count will include the extra var meaning it needs to return with error*/
    int res = sscanf(args, " %c , %lf %1s", &var, &num, extra);

    if (res >= 1 && !getVar(var, variables)) {
        fprintf(stderr, "Undefined complex variable\n");
        return;
    }

    if (check_multiple_commas(args)) {
        fprintf(stderr, "Multiple consecutive commas\n");
        return;
    }

    if (res == 2) {/*if there is no extra after command*/
        complex *v = getVar(var, variables);/*check for the inputted var in var list*/
        if (v) { complex res_c = multImg(v, num); printComp(&res_c); }/*if they exist run the proper commands*/
        else fprintf(stderr, "Undefined complex variable\n");
    } else if (res == 3) fprintf(stderr, "Extraneous text after end of command\n");/*check if there is extra text after the command*/
    else fprintf(stderr, "Invalid parameters\n");
}

void taskMultComp(char *args, complex *variables) {
    char n1, n2, extra[2];/*2 chars to see if there is extra after command even a single thing*/

    /* get from the command the needed vars and keep count of them
    (if there is extra that isnt needed then the count will include the extra var meaning it needs to return with error*/
    int res = sscanf(args, " %c , %c %1s", &n1, &n2, extra);

    if (res >= 1 && !getVar(n1, variables)) {
        fprintf(stderr, "Undefined complex variable\n");
        return;
    }

    if (check_multiple_commas(args)) {
        fprintf(stderr, "Multiple consecutive commas\n");
        return;
    }

    if (res == 2) {/*if there is no extra after command*/
        complex *v1 = getVar(n1, variables), *v2 = getVar(n2, variables);/*check for the inputted variables in var list*/
        if (v1 && v2) { complex res_c = multComp(v1, v2); printComp(&res_c); }/*if they exist run the proper command*/
        else fprintf(stderr, "Undefined complex variable\n");
    } else if (res == 3) fprintf(stderr, "Extraneous text after end of command\n");/*check if there is extra text after the command*/
    else fprintf(stderr, "Invalid parameters\n");
}

void taskAbs(char *args, complex *variables) {
    char var, extra[2];/*2 chars to see if there is extra after command even a single thing*/

    /* get from the command the needed vars and keep count of them
    (if there is extra that isnt needed then the count will include the extra var meaning it needs to return with error*/
    int res = sscanf(args, " %c %1s", &var, extra);

    if (res >= 1 && !getVar(var, variables)) {
        fprintf(stderr, "Undefined complex variable\n");
        return;
    }

    if (check_multiple_commas(args)) {
        fprintf(stderr, "Multiple consecutive commas\n");
        return;
    }

    if (res == 1) {/*if there is no extra after command*/
        complex *v = getVar(var, variables);/*check for the inputted var in var list*/
        if (v) printf("%.2f\n", absComp(v));/*if it exists run the proper command*/
        else fprintf(stderr, "Undefined complex variable\n");/*if not return with error*/
    } else if (res == 2) fprintf(stderr, "Extraneous text after end of command\n");/*check if there is extra text after the command*/
    else fprintf(stderr, "Invalid parameters\n");/*if the params were invalid*/
}

void taskStop(char *args, complex *variables) {
    char extra[2];/*2 chars to see if there is extra after command even a single thing*/

    if (check_multiple_commas(args)) {
        fprintf(stderr, "Multiple consecutive commas\n");
        return;
    }

    if (sscanf(args, " %1s", extra) == 1) fprintf(stderr, "Extraneous text after end of command\n");/*check if there is extra text after the command*/
    else exit(0);
}

/*orginize the commands in a struct of their names and functions*/
typedef struct {
    char *name;
    void (*op)(char *, complex *);
} Command;

/* put them in the array*/
Command cmdTable[] = {
    {"read_comp", taskRead}, {"print_comp", taskPrint},
    {"add_comp", taskAdd}, {"sub_comp", taskSub},
    {"mult_comp_real", taskMultReal}, {"mult_comp_img", taskMultImg},
    {"mult_comp_comp", taskMultComp}, {"abs_comp", taskAbs},
    {"stop", taskStop}, {NULL, NULL}
};

void runCommand(char *line, complex *variables) {
    char cmd[MAX_COMMAND_LEN], *args;/* where it will disect the input*/
    int i, n = 0;

    /* skip spaces before command */
    while (*line && isspace((unsigned char)*line)) line++;
    if (*line == '\0') return;

    /* go over the line for the possible command lenght or until it hit a space or comma */
    while (line[n] && !isspace((unsigned char)line[n]) && line[n] != ',' && n < 49) {
        cmd[n] = line[n];/* extract the command*/
        n++;
    }
    cmd[n] = '\0';

    /*put the passed pointer to after the command */
    args = line + n;

    /* go over the possible commands to try and find the inputted one, if it is not in the list then return with error */
    for (i = 0; cmdTable[i].name; i++) {
        if (strcmp(cmd, cmdTable[i].name) == 0) break;
    }

    if (cmdTable[i].name == NULL) {
        fprintf(stderr, "Undefined command name\n");
        return;
    }

    /* check for illigal comma right after command name */
    while (*args && isspace((unsigned char)*args)) args++;/* skip spaces*/
    if (*args == ',') {
        fprintf(stderr, "Illegal comma\n");
        return;
    }

    /* execute the found command */
    cmdTable[i].op(args, variables);
}

int main() {
    char line[MAX_INPUT_LEN];
    complex variables[NUM_OF_VAR];
    int i;

    for (i = 0; i < NUM_OF_VAR; i++) { variables[i].real = variables[i].imag = 0.0; }/*reset the variables*/

    printf("Complex Number Calculator (Elegant)\n");
    while (fgets(line, sizeof(line), stdin)) {/*execute commands until no input*/
        runCommand(line, variables);
    }
    return 0;
}
