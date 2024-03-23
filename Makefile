wish: main.o shell.o commands.o error.o
	gcc -o wish main.o shell.o commands.o error.o -Wall

main: main.c
	gcc -c main.c -Wall

shell: shell.c
	gcc -c shell.c -Wall

commands: commands.c
	gcc -c commands.c -Wall

error: error.c
	gcc -c error.c -Wall
