#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

// Rock-Paper-Scissors named constants
//enum RPS {
//	ERROR,
//	ROCK,
//	PAPER,
//	SCISSORS
//};

typedef int RPS;
#define ERROR		0
#define ROCK		1
#define PAPER		2
#define SCISSORS	3


const char *RPS_AsString(RPS v)
{
	switch (v) {
		case ROCK: return "rock";
		case PAPER: return "paper";
		case SCISSORS: return "scissors";
		default: return "?";
	}
}

// Function CompareRPS
// compares to RPS values and returns:
//   0 - values are equal, result draw;
//   1 - first value wins
//   2 - second value wins
//   -1 - error.
int CompareRPS(RPS v1, RPS v2)
{
	if (v1 == ERROR || v2 == ERROR) return -1;
	if (v1 == v2) return 0;
	if (v1 == ROCK) {
		if (v2 == PAPER) return 2;
		if (v2 == SCISSORS) return 1;
	} else if (v1 == PAPER) {
		if (v2 == ROCK) return 1;
		if (v2 == SCISSORS) return 2;
	} else if (v1 == SCISSORS) {
		if (v2 == ROCK) return 2;
		if (v2 == PAPER) return 1;
	}
	return -1;
}


RPS GetHumanChoise(void)
{
	char inp_buff[32];
	printf("-----------------------------------------------------------------------------\n");
	printf("Please enter your choise: r(ock) or p(aper) or s(cissors) or ENTER to quit : ");
	fflush(stdin);
	inp_buff[0] = 0;
	fgets(inp_buff, sizeof(inp_buff), stdin);
	char ch = tolower(inp_buff[0]);
	switch (ch) {
		case 'r': return ROCK;
		case 'p': return PAPER;
		case 's': return SCISSORS;
	}
	return ERROR;
}

RPS GetMachineChoise(void)
{
	int r = rand() % 3;
	switch (r) {
		case 0: return ROCK;
		case 1: return PAPER;
		return SCISSORS;
	}
	return SCISSORS;
}

void PrintResult(RPS h, RPS m, int res)
{
	const char *hs = RPS_AsString(h);
	const char *ms = RPS_AsString(m);
	printf("You choose %s, I choose %s.\n", hs, ms);
	if (res == 0) {
		printf("Draw. Try again.\n");
		return;
	}
	printf("%s win: %s beats %s.\n", 
		res == 1 ? "You" : "I",
		res == 1 ? hs : ms,
		res == 1 ? ms : hs);
}

int main(int argc, char *argv[])
{
	srand((unsigned)time(NULL));
	printf("Hello from Rock-Paper-Scissors Terminal Game!\n");
	int Human = 0;
	int Machine = 0;
	for (;;) {
		RPS h = GetHumanChoise();
		if (h == ERROR) break;
		RPS m = GetMachineChoise();
		if (m == ERROR) break;
		int res = CompareRPS(h, m);
		if (res < 0) break;
		PrintResult(h, m, res);
		if (res == 1) Human++;
		else if (res == 2) Machine++;
	}
	printf("Game is over. Final score is: Human:%u / Machine:%u. Bye!", Human, Machine);
	return 0;
}

