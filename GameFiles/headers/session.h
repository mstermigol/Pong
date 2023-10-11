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
} Session;

void updatePaddlePosition(Session *session, int playerNumber, int paddlePosition);
int checkScore(Session *session);

#endif // SESSION_H