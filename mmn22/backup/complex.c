/*
 * complex.c
 * Implementation of arithmetic operations for complex numbers.
 */

#include <stdio.h>
#include <math.h>
#include "complex.h"

void printComp(complex *c) {
    printf("%.2f + (%.2f)i\n", c->real, c->imag);
}

complex addComp(complex *c1, complex *c2) {
    complex res;
    res.real = c1->real + c2->real;
    res.imag = c1->imag + c2->imag;
    return res;
}

complex subComp(complex *c1, complex *c2) {
    complex res;
    res.real = c1->real - c2->real;
    res.imag = c1->imag - c2->imag;
    return res;
}

complex multReal(complex *c, double real) {
    complex res;
    res.real = c->real * real;
    res.imag = c->imag * real;
    return res;
}

complex multImg(complex *c, double imag) {
    complex res;
    res.real = -(c->imag * imag);
    res.imag = c->real * imag;
    return res;
}

complex multComp(complex *c1, complex *c2) {
    complex res;
    res.real = (c1->real * c2->real) - (c1->imag * c2->imag);
    res.imag = (c1->real * c2->imag) + (c1->imag * c2->real);
    return res;
}

double absComp(complex *c) {
    return sqrt((c->real * c->real) + (c->imag * c->imag));
}
