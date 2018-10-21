/**
 * Describes player move entity
 */
#include "Move.h"
#include "Common.h"
#include <stdlib.h>
#include <time.h>

namespace RockPaperScissors {

	Move::Move() {
		srand(time(0));
		mValue = (MoveValue)(rand() % 3);
	}

	Move::Move(char value) {
		switch(value) {
			case 'r' : mValue = mvvRock; break;
			case 'p' : mValue = mvvPaper; break;
			case 's' : mValue = mvvScissors; break;
			default : mValue = mvvError;
		}
	}

	MoveValue Move::getValue() {
		return mValue;
	}
}