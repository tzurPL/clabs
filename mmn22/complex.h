/*
 * complex.h
 * maman22 Tzur Pinto Lazar
 */

#ifndef COMPLEX_H
#define COMPLEX_H

#define MAX_COMMAND_LEN 50
#define MAX_INPUT_LEN 1000
#define NUM_OF_VAR 6

/* the complex number struct*/
typedef struct {
    double real;
    double imag;
} complex;

/*
 * prints the complex number to the screen
 * the input is a pointer to the complex number
 * returns void
 */
void printComp(complex *c);

/*
 * adds two complex numbers together
 * the input is pointers to two complex numbers
 * returns the result as a complex number
 */
complex addComp(complex *c1, complex *c2);

/*
 * subtracts the second complex number from the first
 * the input is pointers to the two complex numbers
 * returns the result as a complex number
 */
complex subComp(complex *c1, complex *c2);

/*
 * multiplies a complex number by a real num
 * the input is a pointer to the complex number and a double representing the real num
 * returns the result as a complex number
 */
complex multReal(complex *c, double real);

/*
 * multiplies a complex number by an imaginary num value
 * the input is a pointer to the complex number and a double that is the imaginary num
 * returns the result as a complex number
 */
complex multImg(complex *c, double imag);

/*
 * multiplies two complex numbers together
 * the input is pointers to the two complex numbers
 * returns the result as a complex number
 */
complex multComp(complex *c1, complex *c2);

/*
 * finds the absulute value of the complex num
 * the input is a pointer to the complex number
 * returns the absulute value of the complex num in a new complex num
 */
double absComp(complex *c);
#endif /* COMPLEX_H */
