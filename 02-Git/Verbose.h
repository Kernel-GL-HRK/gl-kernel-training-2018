/**
 * Implements UI.
 */
#ifndef LESSON02_GIT_VERBOSE_H
#define LESSON02_GIT_VERBOSE_H

#include "Move.h"
#include "Driver.h"

namespace RockPaperScissors {

	class Verbose
	{
	public:
		void printRules();
		void printChoise(MoveValue const& player, MoveValue const& pc);
		void printGameResult(GameResult result);
		void printPrank();
	};
}

#endif