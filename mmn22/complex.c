/*
 * complex.c
 * maman22
 */

#include <stdio.h>
#include <math.h>
#include "complex.h"

/*
 * prints the complex number to the screen
 * the input is a pointer to the complex number
 * returns void
 */
void printComp(complex *c) {
    printf("%.2f + (%.2f)i\n", c->real, c->imag);
}

/*
 * adds two complex numbers together
 * the input is pointers to two complex numbers
 * returns the result as a complex number
 */
complex addComp(complex *c1, complex *c2) {
    complex res;
    res.real = c1->real + c2->real;
    res.imag = c1->imag + c2->imag;
    return res;
}

/*
 * subtracts the second complex number from the first
 * the input is pointers to the two complex numbers
 * returns the result as a complex number
 */
complex subComp(complex *c1, complex *c2) {
    complex res;
    res.real = c1->real - c2->real;
    res.imag = c1->imag - c2->imag;
    return res;
}

/*
 * multiplies a complex number by a real num
 * the input is a pointer to the complex number and a double representing the real num
 * returns the result as a complex number
 */
complex multReal(complex *c, double real) {
    complex res;
    res.real = c->real * real;
    res.imag = c->imag * real;
    return res;
}

/*
 * multiplies a complex number by an imaginary num value
 * the input is a pointer to the complex number and a double that is the imaginary num
 * returns the result as a complex number
 */
complex multImg(complex *c, double imag) {
    complex res;
    res.real = -(c->imag * imag);
    res.imag = c->real * imag;
    return res;
}

/*
 * multiplies two complex numbers together
 * the input is pointers to the two complex numbers
 * returns the result as a complex number
 */
complex multComp(complex *c1, complex *c2) {
    complex res;
    res.real = (c1->real * c2->real) - (c1->imag * c2->imag);
    res.imag = (c1->real * c2->imag) + (c1->imag * c2->real);
    return res;
}

/*
 * finds the absulute value of the complex num
 * the input is a pointer to the complex number
 * returns the absulute value of the complex num in a new complex num
 */
double absComp(complex *c) {
    return sqrt((c->real * c->real) + (c->imag * c->imag));
}
