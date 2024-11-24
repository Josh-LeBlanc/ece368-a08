a8: main.c
	gcc -O3 -std=c99 -Wall -Wshadow -Wvla -pedantic *.c -o a8

test: main.c
	gcc -O3 -std=c99 -Wall -Wshadow -Wvla -pedantic *.c -o a8
	./a8 testcases/ex1
