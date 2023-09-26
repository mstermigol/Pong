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
    struct sockaddr_in address;
    char buffer[1024];
    socklen_t addressSize;

    socketFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&address, '\0', sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(ip);

    while (1)
    {
        bzero(buffer, 1024);
        fgets(buffer, 1024, stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        sendto(socketFileDescriptor, buffer, 1024, 0, (struct sockaddr *)&address, sizeof(address));
        printf("[+]Data send: %s\n", buffer);
    }

    return 0;
}