#include <ctype.h>
#include <stdio.h>

#define MAX_LEN 100

/* remove_blanks:
 * this function takes in a string and remove all the blank spaces:
 * it removes tabs, spaces and the newline by going over the string with 2 pointers
 * p1 goes over the string and puts all characters that are not spaces
 * back into the array using p2 and adds a \0 at the end.
*/
void remove_blanks(char str[]);

int main(void) {
    char str[MAX_LEN];
    int ch;
    int i = 0;
    while ((ch = getchar()) != '\n' && ch != EOF && i < MAX_LEN - 1) {
        str[i] = ch;
        i++;
    }
    str[i] = '\0';
    remove_blanks(str);
    return 0;
}

void remove_blanks(char str[]){
    int p1 = 0;
    int p2 = 0;

    printf("The Input string: \n\"%s\"\n",str);
    printf("The string as received by the function: \n\"%s\"\n", str);

    while(str[p1]!='\0'){
        if(!isspace(str[p1])) {
            /*if not a space then copy it to the writing pointer pos*/
            str[p2]=str[p1];

            /*move the writing pointer*/
            p2++;
        }
        /*continue going over the array*/
        p1++;
    }
    str[p2]='\0';
    printf("The string at the end of the function: \n\"%s\"\n",str);
}
