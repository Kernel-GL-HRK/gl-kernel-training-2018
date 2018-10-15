#include "stdio.h"
#include "stdlib.h"
#include "time.h"

typedef struct {
	const char *fullname;
	char shortname;
	char weakcase;
} case_type;

int
main(void)
{
	srand(time(NULL));

	const case_type gamecase[3] = {
			{.fullname = "rock", .shortname = 'r', .weakcase = 's'},
			{.fullname = "paper", .shortname = 'p', .weakcase = 'r'},
			{.fullname = "scissors", .shortname = 's', .weakcase = 'p'},
	};
	const size_t numgamecase = sizeof(gamecase) / sizeof(gamecase[0]);

	while (1) {
		printf("> Please choose: ");
		for (size_t i = 0; i < numgamecase; i++) {
			printf("%s (%c) ", gamecase[i].fullname, gamecase[i].shortname);
		}
		printf("\n");

		char cuser = getchar();
		int rval = rand() / (RAND_MAX / numgamecase);
		for (size_t i = 0; i < numgamecase; i++) {
			if (cuser == gamecase[i].shortname) {
				printf("You choose: %s, I choose %s\n", gamecase[i].fullname,
						gamecase[rval].fullname);
				if (gamecase[i].shortname == gamecase[rval].shortname) {
					printf("Draw\n");
				}
				else if (gamecase[i].weakcase == gamecase[rval].shortname) {
					printf("You win: %s beats %s\n", gamecase[i].fullname,
							gamecase[rval].fullname);
				}
				else {
					printf("I win: %s beats %s\n", gamecase[rval].fullname,
							gamecase[i].fullname);
				}
				break;
			}

		}

		int c;
		while ((c = getchar()) != '\n' && c != EOF) {
		}
	}
	return 0;
}
