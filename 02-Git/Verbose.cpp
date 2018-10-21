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
		static char choise_verbose[3][10] = {"rock\0", "paper\0", "scissors\0"};
		printf("You choose %s. I choose %s\n", choise_verbose[player], choise_verbose[pc]);
	}

	void Verbose::printGameResult(GameResult result) {
		static int cash = 0;
		static char result_verbose[3][33] = {"You won. Let's play once agean.\0", "You lose.\0", "Draw.\0"};
		if(result == grPlayer1Win) {
			cash++;
			printf("%s You own me %d$    ($_$)\n\n", result_verbose[result], cash*100);
		} else {
			printf("%s\n\n", result_verbose[result]);
		}
	}

	void Verbose::printPrank() {
		printf("You must be joking    (-_-). Have you ever learn alfabet?\n");
	}
}