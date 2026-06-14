

#include <stdio.h>
#include <stdlib.h>

/*\
 * a funct that takes a file name and an int
 * and tries to print the char in the file at the int place
 */
void seek(int place, const char *fname);

int main(int argc, char **argv){
    int i; long place; char *ph;

    if (argc == 1){/*if args were not provided */
        fprintf(stderr, "\nMissing Parameters, Usage: ./seek {place} {files}");
        exit(EXIT_FAILURE);}

    if(!(place = strtol(argv[1], &ph, 10))){
        fprintf(stderr, "first param is NEEDED TO BE A NUM!");/*try to save the first arg to a var*/
        exit(EXIT_FAILURE);}
    if(place<=0){/*if the place given is invalid*/
        fprintf(stderr, "The wanted place of a char in the files has to be positive!");
        exit(EXIT_FAILURE);
    }
    if (argc >= 2) {/*run through the files and run the function for it*/
        for (i = 2; i < argc; i++){
            seek(place, argv[i]);
        }
    }
    return 0;
}

void seek(int place, const char *fname){
    FILE *f;/*var to open the file for the given file name*/
    if (!(f = fopen(fname, "r"))) {
        /* If the file doesn't exist or can't be opened or argument is not a file, print an error and skip */
        fprintf(stderr, "Cannot open file or argument is not a file'%s'.\n", fname);
        return;
    }if (f==NULL) {
        /* If the file doesn't exist or can't be opened, print an error and skip */
        fprintf(stderr, "Cannot open file '%s'.\n", fname);
        fclose(f);
        return;
    }
    /* use fseek to go to the specified character at the place inputted from the beginning of the file */
    if (fseek(f, place-1, SEEK_SET) == 0) {
        int ch = fgetc(f);/*read char*/
        /*if the place has a char in the file print it if not EOF */
        if (ch != EOF) {
            printf("%c\n", ch);
            fclose(f);
            return;
        } else {
            fprintf(stderr, "EOF or error reading %s\n", fname);
            fclose(f);
            return;
        }
    } else {
        fprintf(stderr,"Seek failed on %s\n", fname);
        fclose(f);
        return;
    }
    /* close file */
    fclose(f);
}
