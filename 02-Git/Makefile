# makefile config
# compiller
CC = gcc
CFLAGS  = -g -Wall
COBJ = -c
# final destination file name
SOURCES=main.cpp Driver.cpp Move.cpp Verbose.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=RockPaperScissors
# makefile instructions
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

main.o: main.cpp
	$(CC) $(CFLAGS) $(COBJ) main.cpp

Verbose.o: Verbose.cpp
	$(CC) $(CFLAGS) $(COBJ) Verbose.cpp

Driver.o: Driver.cpp
	$(CC) $(CFLAGS) $(COBJ) Driver.cpp

Move.o: Move.cpp
	$(CC) $(CFLAGS) $(COBJ) Move.cpp

clean:
	rm -rf *.o