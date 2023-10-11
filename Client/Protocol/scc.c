#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char *Send(char *gameState, int ballX, int allY, int paddle1Y, int paddle2Y)
{
    char returnString[1000];
    sprintf(returnString, "%s %d %d %d %d /", gameState, ballX, allY, paddle1Y, paddle2Y);
    return returnString;
}

const char *Send(char *move, int direction, int player)
{
    char returnString[1000];
    sprintf(returnString, "%s %d %d /", move, direction, player);
    return returnString;
}

const char *Receive(char *protocol)
{
    char *protocolArray[1000];
    int i = 0;
    char *word = strtok(protocol, " ");
    while (word != NULL)
    {
        protocolArray[i++] = word;
        word = strtok(NULL, " ");
    }
    if (strcmp(protocolArray[0], "GameState") == 0)
    {
        char *gameState[100];
        return (gameState, "%s %s %s %s", protocolArray[1], protocolArray[2], protocolArray[3], protocolArray[4]);
    }
    else if (strcmp(protocolArray[0], "Move") == 0)
    {
        char *move[100];
        return (move, "%s %s", protocolArray[1], protocolArray[2]);
    }
}