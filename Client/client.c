#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <ncurses.h>
#include <pthread.h>

#define MAX_NICKNAME_LENGTH 20

void *EventListener(void *arg)
{
    int clientSocket = *(int *)arg;
    struct sockaddr_in serverAddress;
    socklen_t serverLen = sizeof(serverAddress);

    fd_set readFileDescriptors;
    int maxFileDescriptors = clientSocket + 1;

    while (1)
    {
        FD_ZERO(&readFileDescriptors);
        FD_SET(clientSocket, &readFileDescriptors);

        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        int activity = select(maxFileDescriptors, &readFileDescriptors, NULL, NULL, &timeout);
        if (activity < 0)
        {
            perror("Select error");
            exit(1);
        }

        if (FD_ISSET(clientSocket, &readFileDescriptors))
        {
            char buffer[1024];
            int bytesReceived = recvfrom(clientSocket, buffer, sizeof(buffer), 0,
                                         (struct sockaddr *)&serverAddress, &serverLen);
            printf("Bytes received: %d\n", bytesReceived);
            if (bytesReceived < 0)
            {
                perror("Error receiving data");
                continue;
            }

            buffer[bytesReceived] = '\0';
            printf("Received from server: %s \n", buffer);
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s [IP] [PORT] [NICKNAME]\n", argv[0]);
        exit(1);
    }

    int clientSocket;
    struct sockaddr_in serverAddress;
    socklen_t serverLen = sizeof(serverAddress);

    clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket < 0)
    {
        perror("Socket creation error");
        exit(1);
    }

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &serverAddress.sin_addr) <= 0)
    {
        perror("Invalid server IP address");
        close(clientSocket);
        exit(1);
    }

    char nickname[MAX_NICKNAME_LENGTH];
    strncpy(nickname, argv[3], MAX_NICKNAME_LENGTH);

    char initialMessage[1024];
    snprintf(initialMessage, sizeof(initialMessage), "name %s", nickname);

    if (sendto(clientSocket, initialMessage, strlen(initialMessage), 0,
               (struct sockaddr *)&serverAddress, serverLen) < 0)
    {
        perror("Error sending data");
        close(clientSocket);
        exit(1);
    }

    char playerNumberAux[1024];
    int bytesReceived = recvfrom(clientSocket, playerNumberAux, sizeof(playerNumberAux), 0,
                                 (struct sockaddr *)&serverAddress, &serverLen);

    int playerNumber = atoi(playerNumberAux);
    printf("Player number: %d\n", playerNumber);

    pthread_t listenerThread;
    if (pthread_create(&listenerThread, NULL, EventListener, &clientSocket) != 0)
    {
        perror("Error creating thread");
        exit(1);
    }

    while (1)
    {
        initscr();
        cbreak();
        echo();
        int key = getch();

        if (key == 'w')
        {
            char buffer[1024];
            snprintf(buffer, sizeof(buffer), "move 0 %d", playerNumber);
            if (sendto(clientSocket, buffer, strlen(buffer), 0,
                       (struct sockaddr *)&serverAddress, serverLen) < 0)
            {
                perror("Error sending data");
                close(clientSocket);
                exit(1);
            }
        }
        else if (key == 's')
        {
            char buffer[1024];
            snprintf(buffer, sizeof(buffer), "move 1 %d", playerNumber);
            if (sendto(clientSocket, buffer, strlen(buffer), 0,
                       (struct sockaddr *)&serverAddress, serverLen) < 0)
            {
                perror("Error sending data");
                close(clientSocket);
                exit(1);
            }
        }

        endwin();
    }

    close(clientSocket);

    return 0;
}