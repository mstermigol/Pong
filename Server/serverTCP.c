#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 4444

int main()
{

    int socketFileDescriptor, ret;
    struct sockaddr_in serverAddress;

    int newSocket;
    struct sockaddr_in newAddress;

    socklen_t addressSize;

    char buffer[1024];
    pid_t childpid;

    socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFileDescriptor < 0)
    {
        printf("[-]Error in connection.\n");
        exit(1);
    }
    printf("[+]Server Socket is created.\n");

    memset(&serverAddress, '\0', sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    ret = bind(socketFileDescriptor, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (ret < 0)
    {
        printf("[-]Error in binding.\n");
        exit(1);
    }
    printf("[+]Bind to port %d\n", 4444);

    if (listen(socketFileDescriptor, 10) == 0)
    {
        printf("[+]Listening....\n");
    }
    else
    {
        printf("[-]Error in binding.\n");
    }

    while (1)
    {
        newSocket = accept(socketFileDescriptor, (struct sockaddr *)&newAddress, &addressSize);
        if (newSocket < 0)
        {
            exit(1);
        }
        printf("Connection accepted from %s:%d\n", inet_ntoa(newAddress.sin_addr), ntohs(newAddress.sin_port));

        if ((childpid = fork()) == 0)
        {
            close(socketFileDescriptor);

            while (1)
            {
                recv(newSocket, buffer, 1024, 0);
                if (strcmp(buffer, ":exit") == 0)
                {
                    printf("Disconnected from %s:%d\n", inet_ntoa(newAddress.sin_addr), ntohs(newAddress.sin_port));
                    break;
                }
                else
                {
                    printf("Client: %s\n", buffer);
                    send(newSocket, buffer, strlen(buffer), 0);
                    bzero(buffer, sizeof(buffer));
                }
            }
        }
    }

    close(newSocket);

    return 0;
}