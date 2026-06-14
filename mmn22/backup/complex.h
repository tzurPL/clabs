/*
 * complex.h
 * Definition of the complex number structure and prototypes for arithmetic operations.
 */

#ifndef COMPLEX_H
#define COMPLEX_H

#define MAX_COMMAND_LEN 50
#define MAX_INPUT_LEN 1000
#define NUM_OF_VAR 6

/* Define the complex number structure */
typedef struct {
    double real;
    double imag;
} complex;

/* Function prototypes for complex number operations */

/* Prints the complex number in the format: real + (imag)i */
void printComp(complex *c);

/* Adds two complex numbers and returns the result */
complex addComp(complex *c1, complex *c2);

/* Subtracts the second complex number from the first and returns the result */
complex subComp(complex *c1, complex *c2);

/* Multiplies a complex number by a real scalar and returns the result */
complex multReal(complex *c, double real);

/* Multiplies a complex number by an imaginary scalar and returns the result */
complex multImg(complex *c, double imag);

/* Multiplies two complex numbers and returns the result */
complex multComp(complex *c1, complex *c2);

/* Returns the absolute value (magnitude) of a complex number */
double absComp(complex *c);

#endif /* COMPLEX_H */
