#include "../headers/config.h"
#include "../headers/session.h"

void UpdatePaddlePosition(Session *session, int playerNumber, int paddleY)
{
    if (playerNumber == 0)
    {
        session->gameState.paddle1Y += paddleY;
    }
    else if (playerNumber == 1)
    {
        session->gameState.paddle2Y += paddleY;
    }
}

int CheckScore(Session *session)
{
    if (session->gameState.score1 >= WINNING_SCORE)
    {
        return 1;
    }
    else if (session->gameState.score2 >= WINNING_SCORE)
    {
        return 2;
    }
    return 3;
}