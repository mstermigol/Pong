#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdlib.h>
#include "config.h"

typedef struct
{
    int socket;
    int paddle_y;
    int score;
} Player;

typedef struct
{
    Player players[MAX_CLIENTS];
    int ball_x;
    int ball_y;
    int ball_dx;
    int ball_dy;
} GameSession;

#endif