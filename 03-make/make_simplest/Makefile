all: hello

hello: main.o
	gcc -o hello main.o -Wall -lc

main.o: main.c
	gcc -c main.c -Wall

clean:
	rm *.o hello
