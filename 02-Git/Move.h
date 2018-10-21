/**
 * Describes player move entity
 */

#ifndef LESSON02_GIT_MOVE_H
#define LESSON02_GIT_MOVE_H

#define MAX_MOVE_VALUE 			2

namespace RockPaperScissors {

	enum MoveValue
	{
		mvvError = -1,
		mvvRock = 0,
		mvvPaper = 1,
		mvvScissors = 2
	};

	class Move
	{
	private:
		MoveValue mValue;
	public:
		Move();
		Move(char value);
		MoveValue getValue();
	};
}

#endif