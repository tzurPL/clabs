#include <stdio.h>
#include <limits.h>

#define NUM_BITS (sizeof(long) * CHAR_BIT)/*how many bits are in a long*/

/*turn_on:
 *gets a whole long and prints how it will look in binary form(using print_binary) then
 *turns on the 16th bit and then prints the new number in decimal and binary form.
 *num is the input
 */
void turn_on(long num);

/*print_binary:
 *get a long and using a mask it goes over the amount of bits in a long and prints out each bit in the num
 */
void print_binary(long num);

int main(void) {
    long num;
    if (scanf("%li", &num) == 1) {
        turn_on(num);
    }
    return 0;
}

void print_binary(long num) {
    /*go over the amount of bits in a long*/
    int i = NUM_BITS - 1;
    for (; i >= 0; i--) {
        /*go to the next bit in the number via shifting right and using a mask printing if the bit is 1 or 0*/
        if ((num >> i) & 1L) {
            putchar('1');
        } else {
            putchar('0');
        }
    }
    putchar('\n');
}

void turn_on(long num) {
    printf("The number before the change: \n%ld\n", num);
    print_binary(num);/*print the original num in binary*/

    /*shifting a long with 1 bit turned on to the 16th pos to the left*/
    /*turning it on in the num via comparing with the Or operation.*/
    num = num | (1L << 16);

    printf("The number after the change: \n%ld\n", num);
    print_binary(num);/*print the number after the change in binary*/
}


