/**
 * Describes player move entity
 */

#ifndef 02_GIT_MOVE_H
#define 02_GIT_MOVE_H

namespace RockPaperScissors {

	typedef enum MoveValue
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