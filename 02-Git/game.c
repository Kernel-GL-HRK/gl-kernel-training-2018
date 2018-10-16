#include <stdio.h>
#include <stdlib.h>

void result (char weapon, _Bool win)
{
	char * output;
	if(win)
	{
		switch (weapon)
		{
			case 'r':
				printf(" I choose scissors\n");
				output = "You win: rock beats scissors\n";
				break;
			case 'p':
				printf(" I choose rock\n");
				output = "You win: paper beats rock\n";
				break;
			case 's':
				printf(" I choose paper\n");
				output = "You win: scissors beats paper\n";
				break;
		}
	}
	else
	{
		switch (weapon)
		{
			case 's':
				printf(" I choose rock\n");
				output = "I win: rock beats scissors\n";
				break;
			case 'r':
				printf(" I choose paper\n");
				output = "I win: paper beats rock\n";
				break;
			case 'p':
				printf(" I choose scissors\n");
				output = "I win: scissors beats paper\n";
				break;
		}
	}
	printf(output);
}

int main()
{
	time_t t;
	srand((unsigned) time(&t));
	char answer=0;
	_Bool win = rand()%2;

	printf("Please choose: rock (r) - paper (p) - scissors (s)\n");
	scanf("%c",&answer);
	char * player;
	switch (answer)
	{
		case 'r':
			player = "rock";
			break;
		case 'p':
			player = "paper";
			break;
		case 's':
			player = "scissors";
			break;
		default:
			printf("Error char\n");
			return 1;
	}
	printf("You choose %s,",player);
	result(answer, win);
	return 0;
}
