#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdlib.h>
#include "SDL2/SDL.h"
#include "config.h"

typedef struct
{
    int x, y;
    int w, h;
} Paddle;

typedef struct
{
    int x, y;
    int w, h;
} Ball;

typedef struct
{
    int socket;
    int paddleY;
    int score;
} Player;

typedef struct
{
    Player players[MAX_CLIENTS];
    int ballX;
    int ballY;
    int ballDx;
    int ballDy;
} GameSession;

static SDL_Surface *screen;

#endif