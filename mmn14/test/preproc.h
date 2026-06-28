/*
 * preproc.h
 * mmn14
 * Tzur Pinto Lazar
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
