CXX = cc
CXXFLAGS = -Wall -Wextra -pedantic-errors -O3 -std=c99
OBJ = main.o
SRC = $(wildcard src/*.c)
EXEC = game

main: $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $(EXEC)
	rm -rf *.o

%.o: $(SRC)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf *.o $(EXEC)
