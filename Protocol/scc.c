#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "scc.h"
#include "../GameFiles/headers/gamestate.h"
#include "../GameFiles/headers/gamestate.h"

char *SendGame(GameState game)
{
    char *buffer = malloc(1000);
    sprintf(buffer, "GameState %d %d %d %d %d %d %d %d", game.ballX, game.ballY, game.ballDx, game.ballDy, game.paddle1Y, game.paddle2Y, game.score1, game.score2);
    sprintf(buffer, "GameState %d %d %d %d %d %d %d %d", game.ballX, game.ballY, game.ballDx, game.ballDy, game.paddle1Y, game.paddle2Y, game.score1, game.score2);
    return buffer;
}

char *SendMove(int upOrDown, int player)
{
    char *buffer = malloc(1000);
    sprintf(buffer, "Move %d %d", upOrDown, player);
    return buffer;
}

char *SendName(const char *name)
{
    char *buffer = malloc(1000);
    sprintf(buffer, "Name %s", name);
    return buffer;
}

char *SendState(int state)
{
    if (state == 0 || state == 1)
    {
        char *buffer = malloc(1000);
        sprintf(buffer, "Player %d", state);
        return buffer;
    }
    else if (state == 2)
    {
        char *buffer = malloc(1000);
        sprintf(buffer, "Start");
        return buffer;
    }
    else if (state == 3)
    {
        char *buffer = malloc(1000);
        sprintf(buffer, "End");
        return buffer;
    }
}

int Receive(char *buffer)
{
    if (strncmp(buffer, "Player", 6) == 0) {
        return 1;
    }
    else if (strncmp(buffer, "Start", 5) == 0) {
        return 2;
    }
    else if (strncmp(buffer, "GameState", 9) == 0) {
        return 3;
    }
    else if (strncmp(buffer, "Move", 4) == 0) {
        return 4;
    }
    else if (strncmp(buffer, "End", 3) == 0) {
        return 5;
    }
    else if (strncmp(buffer, "Name", 4) == 0) {
        return 6;
    }
    return 0;
}