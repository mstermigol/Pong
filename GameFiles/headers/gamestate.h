#ifndef GAMESTATE_H
#define GAMESTATE_H

typedef struct
{
    int ballX, ballY;
    int ballDx, ballDy;
    int paddle1Y, paddle2Y;
    int score1, score2;
} GameState;

GameState InitGame(GameState game);
GameState MoveBall(GameState game);
int CheckCollision(GameState game, int player);
GameState MovePaddle(int upOrDown, int player, GameState game);

#endif // GAMESTATE_H