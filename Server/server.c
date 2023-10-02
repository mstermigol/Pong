#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <pthread.h>

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
    int var1;
    int var2;
    Client clients[MAX_CLIENTS];
    int numClients;
    int gameStarted;
} Session;

void UpdateVariables(Session *session, int playerNumber, int number)
{
    if (playerNumber == 1)
    {
        session->var1 += number;
    }
    else if (playerNumber == 2)
    {
        session->var2 += number;
    }
}

int CheckWinner(Session *session)
{
    if (session->var1 >= WINNING_SCORE)
    {
        return 1;
    }
    else if (session->var2 >= WINNING_SCORE)
    {
        return 2;
    }
    return 0;
}

void *BroadcastVariables(void *arg)
{
    Session *session = (Session *)arg;
    char message[256];

    while (1)
    {
        snprintf(message, sizeof(message), "var1: %d and var2: %d", session->var1, session->var2);

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
                socklen_t adrressLength = sizeof(clientAddress);
                ssize_t bytesRecieved = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientAddress, &adrressLength);

                if (bytesRecieved == -1)
                {
                    perror("Receive error");
                }
                else
                {
                    buffer[bytesRecieved] = '\0';

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
                                    pthread_create(&broadcastThread, NULL, BroadcastVariables, &session);
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
                    socklen_t adrressLength = sizeof(clientAddress);
                    ssize_t bytesRecieved = recvfrom(sd, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientAddress, &adrressLength);

                    if (bytesRecieved == -1)
                    {
                        perror("Receive error");
                    }
                    else
                    {
                        buffer[bytesRecieved] = '\0';

                        int number, player;
                        if (sscanf(buffer, "move %d %d", &number, &player) == 2)
                        {
                            if (player == 1 || player == 2)
                            {
                                UpdateVariables(&session, player, number);
                                int winner = CheckWinner(&session);

                                char message[256];
                                snprintf(message, sizeof(message), "var1: %d and var2: %d", session.var1, session.var2);

                                for (int j = 0; j < session.numClients; j++)
                                {
                                    ssize_t bytesSent = sendto(session.clients[j].socket, message, strlen(message), 0, (struct sockaddr *)&session.clients[j].address, sizeof(session.clients[j].address));
                                    if (bytesSent == -1)
                                    {
                                        perror("Send error");
                                    }
                                }

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
                                    session.var1 = 0;
                                    session.var2 = 0;
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