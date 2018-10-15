#include <stdio.h>
#include <stdlib.h>

enum items {
	ITEMS_ROCK,
	ITEMS_PAPER,
	ITEMS_SCISSORS,
	ITEMS_COUNT
};

enum game_states {
	GAME_STATES_WIN,
	GAME_STATES_LOSE,
	GAME_STATES_DRAW
};

static enum items get_user_choice(void)
{
	int c;
	c = getchar();
	c = tolower(c);
	
	switch(c) {
	case 'r':
		return ITEMS_ROCK;
	case 'p':
		return ITEMS_PAPER;
	case 's':
		return ITEMS_SCISSORS;
	break;
	default:
		printf("Err: Unknown choise");
		exit(1);
	}
}

static enum items get_ai_choice(void)
{
	return rand() % ITEMS_COUNT;
}

static enum game_states check_choices(enum items user_choice, enum items ai_choice)
{
	if (user_choice == ai_choice)
		return GAME_STATES_DRAW;
		
	if (user_choice == ITEMS_ROCK && ai_choice == ITEMS_SCISSORS)
		return GAME_STATES_WIN;
	else if (user_choice == ITEMS_SCISSORS && ai_choice == ITEMS_PAPER)
		return GAME_STATES_WIN;
	else if (user_choice == ITEMS_PAPER && ai_choice == ITEMS_ROCK)
		return GAME_STATES_WIN;
		
 	return GAME_STATES_LOSE;
}

static void print_game_state_msg( enum game_states state)
{
	switch( state ) {
	case GAME_STATES_WIN:
		printf("You win!\n");
	break;
	case GAME_STATES_LOSE:
		printf("You lose!\n");
	break;
	case GAME_STATES_DRAW:
		printf("Draw\n");
	break;
	};
}

void start_game(void)
{
	enum items user_choice; 
	enum items ai_choice;
	enum game_states state;
	
	printf("Hello welcome to rock_paper_scissors game!\n");
	printf("Please make your choise: r(rock), p(paper), s(scissors)!\n");
	
	user_choice = get_user_choice();
	ai_choice = get_ai_choice();
	state = check_choices(user_choice, ai_choice);
	print_game_state_msg(state);
	
}


int main(int argc, char *argv[])
{
	start_game();
	return 0;
}
