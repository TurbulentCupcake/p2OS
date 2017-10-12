# the compiler: gcc for C program, define as g++ for C++
all: mysh.c
	gcc -g -Wall -o mysh mysh.c -O

clean:
	$(RM) mysh
