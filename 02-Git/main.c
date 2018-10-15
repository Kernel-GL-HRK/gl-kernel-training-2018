#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>

const char VARIANTS[] = {'r', 'p', 's'};

bool checkVictory(char firstChoice, char secondChoice)
{
	if (firstChoice == 'r' && secondChoice == 's')
		return true;
	else if (firstChoice == 's' && secondChoice == 'p')
		return true;
	else if (firstChoice == 'p' && secondChoice == 'r')
		return true;

	return false;
}

const char *getDescription(char choice)
{
	switch (choice) {
	case 'r': return "rock";
	case 'p': return "paper";
	case 's': return "scissors";
	}

	return NULL;
}

bool isValidChoice(char choice)
{
	for (int i = 0; i < sizeof(VARIANTS); ++i) {
		if (choice == VARIANTS[i])
			return true;
	}

	return false;
}

char makeComputerChoice(void)
{
	int idx = rand() % sizeof(VARIANTS);
	return VARIANTS[idx];
}

int main(int argc, char **argv)
{
	srand(time(NULL));

	char playerChoice;
	char computerChoice;

	const char *playerChoiceDescription;
	const char *computerChoiceDescription;

	do {
		printf("\nPlease choose: rock (r) - paper (p) - scissors (s) - exit (e): ");

		scanf(" %c", &playerChoice);
		if (playerChoice == 'e')
			return 0;

		if (!isValidChoice(playerChoice)) {
			printf("Invalid input. Please, try again.\n");
			continue;
		}

		computerChoice = makeComputerChoice();

		playerChoiceDescription = getDescription(playerChoice);
		computerChoiceDescription = getDescription(computerChoice);

		printf("You choose %s, I choose %s.\n",
			playerChoiceDescription,
			computerChoiceDescription
		);

		if (checkVictory(playerChoice, computerChoice))
			printf("You win, %s beats %s.\n",
				playerChoiceDescription,
				computerChoiceDescription
			);
		else if (checkVictory(computerChoice, playerChoice))
			printf("I win, %s beats %s.\n",
				computerChoiceDescription,
				playerChoiceDescription
			);
		else
			printf("It's draw.\n");

	} while (true);

	return 0;
}
