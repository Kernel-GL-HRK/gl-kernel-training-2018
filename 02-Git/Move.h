/**
 * Describes player move entity
 */

#ifndef LESSON02_GIT_MOVE_H
#define LESSON02_GIT_MOVE_H

#define MAX_MOVE_VALUE 			2

namespace RockPaperScissors {

	enum MoveValue
	{
		mvvRock = 0,
		mvvPaper = 1,
		mvvScissors = 2
	};

	class Move
	{
	public:
		Move();
		~Move();
		
	};
}

#endif