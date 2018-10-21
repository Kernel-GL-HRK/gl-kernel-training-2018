/**
 * Implements game core
 */
#include "Driver.h"
#include "Move.h"
#include "Verbose.h"

namespace RockPaperScissors {

	Driver::Driver() {
		mState[mvvRock][mvvRock] = grDraw;
		mState[mvvRock][mvvPaper] = grPlayer1Win;
		mState[mvvRock][mvvScissors] = grPlayer0Win;

		mState[mvvPaper][mvvRock] = grPlayer0Win;
		mState[mvvPaper][mvvPaper] = grDraw;
		mState[mvvPaper][mvvScissors] = grPlayer1Win;

		mState[mvvScissors][mvvRock] = grPlayer1Win;
		mState[mvvScissors][mvvPaper] = grPlayer0Win;
		mState[mvvScissors][mvvScissors] = grDraw;
	}

	Driver::~Driver() {}

	GameResult Driver::getGameResult(Move player0, Move player1) {
		return mState[player0.getValue()][player1.getValue()];
	}
}