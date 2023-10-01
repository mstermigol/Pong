#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>

SDL_Surface *screen;

typedef struct
{
    int ballX, ballY;
    int ballDx, ballDy;
    int paddle1Y, paddle2Y;
    int score1, score2;
} GameState;

GameState InitGame(GameState game)
{

    game.ballX = screen->w / 2;
    game.ballY = screen->h / 2;
    game.ballDy = 1;
    game.ballDx = 1;

    game.paddle1Y = screen->h / 2 - 50;
    game.paddle2Y = screen->h / 2 - 50;

    return game;
}

#endif // GAME_H