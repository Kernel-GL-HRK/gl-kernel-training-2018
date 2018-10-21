/**
 * Implements UI.
 */
#include "Verbose.h"
#include <cstdio>

using std::printf;

namespace RockPaperScissors {

	void Verbose::printRules() {
		printf("Please choose: rock (r) - paper (p) - scissors (s). Or (x) to exit.\n");
	}

	void Verbose::printChoise(MoveValue const& player, MoveValue const& pc) {
		char choise_verbose[3][10] = {"rock\0", "paper\0", "scissors\0"};
		printf("You choose %s. My coose was %s\n", choise_verbose[0], choise_verbose[0]);
	}

	void printGameResult(GameResult result) {
		// static string result_verbose[3] = {"You won.", "You lose. You own me 10$.", "Draw."};
		// printf("%s\n", result_verbose[(int)result].c_str());
	}
}