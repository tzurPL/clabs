
#ifndef DIFFERENCE_AND_SUM_H
#define DIFFERENCE_AND_SUM_H

#include <stdlib.h>

/* Macros for sum and abs difference */
#define SUM(a, b) ((a) + (b))
#define DIFF(a, b) (abs((a) - (b)))

/* initial array capacity */
#define INIT_CAP 2

/*
 * findDifferenceAndSum func
 * Calculates the sum and absolute difference of two integers
 * and stores the results in the addresses it got.
 */
void findDifferenceAndSum(int num, int num1, int *diff, int *sum);

/*
 * saveArray func
 * Saves a pair of integers into a dynamic array.
 * Reallocates memory if the capacity is reached.
 * it's input is a pointer to the array,
 * and pointers to the max capacity and the size of the array
 * and the 2 integers that make the pair that the function adds to the array
 * at last it puts the pair in the array and does the reallocation action if needed.
 */
int* saveArray(int *arr, int *cap, int *size, int num, int num1);

#endif /* DIFFERENCE_AND_SUM_H */