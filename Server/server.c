#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{

    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(0);
    }

    char *ip = "127.0.0.1";
    int port = atoi(argv[1]);

    int socketFileDescriptor;
    struct sockaddr_in serverAddress, clientAddress;
    char buffer[1024];
    socklen_t addressSize;
    int n;

    socketFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketFileDescriptor < 0)
    {
        perror("[-]socket error");
        exit(1);
    }

    memset(&serverAddress, '\0', sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = inet_addr(ip);

    n = bind(socketFileDescriptor, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (n < 0)
    {
        perror("[-]bind error");
        exit(1);
    }

    while (1)
    {
        bzero(buffer, 1024);
        addressSize = sizeof(clientAddress);
        recvfrom(socketFileDescriptor, buffer, 1024, 0, (struct sockaddr *)&clientAddress, &addressSize);
        printf("[+]Data recv: %s\n", buffer);
    }

    return 0;
}