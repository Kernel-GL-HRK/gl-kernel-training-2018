/**
 * @brief Rock–paper–scissors game
 * @detail implementation of R.P.S. game
 * 
 * @param argc not used
 * @param argv not used
 * 
 * @return 0
 */

#include "Driver.h"
#include "Verbose.h"
#include <cstdio> // getchar

using namespace RockPaperScissors;

int main(int argc, char const *argv[])
{
	Verbose croupier;

	char userOption = '.';
	Driver driver;

	while (userOption != 'x') {
		croupier.printRules();
		userOption = getchar();

		if((userOption == 'p') || (userOption == 's') || (userOption == 'r')) {
			Move usr(userOption);
			Move pc;
			croupier.printChoise(usr.getValue(), pc.getValue());
			croupier.printGameResult(driver.getGameResult(usr, pc));
		} else if(userOption == 'x') {
			break;
		} else {
			croupier.printPrank();
		}

		getchar();
	}
	return 0;
}