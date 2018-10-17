#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

// Rock-Paper-Scissors named constants
// type for player turn selection store and handling
typedef int RPS;
#define ERROR		0
#define ROCK		1
#define PAPER		2
#define SCISSORS	3

// represent RPS as string for dialog
const char *RPS_AsString(RPS v)
{
	switch (v) {
		case ROCK: return "rock";
		case PAPER: return "paper";
		case SCISSORS: return "scissors";
		default: return "?";
	}
}


int main(int argc, char *argv[])
{
	// Init Random Generator
	srand((unsigned)time(NULL));
	// Game Title
	printf("Hello from Rock-Paper-Scissors Terminal Game!\n");

	return 0;
}

