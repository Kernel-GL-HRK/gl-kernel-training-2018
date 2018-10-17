#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define INTRO_STRING "> Please choose: rock (r) - paper (p) - scissors (s)\n> "
#define ROCK_STRING "rock"
#define PAPER_STRING "paper"
#define SCISSORS_STRING "scissors"
#define ANSWER_STRING "> You choose %s, I choose %s\n"
#define ANSWER2_STRING "> I %s: %s beats %s\n"
#define ANSWER3_STRING "> Draw: we both choose %s\n"
#define WRONG_INPUT_STRING "> Wrong input, you have %d retries\n"
#define WON_STRING "won"
#define LOSE_STRING "lose"

const char *selectionArray[] = {ROCK_STRING, PAPER_STRING, SCISSORS_STRING};

const int DBG = 0;

int main(int argc, char *argv[])
{
	char input = 0;
	int retrycount = 3, playerChoose = -1, compChoose = -1;

	srand(time(NULL));

	do {
		printf(INTRO_STRING);
		scanf(" %c", &input);
		if (DBG)
			printf("input %c\n", input);
		switch (input) {
		case 'r':
			playerChoose = 0;
			break;
		case 'p':
			playerChoose = 1;
			break;
		case 's':
			playerChoose = 2;
			break;
		default:
			printf(WRONG_INPUT_STRING, retrycount);
			break;
		}
		if (DBG)
			printf("playerChoose %d", playerChoose);
		if (playerChoose >= 0)
			break;
		retrycount--;
	} while (retrycount >= 0);
	if (retrycount < 0 && playerChoose < 0)
		goto end;

	compChoose = rand() % 3;

	printf(ANSWER_STRING, selectionArray[playerChoose],
	       selectionArray[compChoose]);
	if (compChoose > playerChoose
	    && !(compChoose == 2 && playerChoose == 0)) {
		printf(ANSWER2_STRING, WON_STRING, selectionArray[compChoose],
		       selectionArray[playerChoose]);
	} else if (compChoose == playerChoose) {
		printf(ANSWER3_STRING, selectionArray[playerChoose]);
	} else {
		printf(ANSWER2_STRING, LOSE_STRING,
		       selectionArray[playerChoose],
		       selectionArray[compChoose]);
	}
end:
	return 0;
}
