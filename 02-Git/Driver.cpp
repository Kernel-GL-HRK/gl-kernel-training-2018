/**
 * Implements game core
 */
#include "Driver.h"
#include "Move.h"

namespace RockPaperScissors {

	Driver::Driver() {
		state[mvvRock][mvvRock] = grDraw;
		state[mvvRock][mvvPaper] = grPlayer1Win;
		state[mvvRock][mvvScissors] = grPlayer0Win;

		state[mvvPaper][mvvRock] = grPlayer0Win;
		state[mvvPaper][mvvPaper] = grDraw;
		state[mvvPaper][mvvScissors] = grPlayer1Win;

		state[mvvScissors][mvvRock] = grPlayer1Win;
		state[mvvScissors][mvvPaper] = grPlayer0Win;
		state[mvvScissors][mvvScissors] = grDraw;
	}

	Driver::~Driver() {

	}

	GameResult Driver::getGameResult(Move player0, Move player1) {
	}
}