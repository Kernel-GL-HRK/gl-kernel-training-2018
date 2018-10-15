CXX = cc
CXXFLAGS = -Wall -Wextra -pedantic-errors -O3 -std=c99
OBJ = main.o
EXEC = game

main: $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $(EXEC)

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf *.o $(EXEC)
