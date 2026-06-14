all: remove_blanks/remove_blanks turn_on/turn_on

remove_blanks/remove_blanks: remove_blanks/remove_blanks.c
	gcc -Wall -ansi -pedantic -g -o remove_blanks/remove_blanks remove_blanks/remove_blanks.c

turn_on/turn_on: turn_on/turn_on.c
	gcc -Wall -ansi -pedantic -g -o turn_on/turn_on turn_on/turn_on.c

# Rule to easily remove compiled files
clean:
	rm -f remove_blanks/remove_blanks turn_on/turn_on