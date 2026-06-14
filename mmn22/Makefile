mycomp: mycomp.o complex.o parser.o
	gcc -Wall -ansi -pedantic -g mycomp.o complex.o parser.o -o mycomp -lm

mycomp.o: mycomp.c complex.h parser.h
	gcc -c -Wall -ansi -pedantic -g mycomp.c -o mycomp.o

complex.o: complex.c complex.h
	gcc -c -Wall -ansi -pedantic -g complex.c -o complex.o

parser.o: parser.c parser.h complex.h
	gcc -c -Wall -ansi -pedantic -g parser.c -o parser.o

clean:
	rm -f *.o mycomp
