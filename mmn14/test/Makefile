assembler: assembler.o util.o table.o preproc.o first_pass.o second_pass.o errors.o
	gcc -Wall -ansi -pedantic -g assembler.o util.o table.o preproc.o first_pass.o second_pass.o errors.o -o assembler

assembler.o: assembler.c
	gcc -Wall -ansi -pedantic -g -c assembler.c -o assembler.o

util.o: util.c
	gcc -Wall -ansi -pedantic -g -c util.c -o util.o

table.o: table.c
	gcc -Wall -ansi -pedantic -g -c table.c -o table.o

preproc.o: preproc.c
	gcc -Wall -ansi -pedantic -g -c preproc.c -o preproc.o

first_pass.o: first_pass.c
	gcc -Wall -ansi -pedantic -g -c first_pass.c -o first_pass.o

second_pass.o: second_pass.c
	gcc -Wall -ansi -pedantic -g -c second_pass.c -o second_pass.o

errors.o: errors.c
	gcc -Wall -ansi -pedantic -g -c errors.c -o errors.o

clean:
	rm -f *.o assembler *.am *.ob *.ent *.ext
