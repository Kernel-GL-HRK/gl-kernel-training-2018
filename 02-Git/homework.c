#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char read_user()
{
	printf("Please choose: rock (r) - paper (p) - scissors (s)\n");
	char user;
	while(user != 'r' && user != 'p' && user != 's')
		scanf("%c", &user);

	return user;
} 

char rnd()
{
	char arr[]={'r','p','s'};
	
	srand(time(NULL));
	int rnd =  rand()%3;
	
	return arr[rnd];
}

char *conver_char(char c)
{
	if (c == 'r') return "rock";       
	if (c == 's') return "scissors";       
	if (c == 'p') return "paper"; 
}

void result(char user, char comp)
{
	
	printf("You choose %s, I choose %s\n", 
		conver_char(user), conver_char(comp));
		
	if(user == comp)
		printf("Draw:\n");
		else if ((user == 'r') && (comp == 's') ||
			(user == 's') && (comp == 'p') 	||
			(user == 'p') && (comp == 'r'))	
				printf("You Win: %s beats %s\n",
					conver_char(user), conver_char(comp));
			else	
				printf("You Lose: %s beats %s\n",
					conver_char(comp), conver_char(user));				
}

int main()
{

	char user = read_user();
	char comp = rnd();

	result(user, comp);
		
	return 0;	
}
