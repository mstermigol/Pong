#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_NICKNAME_LENGTH 20
#define MAX_SESSIONS 2

struct ClientInfo {
    char nickname[MAX_NICKNAME_LENGTH];
    struct sockaddr_in clientAddr;
};

struct Session {
    struct ClientInfo players[2];
    int sessionNumber;
    int isOccupied;
};

void PrintSessionClients(struct Session *session) {
    printf("Session %d:\n", session->sessionNumber);
    for (int i = 0; i < session->isOccupied; i++) {
        printf("Client %d: %s - Socket Structure: %s:%d\n",
               i + 1, session->players[i].nickname,
               inet_ntoa(session->players[i].clientAddr.sin_addr),
               ntohs(session->players[i].clientAddr.sin_port));
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [PORT]\n", argv[0]);
        exit(1);
    }

    int serverSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientLen = sizeof(clientAddr);

    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket < 0) {
        perror("Socket creation error");
        exit(1);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[1]));
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Binding error");
        close(serverSocket);
        exit(1);
    }

    printf("Server is running and listening on port %d...\n", atoi(argv[1]));

    struct ClientInfo clients[MAX_SESSIONS*2];
    int numClients = 0;

    struct Session sessions[MAX_SESSIONS];
    int numSessions = 0;

    while (1) {
        char buffer[1024];
        int bytesRecieved = recvfrom(serverSocket, buffer, sizeof(buffer), 0,
                                      (struct sockaddr *)&clientAddr, &clientLen);
        if (bytesRecieved < 0) {
            perror("Error receiving data");
            continue;
        }

        buffer[bytesRecieved] = '\0';

        char nickname[MAX_NICKNAME_LENGTH];
        sscanf(buffer, "%s", nickname);

        int nicknameExists = 0;
        for (int i = 0; i < numClients; i++) {
            if (strcmp(clients[i].nickname, nickname) == 0) {
                nicknameExists = 1;
                break;
            }
        }

        if (nicknameExists) {
            printf("Nickname %s already exists. Ignoring new connection request.\n", nickname);
        } else {
            if (numClients < MAX_SESSIONS*2) {
                strncpy(clients[numClients].nickname, nickname, MAX_NICKNAME_LENGTH);
                memcpy(&clients[numClients].clientAddr, &clientAddr, sizeof(clientAddr));
                numClients++;

                int sessionIndex = -1;
                for (int i = 0; i < numSessions; i++) {
                    if (sessions[i].isOccupied < 2) {
                        sessionIndex = i;
                        break;
                    }
                }

                if (sessionIndex == -1 && numSessions < MAX_SESSIONS) {
                    sessionIndex = numSessions;
                    sessions[sessionIndex].sessionNumber = numSessions;
                    numSessions++;
                }

                if (sessionIndex != -1) {
                    struct Session *currentSession = &sessions[sessionIndex];
                    if (currentSession->isOccupied < 2) {
                        currentSession->players[currentSession->isOccupied] = clients[numClients - 1];
                        currentSession->isOccupied++;

                        char welcomeMsg[1024];
                        snprintf(welcomeMsg, sizeof(welcomeMsg),
                                 "%s welcome to Session %d! This is your socket structure: %s:%d\n",
                                 nickname, currentSession->sessionNumber,
                                 inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

                        sendto(serverSocket, welcomeMsg, strlen(welcomeMsg), 0,
                               (struct sockaddr *)&clientAddr, clientLen);
                    }
                }
            } else {
                printf("Maximum number of clients reached. Ignoring new connection request.\n");
            }
        }

        for (int i = 0; i < numSessions; i++) {
            if (i < numSessions) {
                PrintSessionClients(&sessions[i]);
            }
        }
    }

    close(serverSocket);

    return 0;
}
