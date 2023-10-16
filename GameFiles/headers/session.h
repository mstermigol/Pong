#ifndef SESSION_H
#define SESSION_H

#include "client.h"
#include "gamestate.h"

typedef struct
{
    int sessionId;
    int gameStarted;
    GameState gameState;
    Client clients[2];
    int numClients;
    int serverSocket;
    const char *fileName;
} Session;

void UpdatePaddlePosition(Session *session, int playerNumber, int paddlePosition);
int CheckScore(Session *session);

#endif // SESSION_H