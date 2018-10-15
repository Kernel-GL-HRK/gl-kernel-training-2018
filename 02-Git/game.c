#include <stdio.h>

int main()
{
	char answer=0;
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
	printf("You choose %s, I choose rock\n",player);
	printf("I win: paper beats rock\n");
	return 0;
}
