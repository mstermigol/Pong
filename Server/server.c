#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include "../headers/gamestate.h"
#include "../headers/config.h"
#include "../headers/client.h"
#include "../headers/session.h"
#include "../headers/log.h"

void *GameLogicAndBroadcast(void *arg)
{
    char message[256];

    Session *session = (Session *)arg;

    int checkScore1 = 0;
    int checkScore2 = 0;

    while (session->gameStarted > 0)
    {
        session->gameState = MoveBall(session->gameState);

        int winner = CheckScore(session);

        snprintf(message, sizeof(message), "GameState %d %d %d %d %d %d %d %d",
                 session->gameState.ballX, session->gameState.ballY,
                 session->gameState.ballDx, session->gameState.ballDy,
                 session->gameState.paddle1Y, session->gameState.paddle2Y,
                 session->gameState.score1, session->gameState.score2);

        for (int j = 0; j < session->numClients; j++)
        {
            ssize_t bytesSent = sendto(session->serverSocket, message, strlen(message), 0, (struct sockaddr *)&session->clients[j].address, sizeof(session->clients[j].address));
            if (bytesSent == -1)
            {
                perror("Send error");
            }
        }

        if (session->gameState.score1 > checkScore1)
        {
            logMessage("%s scored a point in session %d\n", session->clients[0].name, session->sessionId);
            // printf("%s scored a point in session %d\n", session->clients->name, session->sessionId);
            checkScore1 = session->gameState.score1;
        }

        if (session->gameState.score2 > checkScore2)
        {
            logMessage("%s scored a point in session %d\n", session->clients[1].name, session->sessionId);
            // printf("%s scored a point in session %d\n", session->clients->name, session->sessionId);
            checkScore2 = session->gameState.score2;
        }

        if (winner != 3)
        {
            logMessage("Player %s wins in session %d!\n", session->clients[winner].name, session->sessionId);
            // printf("Player %s wins in session %d!\n", session->clients[winner].name, session->sessionId);

            session->numClients = 0;
            session->gameStarted = 0;
            session->gameState = InitGame(session->gameState);
            session->gameState.score1 = 0;
            session->gameState.score2 = 0;
            pthread_exit(NULL);
        }

        usleep(100000);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s [port]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int serverSocket;
    struct sockaddr_in serverAddress;
    Session sessions[MAX_SESSIONS];

    if ((serverSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int flags = fcntl(serverSocket, F_GETFL, 0);
    if (flags < 0)
    {
        perror("Failed to get socket flags");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }
    if (fcntl(serverSocket, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        perror("Failed to set socket to non-blocking mode");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(atoi(argv[1]));

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    printf("Server started on port %d\n", atoi(argv[1]));

    for (int i = 0; i < MAX_SESSIONS; i++)
    {
        sessions[i].numClients = 0;
        sessions[i].gameStarted = 0;
        sessions[i].gameState = InitGame(sessions[i].gameState);
        sessions[i].gameState.score1 = 0;
        sessions[i].gameState.score2 = 0;
        sessions[i].serverSocket = serverSocket;
        sessions[i].sessionId = i;
    }

    while (1)
    {
        char buffer[1024];
        struct sockaddr_in clientAddress;
        socklen_t addressLength = sizeof(clientAddress);
        ssize_t bytesReceived = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientAddress, &addressLength);

        if (bytesReceived < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
        {
            continue;
        }
        else
        {
            buffer[bytesReceived] = '\0';

            if (strncmp(buffer, "name ", 5) == 0)
            {
                for (int i = 0; i < MAX_SESSIONS; i++)
                {
                    if (sessions[i].numClients < 2)
                    {
                        char nickname[MAX_NICKNAME_LEN];
                        Client newClient;
                        strncpy(nickname, buffer + 5, MAX_NICKNAME_LEN);
                        nickname[MAX_NICKNAME_LEN] = '\0';
                        strncpy(newClient.name, nickname, MAX_NICKNAME_LEN);
                        newClient.playerNumber = sessions[i].numClients;
                        newClient.address = clientAddress;
                        newClient.sessionId = i;
                        sessions[i].clients[sessions[i].numClients] = newClient;

                        char playerNumber[2];
                        snprintf(playerNumber, sizeof(playerNumber), "%d", newClient.playerNumber);
                        ssize_t bytesSent = sendto(serverSocket, playerNumber, strlen(playerNumber), 0, (struct sockaddr *)&newClient.address, sizeof(newClient.address));
                        if (bytesSent == -1)
                        {
                            perror("Send error");
                        }

                        sessions[i].numClients++;

                        logMessage("%s connected as Player %d, in session %d\n", newClient.name, newClient.playerNumber, sessions[i].sessionId);
                        // printf("%s connected as Player %d, in session %d\n", newClient.name, newClient.playerNumber, sessions[i].sessionId);

                        if (sessions[i].numClients == 2)
                        {
                            sessions[i].gameStarted = 1;
                            logMessage("Game started in session %d!\n", sessions[i].sessionId);
                            // printf("Game started in session %d!\n", sessions[i].sessionId);

                            pthread_t GameLogicAndBroadcastThread;
                            pthread_create(&GameLogicAndBroadcastThread, NULL, GameLogicAndBroadcast, &sessions[i]);
                        }
                        break;
                    }
                    else
                    {
                        if (i == MAX_SESSIONS - 1)
                        {

                            logMessage("No space available. Ignoring client request.\n");
                            // printf("No space available. Ignoring client request.\n");
                        }
                    }
                }
            }
            else
            {
                int numSession;
                int numClient;

                for (int i = 0; i < MAX_SESSIONS; i++)
                {
                    for (int j = 0; j < 2; j++)
                    {
                        if (memcmp(&clientAddress.sin_addr, &sessions[i].clients[j].address.sin_addr, sizeof(struct in_addr)) == 0 && clientAddress.sin_port == sessions[i].clients[j].address.sin_port)

                        {
                            numSession = i;
                            numClient = j;
                            break;
                        }
                    }
                }

                int number, player;
                if (sscanf(buffer, "Move %d %d", &number, &player) == 2)
                {
                    if (player == 0 || player == 1)
                    {
                        logMessage("%s with player number %d moved %d the paddle in session %d\n", sessions[numSession].clients[numClient].name, player, number, numSession);
                        // printf("%s with player number %d moved %d the paddle in session %d\n", sessions[numSession].clients[numClient].name, player, number, numSession);

                        sessions[numSession].gameState = MovePaddle(number, player, sessions[numSession].gameState);
                    }
                }
            }
        }
    }

    close(serverSocket);
    return 0;
}