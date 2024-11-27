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

mem:
	gcc -O3 -std=c99 -Wall -Wshadow -Wvla -pedantic *.c -o a8
	valgrind --tool=memcheck --leak-check=full --verbose ./a8 testcases/2

test2: main2.c
	gcc -O3 -std=c99 -Wall -Wshadow -Wvla -pedantic main2.c -o a8
	./a8 testcases/ex1

dbg2: main2.c
	gcc -g -O3 -std=c99 -Wall -Wshadow -Wvla -pedantic main2.c -o a8
	gdb a8

dbgn2: main2.c
	gcc -g -std=c99 -Wall -Wshadow -Wvla -pedantic main2.c -o a8
	gdb a8

mem2: main2
	gcc -O3 -std=c99 -Wall -Wshadow -Wvla -pedantic main2.c -o a8
	valgrind --tool=memcheck --leak-check=full --verbose ./a8 testcases/ex1
