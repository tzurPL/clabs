/* Maman 12 - Tzur Pinto Lazar */

#include <stdio.h>
#include "difference_and_sum.h"

/*
 * findDifferenceAndSum func
 * Calculates the sum and absolute difference of two integers
 * and stores the results in the addresses it got.
 */
void findDifferenceAndSum(int num, int num1, int *diff, int *sum) {
    *sum = num + num1;
    *diff = abs(num - num1);
}



/*
 * saveArray func
 * Saves a pair of integers into a dynamic array.
 * Reallocates memory if the capacity is reached.
 * it's input is a pointer to the array,
 * and pointers to the max capacity and the size of the array
 * and the 2 integers that make the pair that the function adds to the array
 * at last it puts the pair in the array and does the reallocation action if needed.
 */
int* saveArray(int *arr, int *cap, int *size, int num, int num1) {
	/*check if there is enough space for another pair
	by checking if the current size plus the size of a pair will fit in the capacity*/
    if (*size + 2 > *cap) {
        if (*cap == 0) {*cap = INIT_CAP;}/*set the capacity in the first run to the initial capacity*/
        else{*cap *= 2;}/*in any other run double the capacity*/

		/*try to reallocate space for the array to size it up to the new capacity*/
        if (!(arr = (int*)realloc((void*)arr, *cap * sizeof(int)))) {/*multypling the capacity by the size of int*/
			/*if we dont have enough space for the reallocation then fail*/
            printf("Not enough memory.\n");
            free(arr);
            exit(1);
        }
    }

	/*add the pair to the array*/
    arr[*size] = num;
    arr[*size+1] = num1;
    *size += 2;/*add the size of a pair to the counter size of the array*/

    return arr;
}

int main(void) {
    int num, num1, sum, diff;/*the number pair and the holders for the sum and diff*/

	/*dynamic array */
    int *arr = NULL;/*the array pointer*/
    int size = 0;/*the size used in the array*/
    int cap = 0;/*the max size of the array*/
    int i;/*itirator for the loop over the array*/

    printf("Enter pairs of integers in this format: {a, b} , use the space or new line to seperate each pair.\n ");
    printf("Use Enter and then Ctrl D or Ctrl Z to exit.\n");

    while (scanf("%d, %d", &num, &num1) == 2) {/*scan the pairs*/

        arr = saveArray(arr, &cap, &size, num, num1);/*save the pair to the array*/

		printf("The original numbers received are: %d, %d\n", num, num1);

		/*copy the pair to temp variables for the function*/
		diff = num;
		sum = num1;

		/*run the function for diff and sum*/
        printf("\nCalling function findDifferenceAndSum : \n");
        findDifferenceAndSum(diff, sum, &diff, &sum);
        printf("The difference is: %d\nThe sum is: %d\n", diff, sum);

		/*calculate the diff and sum via the macros*/
        printf("\nCalling macro DIFF : \n");
        printf("The difference is : %d\n", DIFF(num, num1));
		printf("\nCalling macro SUM : \n");
        printf("The sum is : %d\n\n", SUM(num, num1));

    }

	/*printing the pairs*/
    printf("The pairs that have been entered are:\n");
    if (size == 0) {/*if the array is at 0 size*/
        printf("No pairs were entered.\n");
    }
    else {
        for (i = 0; i < size; i+=2) {/*go over the array(growing i by 2 because im counting the pairs not individual ints)*/
            printf("%d, %d\n", arr[i], arr[i+1]);
        }
    }

    free(arr);/*finally free the array and the memory allocated*/

    return 0;
}

