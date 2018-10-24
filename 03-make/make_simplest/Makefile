OBJS := $(main.o)

hello: main.o
	gcc -o hello main.o

main.o: main.c
	gcc -c main.c

clean:
	-rm *.o hello
