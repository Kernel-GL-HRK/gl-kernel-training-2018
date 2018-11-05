all: hello

hello: main.o
	gcc -o main.o hello -lc

main.o: main.c
	gcc -c main.c -o main.o

clean:
	rm -rf *.o hello

