#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../GameFiles/headers/gamestate.h"

char *SendGame(GameState game);

char *SendMove(int upOrDown, int player);

char *SendName(const char *name);

char *SendState(int state);

char *SendEnd(int player);

int Receive(char *buffer);