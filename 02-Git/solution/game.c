#include <stdio.h>
#include <stdlib.h>
#include <time.h>

enum game_element {
	ROCK,
	SCISSORS,
	PAPER,
	INVAL_ELEM
};


static const enum game_element beats[3] = {PAPER, ROCK, SCISSORS};

static void show_game_result(enum game_element my, enum game_element human)
{
	if (my == human)
		printf("Draw\n");
	else if (beats[my] == human)
		printf("You win\n");
	else
		printf("I win\n");
}

static const char *element_to_str(enum game_element e)
{
	static const char * const el_names[] = {"rock", "scissors", "paper"};
	return el_names[e];
}

static enum game_element char_to_element(char c)
{
	switch (c) {
	case 'r':
		return ROCK;
	case 's':
		return SCISSORS;
	case 'p':
		return PAPER;
	default:
		return INVAL_ELEM;
	}

}

static enum game_element generate_rand_element(void)
{
	srand((unsigned int) time(NULL));
	const int upper_bound = INVAL_ELEM;

	return (rand() % upper_bound);
}

static char get_user_input(void)
{
	char user_select;

	printf("Please choose: rock (r) - paper (p) - scissors (s)\n");
	scanf("%c", &user_select);
	return user_select;
}

int main(void)
{
	enum game_element user_select =  char_to_element(get_user_input());

	if (user_select == INVAL_ELEM) {
		printf("Invalid user input\n");
	} else {
		printf("You choose %s\n", element_to_str(user_select));

		enum game_element pc_choice = generate_rand_element();

		printf("I chose %s\n", element_to_str(pc_choice));
		show_game_result(pc_choice, user_select);
	}
	return 0;
}

