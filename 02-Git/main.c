#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

struct SelectType{
	const char *fullName;
	const char shortName;
	const char strongerThan;
};


static const struct SelectType selects[] = {{"rock",'r','s'},{"paper",'p','r'},{"scissors",'s','p'}};





static void whoWin(const struct   SelectType *const userSelect, const struct SelectType *const compSelect)
{

	if (userSelect == NULL || compSelect == NULL)
		printf(">Ups\n");
	else if (userSelect->shortName == compSelect->shortName)
		printf(">Draw\n");
	else if (userSelect->shortName != compSelect->strongerThan)
		printf(">You win: %s beats %s\n", userSelect->fullName,compSelect->fullName);
	else
		printf(">I win: %s beats %s\n", compSelect->fullName,userSelect->fullName);
}

static void playGame()
{
	srand(time(NULL));
	int numSelectTypes = ARRAY_SIZE(selects) ;
	while(1){
		printf("Please choose: rock (r) - paper (p) - scissors (s)\n");
		char userSelect ;
		scanf(" %c", &userSelect);
		unsigned int computerSelect =  rand() % numSelectTypes;
		for (int cntUserSelect = 0 ; cntUserSelect < numSelectTypes; cntUserSelect++){
			if (selects[cntUserSelect].shortName == userSelect){
				printf(">You choose: %s, I choose :%s\n", selects[cntUserSelect].fullName,selects[computerSelect].fullName) ;
				whoWin(&selects[cntUserSelect],&selects[computerSelect]);
			}
		}
	}
}

int main()
{
	playGame();
	return 0;
}
