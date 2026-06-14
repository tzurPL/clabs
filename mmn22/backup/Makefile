CC=gcc
CFLAGS=-Wall -ansi -pedantic -g

mycomp: mycomp.o complex.o
	$(CC) -g mycomp.o complex.o -o mycomp -lm

mycomp.o: mycomp.c complex.h
	$(CC) -c $(CFLAGS) mycomp.c -o mycomp.o

complex.o: complex.c complex.h
	$(CC) -c $(CFLAGS) complex.c -o complex.o

clean:
	rm -f *.o mycomp
