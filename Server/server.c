#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "../headers/gamestate.h"
#include "../headers/config.h"
#include "../headers/client.h"
#include "../headers/session.h"
#include "../headers/log.h"
#include "../Protocol/scc.h"

void *GameLogicAndBroadcast(void *arg)
{
    char message[256];
    char *StartMessage = SendState(2);
    Session *session = (Session *)arg;

    for (int j = 0; j < session->numClients; j++)
    {
        ssize_t bytesSent = sendto(session->serverSocket, StartMessage, strlen(StartMessage), 0, (struct sockaddr *)&session->clients[j].address, sizeof(session->clients[j].address));
        if (bytesSent == -1)
        {
            logMessage(session->fileName, "Send error");
            perror("Send error");
        }
        logMessage(session->fileName, "Sent start message to %s\n", session->clients[j].name);
        printf("Sent start message to %s\n", session->clients[j].name);
    }

    int checkScore1 = 0;
    int checkScore2 = 0;

    while (session->gameStarted > 0)
    {
        session->gameState = MoveBall(session->gameState);

        int winner = CheckScore(session);

        char *result = SendGame(session->gameState);

        strncpy(message, result, sizeof(message));

        message[sizeof(message)] = '\0';

        for (int j = 0; j < session->numClients; j++)
        {
            ssize_t bytesSent = sendto(session->serverSocket, message, strlen(message), 0, (struct sockaddr *)&session->clients[j].address, sizeof(session->clients[j].address));
            if (bytesSent == -1)
            {
                logMessage(session->fileName, "Send error");
                perror("Send error");
            }
        }

        if (session->gameState.score1 > checkScore1)
        {
            logMessage(session->fileName, "%s scored a point in session %d\n", session->clients[0].name, session->sessionId);
            printf("%s scored a point in session %d\n", session->clients->name, session->sessionId);
            checkScore1 = session->gameState.score1;
        }

        if (session->gameState.score2 > checkScore2)
        {
            logMessage(session->fileName, "%s scored a point in session %d\n", session->clients[1].name, session->sessionId);
            printf("%s scored a point in session %d\n", session->clients->name, session->sessionId);
            checkScore2 = session->gameState.score2;
        }

        if (winner != 3)
        {
            logMessage(session->fileName, "Player %s wins (with player number %d) in session %d!\n", session->clients[winner].name, session->clients[winner].playerNumber, session->sessionId);
            printf("Player %s wins in session %d!\n", session->clients[winner].name, session->sessionId);
            char *EndMessage = SendEnd(winner);

            for (int j = 0; j < session->numClients; j++)
            {
                ssize_t bytesSent = sendto(session->serverSocket, EndMessage, strlen(EndMessage), 0, (struct sockaddr *)&session->clients[j].address, sizeof(session->clients[j].address));
                if (bytesSent == -1)
                {
                    logMessage(session->fileName, "Send error");
                    perror("Send error");
                }
                logMessage(session->fileName, "Sent end message to %s\n", session->clients[j].name);
                printf("Sent end message to %s\n", session->clients[j].name);
            }
            session->numClients = 0;
            session->gameStarted = 0;
            session->gameState = InitGame(session->gameState);
            session->gameState.score1 = 0;
            session->gameState.score2 = 0;
            pthread_exit(NULL);
        }

        usleep(15000);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s [port] [Log File]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *fileName = argv[2];

    FILE *file = fopen(fileName, "a");

    if (file == NULL)
    {
        logMessage(fileName, "File opening failed");
        perror("File opening failed");
        exit(EXIT_FAILURE);
    }

    fclose(file);

    int serverSocket;
    struct sockaddr_in serverAddress;
    Session sessions[MAX_SESSIONS];

    if ((serverSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        logMessage(fileName, "Socket creation failed");
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int flags = fcntl(serverSocket, F_GETFL, 0);
    if (flags < 0)
    {
        logMessage(fileName, "Failed to get socket flags");
        perror("Failed to get socket flags");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }
    if (fcntl(serverSocket, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        logMessage(fileName, "Failed to set socket to non-blocking mode");
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
        logMessage(fileName, "Binding failed");
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    printf("Server started on port %d and the log is saved in the file %s\n", atoi(argv[1]), fileName);
    logMessage(fileName, "Server started on port %d and the log is saved in the file %s\n", atoi(argv[1]), fileName);

    for (int i = 0; i < MAX_SESSIONS; i++)
    {
        sessions[i].numClients = 0;
        sessions[i].gameStarted = 0;
        sessions[i].gameState = InitGame(sessions[i].gameState);
        sessions[i].gameState.score1 = 0;
        sessions[i].gameState.score2 = 0;
        sessions[i].serverSocket = serverSocket;
        sessions[i].sessionId = i;
        sessions[i].fileName = fileName;
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
            int messageType = Receive(buffer);

            if (messageType == 6)
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

                        char *playerNumber = SendState(newClient.playerNumber);
                        ssize_t bytesSent = sendto(serverSocket, playerNumber, strlen(playerNumber), 0, (struct sockaddr *)&newClient.address, sizeof(newClient.address));
                        if (bytesSent == -1)
                        {
                            logMessage(fileName, "Send error");
                            perror("Send error");
                        }

                        sessions[i].numClients++;

                        logMessage(fileName, "%s connected as Player %d, in session %d\n", newClient.name, newClient.playerNumber, sessions[i].sessionId);
                        printf("%s connected as Player %d, in session %d\n", newClient.name, newClient.playerNumber, sessions[i].sessionId);

                        if (sessions[i].numClients == 2)
                        {
                            sessions[i].gameStarted = 1;
                            logMessage(fileName, "Game started in session %d!\n", sessions[i].sessionId);
                            printf("Game started in session %d!\n", sessions[i].sessionId);

                            pthread_t GameLogicAndBroadcastThread;
                            pthread_create(&GameLogicAndBroadcastThread, NULL, GameLogicAndBroadcast, &sessions[i]);
                        }
                        break;
                    }
                    else
                    {
                        if (i == MAX_SESSIONS - 1)
                        {

                            logMessage(fileName, "No space available. Ignoring client request.\n");
                            printf("No space available. Ignoring client request.\n");
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
                if (messageType == 4)
                {
                    sscanf(buffer, "Move %d %d", &number, &player) == 2;

                    if (player == 0 || player == 1)
                    {
                        logMessage(fileName, "%s with player number %d moved %d the paddle in session %d\n", sessions[numSession].clients[numClient].name, player, number, numSession);
                        printf("%s with player number %d moved %d the paddle in session %d\n", sessions[numSession].clients[numClient].name, player, number, numSession);

                        sessions[numSession].gameState = MovePaddle(number, player, sessions[numSession].gameState);
                    }
                }
            }
        }
    }

    close(serverSocket);
    return 0;
}