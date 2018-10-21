/**
 * Implements game core
 */
#ifndef LESSON02_GIT_DRIVER_H
#define LESSON02_GIT_DRIVER_H

#include "Move.h"

namespace RockPaperScissors {

	enum GameResult
	{
		grDraw = 0,
		grPlayer0Win = 1,
		grPlayer1Win = -1
	};

	class Driver
	{
	private:
		GameResult state[MAX_MOVE_VALUE][MAX_MOVE_VALUE];
	public:
		Driver();
		~Driver();
		GameResult getGameResult(Move player0, Move player1);
	};
}

#endif