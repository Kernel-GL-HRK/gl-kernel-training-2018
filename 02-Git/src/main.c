/**
 * Hometask #1
 *
 * Create a rock-paper-scissors console game
 * (rules https://en.wikipedia.org/wiki/Rock%E2%80%93paper%E2%80%93scissors)
 *
 * The game should be simple, the goal is to create a git project with proper
 * commit order and structure. E.g.:
 * ```
 * > Please choose: rock (r) - paper (p) - scissors (s)
 * > r
 * > You choose rock, I choose paper
 * > I win: paper beats rock
 * ```
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

// If changing the order, change get_winner table and shape2str list
enum shape {
	ROCK,
	PAPER,
	SCISSORS,
	MAX_SHAPE
};

/* Returns 1 if the 1st index wins, -1 if the 2nd, 0 if it's a draw
 *    r  p  s
 * r  0 -1  1
 * p  1  0 -1
 * s -1  1  0
 */
const int get_winner[MAX_SHAPE][MAX_SHAPE] = {
	{ 0, -1,  1},
	{ 1,  0, -1},
	{-1,  1,  0}
};

static char *shape2str[] = { "rock", "paper", "scissors" };

static enum shape get_user_shape()
{
	char ch;

	while (true) {
		printf("> Please choose: rock (r) - paper (p) - scissors (s): ");
		ch = getchar();
		// Skip the garbage. So far, we don't handle it.
		while (ch != '\n' && getchar() != '\n');

		switch (ch) {
		case 'r':
		case 'R':
			return ROCK;
		case 'p':
		case 'P':
			return PAPER;
		case 's':
		case 'S':
			return SCISSORS;
		default:
			printf("Incorrect input (%c)!\n", ch);
			break;
		}
	}
}

static enum shape get_pc_shape()
{
	return rand() % MAX_SHAPE;
}

static void again_query()
{
	char ch;

	while (true) {
		printf("Play again (y/n)? ");
		ch = getchar();
		// Skip the garbage. So far, we don't handle it.
		while (ch != '\n' && getchar() != '\n');

		switch (ch) {
		case 'n':
		case 'N':
			puts("Thanks for the game! Bye!");
			exit(0);
		case 'y':
		case 'Y':
			return;
		default:
			printf("Incorrect answer (%c)!\n", ch);
			break;
		}
	}
}

void find_winner(enum shape user, enum shape pc)
{
	int winner = get_winner[pc][user];

	printf("You choose %s, I choose %s\n", shape2str[user], shape2str[pc]);
	if (winner > 0)
		printf("I win: %s beats %s\n", shape2str[pc], shape2str[user]);
	else if (winner < 0)
		printf("You win: %s beats %s\n", shape2str[user], shape2str[pc]);
	else
		printf("It\'s a draw!\n");
}

static void play_game()
{
	enum shape user;
	enum shape pc;

	puts("\'\'\'");
	user = get_user_shape();
	pc = get_pc_shape();
	find_winner(user, pc);
	puts("\'\'\'");
}

static void play()
{
	play_game();
	again_query();
}

int main()
{
	// Set the seed for PC player
	srand(time(NULL));

	while (true)
		play();
}

