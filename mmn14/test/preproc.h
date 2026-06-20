#ifndef PREPROC_H
#define PREPROC_H

#include "globals.h"
#include "table.h"

/* preprocess func: runs the preprocessor phase on the input file, expanding macros and removing comments/empty lines. Returns boolean for success/fail and output macros. */
boolean preprocess(const char *filename, MacroNode **outMacros);

#endif
