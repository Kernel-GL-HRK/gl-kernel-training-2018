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

using namespace RockPaperScissors;

int main(int argc, char const *argv[])
{
	Verbose croupier;
	croupier.printRules();
	return 0;
}