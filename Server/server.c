#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <pthread.h>
#include "headers/game.h"

#define MAX_CLIENTS 2
#define MAX_NICKNAME_LEN 20
#define WINNING_SCORE 10

typedef struct
{
    char name[MAX_NICKNAME_LEN];
    int socket;
    struct sockaddr_in address;
    int playerNumber;
} Client;

typedef struct
{
    int score1;
    int score2;
    int gameStarted;
    GameState gameState; // GameState structure
    Client clients[MAX_CLIENTS];
    int numClients;
} Session;

void UpdatePaddlePosition(Session *session, int playerNumber, int paddleY)
{
    if (playerNumber == 1)
    {
        session->gameState.paddle1Y += paddleY;
    }
    else if (playerNumber == 2)
    {
        session->gameState.paddle2Y += paddleY;
    }
}

int CheckWinner(Session *session)
{
    if (session->gameState.score1 >= WINNING_SCORE)
    {
        return 1;
    }
    else if (session->gameState.score2 >= WINNING_SCORE)
    {
        return 2;
    }
    return 0;
}

void *BroadcastGameState(void *arg)
{
    Session *session = (Session *)arg;
    char message[256];

    while (1)
    {
        // Create a message using the GameState structure
        snprintf(message, sizeof(message), "GameState: ballX %d, ballY %d, ballDx %d, ballDy %d, paddle1Y %d, paddle2Y %d, score1 %d, score2 %d",
                 session->gameState.ballX, session->gameState.ballY,
                 session->gameState.ballDx, session->gameState.ballDy,
                 session->gameState.paddle1Y, session->gameState.paddle2Y,
                 session->gameState.score1, session->gameState.score2);

        for (int j = 0; j < session->numClients; j++)
        {
            ssize_t bytesSent = sendto(session->clients[j].socket, message, strlen(message), 0, (struct sockaddr *)&session->clients[j].address, sizeof(session->clients[j].address));
            if (bytesSent == -1)
            {
                perror("Send error");
            }
        }

        printf("Broadcast: %s\n", message);

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
    fd_set readfds;
    int maxSocketDescriptor;
    Session session;
    session.numClients = 0;
    session.gameStarted = 0;

    if ((serverSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("Socket creation failed");
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

    // Initialize the GameState structure
    session.gameState = InitGame(session.gameState);
    session.gameState.score1 = 0;
    session.gameState.score2 = 0;

    while (1)
    {
        FD_ZERO(&readfds);
        FD_SET(serverSocket, &readfds);
        maxSocketDescriptor = serverSocket;

        for (int i = 0; i < session.numClients; i++)
        {
            int sd = session.clients[i].socket;
            FD_SET(sd, &readfds);
            if (sd > maxSocketDescriptor)
            {
                maxSocketDescriptor = sd;
            }
        }

        if (select(maxSocketDescriptor + 1, &readfds, NULL, NULL, NULL) == -1)
        {
            perror("Select error");
            exit(EXIT_FAILURE);
        }

        if (session.gameStarted == 0)
        {
            if (FD_ISSET(serverSocket, &readfds))
            {
                char buffer[1024];
                struct sockaddr_in clientAddress;
                socklen_t addressLength = sizeof(clientAddress);
                ssize_t bytesReceived = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientAddress, &addressLength);

                if (bytesReceived == -1)
                {
                    perror("Receive error");
                }
                else
                {
                    buffer[bytesReceived] = '\0';

                    if (session.numClients < MAX_CLIENTS)
                    {
                        if (strncmp(buffer, "name ", 5) == 0)
                        {
                            char nickname[MAX_NICKNAME_LEN];
                            if (sscanf(buffer, "name %s", nickname) == 1)
                            {
                                Client newClient;
                                strncpy(newClient.name, nickname, MAX_NICKNAME_LEN);
                                newClient.socket = serverSocket;
                                newClient.playerNumber = session.numClients + 1;
                                newClient.address = clientAddress;
                                session.clients[session.numClients] = newClient;

                                char playerNumber[2];
                                snprintf(playerNumber, sizeof(playerNumber), "%d", newClient.playerNumber);
                                ssize_t bytesSent = sendto(newClient.socket, playerNumber, strlen(playerNumber), 0, (struct sockaddr *)&newClient.address, sizeof(newClient.address));
                                if (bytesSent == -1)
                                {
                                    perror("Send error");
                                }

                                session.numClients++;

                                printf("Client %s connected as Player %d\n", newClient.name, newClient.playerNumber);

                                if (session.numClients == MAX_CLIENTS)
                                {
                                    session.gameStarted = 1;
                                    printf("Game started!\n");

                                    pthread_t broadcastThread;
                                    pthread_create(&broadcastThread, NULL, BroadcastGameState, &session);
                                }
                            }
                        }
                    }
                    else
                    {
                        printf("Maximum number of players reached. Ignoring client request.\n");
                    }
                }
            }
        }

        if (session.gameStarted)
        {
            for (int i = 0; i < session.numClients; i++)
            {
                int sd = session.clients[i].socket;
                if (FD_ISSET(sd, &readfds))
                {
                    char buffer[1024];
                    struct sockaddr_in clientAddress;
                    socklen_t addressLength = sizeof(clientAddress);
                    ssize_t bytesReceived = recvfrom(sd, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientAddress, &addressLength);

                    if (bytesReceived == -1)
                    {
                        perror("Receive error");
                    }
                    else
                    {
                        buffer[bytesReceived] = '\0';

                        int number, player;
                        if (sscanf(buffer, "move %d %d", &number, &player) == 2)
                        {
                            if (player == 1 || player == 2)
                            {
                                // Update the paddle position using the new function
                                UpdatePaddlePosition(&session, player, number);
                                int winner = CheckWinner(&session);

                                if (winner != 0)
                                {
                                    printf("Player %d wins!\n", winner);

                                    char winnerMessage[64];
                                    snprintf(winnerMessage, sizeof(winnerMessage), "Player %d wins the game!", winner);
                                    for (int j = 0; j < session.numClients; j++)
                                    {
                                        ssize_t bytesSent = sendto(session.clients[j].socket, winnerMessage, strlen(winnerMessage), 0, (struct sockaddr *)&session.clients[j].address, sizeof(session.clients[j].address));
                                        if (bytesSent == -1)
                                        {
                                            perror("Send error");
                                        }
                                    }

                                    for (int j = 0; j < session.numClients; j++)
                                    {
                                        close(session.clients[j].socket);
                                    }
                                    session.numClients = 0;

                                    // Reset GameState
                                    
                                    session.gameState = InitGame(session.gameState);
                                    session.gameState.score1 = 0;
                                    session.gameState.score2 = 0;
                                    session.gameStarted = 0;
                                    printf("Game ended. Waiting for new players...\n");
                                }
                            }
                            else
                            {
                                printf("Invalid player number: %d\n", player);
                            }
                        }
                    }
                }
            }
        }
    }

    close(serverSocket);
    return 0;
}
