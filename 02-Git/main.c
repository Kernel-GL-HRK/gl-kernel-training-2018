#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

enum userChoice
{
        EPAPER,
        ROCK,
        SCISSORS,
        FINISHGAME
};

enum winner
{
        USER,
        COMPUTER,
        BOTH
};

int get_user_choice()
{
        int result = FINISHGAME;

        while(1){
		printf("enter your choice: \n");
		printf("\t Rock:\t\t'R'\n");
		printf("\t Paper:\t\t'P'\n");
		printf("\t Scissors:\t\t'S'\n");
		printf("\n\t Finish game 'F'\n");

		char userAnswer;
		while( (userAnswer = getchar()) && isspace(userAnswer))
		     ;

		 getchar(); //fetch \n from stream buf	
		
		 switch ( toupper(userAnswer)){
		 case 'R':
		     result = ROCK;
		     break;
		 case 'P':
		     result = EPAPER;
		     break;
		 case 'S':
		     result = SCISSORS;
		     break;
		 case 'F':
		     result = FINISHGAME;
		     break;
		 default:
		     printf("incorrect output %c", result);
		     continue;
		 };
		return result;
	}
}

int get_computer_choice()
{
	return rand() % (int)FINISHGAME;
}

int determine_winner(const int userChoice,const int computerChoice)
{
	static int solution_map[3][3] = {
		BOTH,		USER,		COMPUTER,
		COMPUTER,	BOTH,		USER,
		USER,		COMPUTER,	BOTH		
	};
	return solution_map[userChoice][computerChoice];
}

void print_choice( int choice);
void print_winner( int userChoice, int computerChoice, int winner)
{
    printf("\n\n==========================================\n");
    printf("\t your's ");
    print_choice(userChoice);
    printf("\tvs\t computer's ");
    print_choice(computerChoice);
    printf("\n\n winner:");

    switch(userChoice){
        case USER:
            printf("you win!");
            break;
        case COMPUTER:
            printf("computer win!");
            break;
        case BOTH:
            printf("dead heat!");
            break;
    }
    printf("\n\n==========================================\n");
}

void print_choice( int choice)
{
    switch(choice){
        case SCISSORS:
            printf("Scissors");
            break;
        case ROCK:
            printf("Rock");
            break;
        case EPAPER:
            printf("Paper");
            break;
    }

}

int main(void)
{
	srand(time(NULL));

	int userChoice = FINISHGAME;
	printf("This is console implementation for Rock-Paper-Scissors game: \n");

	while ((userChoice = get_user_choice())!= FINISHGAME){
		int computerChoice = get_computer_choice();

		int winner = determine_winner(userChoice, computerChoice);
		print_winner(userChoice, computerChoice,  winner);
	}

	return 0;
}
