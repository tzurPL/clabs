/*
 * the preprocessor step of the assembler
 * reads the og .as file, finds macro definitions stores them and expands macro calls in the output.
 * checks macro names and outputs the .am file with all macros expanded
 * if errors are found the .am file not created
 */
#ifndef PREPROC_H
#define PREPROC_H

#include "globals.h"
#include "table.h"

/*
 * preprocess func
 * runs the preprocessor on the input file expanding macros and removing comments and empty lines.
 * the input is the filename and a pointer to store the generated macro list.
 * returns a boolean indicating whether the preprocessing succeeded without errors.
 */
boolean preprocess(const char *filename, MacroNode **outMacros);

#endif
