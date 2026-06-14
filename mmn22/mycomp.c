/*
 * mycomp.c
 * maman22 Tzur Pinto Lazar
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "complex.h"
#include "parser.h"

/*
 * takes variable name and two nums from the input runs checks on each part of the command
 * and updates the variable
 * the input is a pointer to the input string pointer and the array of complex variables
 * returns void
 */
void taskRead(char **ptr, complex *variables) {
    complex *v;
    double real_part, imag_part;
    if (!checkVar(ptr, &v, variables)) return;/*valid the var*/
    if (!checkComma(ptr)) return;/*check for the needed commma*/
    if (!checkNoExtraComma(ptr)) return;/*check for extra commas*/
    if (!checkNum(ptr, &real_part)) return;/*check if it is a num and if it exists */
    if (!checkComma(ptr)) return;/*check for the needed commma*/
    if (!checkNoExtraComma(ptr)) return;/*check for extra commas*/
    if (!checkNum(ptr, &imag_part)) return;/*check if it is a num and if it exists */
    if (!checkEnd(ptr)) return;/*check end*/

    /*copy the complex num entered to the variable */
    v->real = real_part;
    v->imag = imag_part;
}

/*
 * takes a variable from the input and runs checks on each part of the command and
 * prints its complex num components to the screen
 * the input is a pointer to the input string pointer and the array of complex variables
 * returns void
 */
void taskPrint(char **ptr, complex *variables) {
    complex *v;
    if (!checkVar(ptr, &v, variables)) return;/*valid the var*/
    if (!checkEnd(ptr)) return;/*check end*/
    printComp(v);
}

/*
 * takes two complex variables from the input runs checks on each part of the command
 * adds the variables together and prints the result
 * the input is a pointer to the input string pointer and the array of complex variables
 * returns void
 */
void taskAdd(char **ptr, complex *variables) {
    complex *v1, *v2, res;
    if (!checkVar(ptr, &v1, variables)) return;/*valid the var*/
    if (!checkComma(ptr)) return;/*check for the needed commma*/
    if (!checkNoExtraComma(ptr)) return;/*check for extra commas*/
    if (!checkVar(ptr, &v2, variables)) return;/*valid the var*/
    if (!checkEnd(ptr)) return;/*check end*/
    res = addComp(v1, v2);
    printComp(&res);
}

/*
 * takes two complex variables from the input and runs checks on each part of the command
 * subtracts the variables from each other and prints the result
 * the input is a pointer to the input string pointer and the array of complex variables
 * returns void
 */
void taskSub(char **ptr, complex *variables) {
    complex *v1, *v2, res;
    if (!checkVar(ptr, &v1, variables)) return;/*valid the var*/
    if (!checkComma(ptr)) return;/*check for the needed commma*/
    if (!checkNoExtraComma(ptr)) return;/*check for extra commas*/
    if (!checkVar(ptr, &v2, variables)) return;/*check if it is a num and if it exists */
    if (!checkEnd(ptr)) return;/*check end*/
    res = subComp(v1, v2);
    printComp(&res);
}

/*
 * takes a complex variable and a real num from the input and runs checks on each part of the command
 * multiplies the variables and prints the result
 * the input is a pointer to the input string pointer and the array of complex variables
 * returns void
 */
void taskMultReal(char **ptr, complex *variables) {
    complex *v, res;
    double num;
    if (!checkVar(ptr, &v, variables)) return;/*valid the var*/
    if (!checkComma(ptr)) return;/*check for the needed commma*/
    if (!checkNoExtraComma(ptr)) return;/*check for extra commas*/
    if (!checkNum(ptr, &num)) return;/*check if it is a num and if it exists */
    if (!checkEnd(ptr)) return;/*check end*/
    res = multReal(v, num);
    printComp(&res);
}

/*
 * takes a complex variable and an imaginary num from the input and runs checks on each part of the command
 * multiplies the variables and prints the result
 * the input is a pointer to the input string pointer and the array of complex variables
 * returns void
 */
void taskMultImg(char **ptr, complex *variables) {
    complex *v, res;
    double num;
    if (!checkVar(ptr, &v, variables)) return;/*valid the var*/
    if (!checkComma(ptr)) return;/*check for the needed commma*/
    if (!checkNoExtraComma(ptr)) return;/*check for extra commas*/
    if (!checkNum(ptr, &num)) return;/*check if it is a num and if it exists */
    if (!checkEnd(ptr)) return;/*check end*/
    res = multImg(v, num);
    printComp(&res);
}

/*
 * takes two complex variables from the input and runs checks on each part of the command
 * multiplies the variables together and prints the result
 * the input is a pointer to the input string pointer and the array of complex variables
 * returns void
 */
void taskMultComp(char **ptr, complex *variables) {
    complex *v1, *v2, res;
    if (!checkVar(ptr, &v1, variables)) return;/*valid the var*/
    if (!checkComma(ptr)) return;/*check for the needed commma*/
    if (!checkNoExtraComma(ptr)) return;/*check for extra commas*/
    if (!checkVar(ptr, &v2, variables)) return;
    if (!checkEnd(ptr)) return;/*check end*/
    res = multComp(v1, v2);/*use needed command*/
    printComp(&res);
}

/*
 * takes a complex variable from the input and runs checks on each part of the command
 * then calculates its abs value and prints the num
 * the input is a pointer to the input string pointer and the array of complex variables
 * returns void
 */
void taskAbs(char **ptr, complex *variables) {
    complex *v;
    if (!checkVar(ptr, &v, variables)) return;/*valid the variable*/
    if (!checkEnd(ptr)) return;/*check the end*/
    printf("%.2f\n", absComp(v));/*use the needed command function*/
}

/*
 * checks if there is extraneous text after the stop command and exits the program
 * the input is a pointer to the input string pointer and the array of complex variables
 * returns void
 */
void taskStop(char **ptr, complex *variables) {
    if (!checkEnd(ptr)) return;/*check if no text after end*/
    exit(0);
}

/* append each command's function with its name to orginize the commands and make it compact */
typedef struct {
    char *name;
    void (*op)(char **, complex *);
} Command;

/*
 * takes the command name from the string and searches the command table to execute the matching function
 * also runs a check for illigal comma after the command in the input.
 * the input is the input string line, the array of complex variables, and the command table
 * returns void
 */
void runCommand(char *line, complex *variables, Command *cmdTable) {
    char cmdName[MAX_COMMAND_LEN];/* a place to save the inputted name*/
    char *ptr = line;
    Command *cmdPtr = cmdTable; /* better go over the commands with pointer*/
    int n = 0;
    int found = 0;

    skipSpaces(&ptr);/*if there are spaces in beggining skip to get to command*/
    if (*ptr == '\0') return;/*if nothing was entered*/

    /* go over the line to get the command name until it hit the end of row/space/, or if it is just too big to be a command then */
    while (*ptr != '\0' && !isspace(*ptr) && *ptr != ',' && n < (MAX_COMMAND_LEN-1)) cmdName[n++] = *ptr++;
    cmdName[n] = '\0';

    skipSpaces(&ptr);/*skip the spaces*/

    if (*ptr == ',') { fprintf(stderr, "Illegal comma\n"); return; }/* if there is a comma then return error */

    /* Search on the command table using a pointer */
    while (cmdPtr->name != NULL && !found) {
        if (strcmp(cmdName, cmdPtr->name) == 0) {/* compare the command name and the name in the table*/

            cmdPtr->op(&ptr, variables);/*if true send the operation to the function for the specific command*/
            found = 1;/*note that it found a command*/
        } else {
            cmdPtr++;/*if it is not the command in the table try with the next one in the table*/
        }
    }

    if (!found) fprintf(stderr, "Undefined command name\n");/*if it didnt fount return with error*/
}

int main() {
    char line[MAX_INPUT_LEN];
    complex variables[NUM_OF_VAR];
    Command cmdTable[] = {
        {"read_comp", taskRead}, {"print_comp", taskPrint},
        {"add_comp", taskAdd}, {"sub_comp", taskSub},
        {"mult_comp_real", taskMultReal}, {"mult_comp_img", taskMultImg},
        {"mult_comp_comp", taskMultComp}, {"abs_comp", taskAbs},
        {"stop", taskStop}, {NULL, NULL}
    };
    int i, n;
    for (i = 0; i < 6; i++) {
        variables[i].real = 0.0;
        variables[i].imag = 0.0;
    }/*reset the variables*/

    printf("Complex number calc\n");
    printf("Enter commands for the calculator to execute\n");
    printf("This is the command list: \n");
    printf("--------------------------------------------------\n");
    for(n=0;n<8;n++){
        printf("%s\n",cmdTable[n].name);
    }
    printf("--------------------------------------------------\n\n");

    while (!(fgets(line, sizeof(line), stdin) == NULL)) {/*execute commands until exit*/
        runCommand(line, variables, cmdTable);
    }
    return 0;
}
