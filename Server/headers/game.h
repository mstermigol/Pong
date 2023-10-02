#include "config.h"

typedef struct
{
    int ballX, ballY;
    int ballDx, ballDy;
    int paddle1Y, paddle2Y;
    int score1, score2;
} GameState;

GameState InitGame(GameState game)
{

    game.ballX = SCREEN_WIDTH / 2;
    game.ballY = SCREEN_HEIGHT / 2;
    game.ballDy = 1;
    game.ballDx = 1;

    game.paddle1Y = SCREEN_HEIGHT / 2 - 50;
    game.paddle2Y = SCREEN_HEIGHT / 2 - 50;

    return game;
}