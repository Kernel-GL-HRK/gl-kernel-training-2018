#include <stdio.h>

enum game_element {
	ROCK,
	SCISSORS,
	PAPER,
	INVAL_ELEM
};

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

	if (user_select == INVAL_ELEM)
		printf("Invalid user input\n");
	else
		printf("User selected %s\n", element_to_str(user_select));

	return 0;
}

