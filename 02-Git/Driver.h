/**
 * Implements game core
 */
#ifndef LESSON02_GIT_DRIVER_H
#define LESSON02_GIT_DRIVER_H

#include "Move.h"

namespace RockPaperScissors
{

    enum GameResult
    {
        grDraw = 2,
        grPlayer0Win = 0,
        grPlayer1Win = 1
    };

    class Driver
    {
    private:
        GameResult mState[MAX_MOVE_VALUE][MAX_MOVE_VALUE];
    public:
        Driver();
        ~Driver();
        GameResult getGameResult(Move player0, Move player1);
    };
}

#endif