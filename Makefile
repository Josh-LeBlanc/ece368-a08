a8: main.c
	gcc -O3 -std=c99 -Wall -Wshadow -Wvla -pedantic *.c -o a8

dbg: main.c
	gcc -g -O3 -std=c99 -Wall -Wshadow -Wvla -pedantic *.c -o a8
	gdb a8

dbgn: main.c
	gcc -g -std=c99 -Wall -Wshadow -Wvla -pedantic *.c -o a8

test: main.c
	gcc -O3 -std=c99 -Wall -Wshadow -Wvla -pedantic *.c -o a8
	./a8 testcases/ex1
