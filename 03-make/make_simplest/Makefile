# blank line is better to be removed
hello: main.o
	gcc -o hello main.o -lc
main.o:
#main.c -removed from 'main.o: main.c' line
	gcc -c main.c -o main.o
clean:
	rm -f *.o hello 
# a.out - removed from line above, since the is no such file cread afrer make
# removed '-' before rm and aded '-f' key afrer rm
#changed main to hello
