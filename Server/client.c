#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ncurses.h>

int main(int argc, char **argv, int player)
{

    if (argc != 3)
    {
        printf("Usage: %s <port> <player_id>\n", argv[0]);
        exit(0);
    }

    char *ip = "127.0.0.1";
    int port = atoi(argv[1]);
    int playerId = atoi(argv[2]);

    int socketFileDescriptor;
    struct sockaddr_in address;
    char buffer[1024];
    socklen_t addressSize;

    socketFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&address, '\0', sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(ip);

    initscr();
    cbreak();
    noecho();

    while (1)
    {
        int ch = getch();
        if (ch == 'w')
        {
            bzero(buffer, 1024);
            sprintf(buffer, "Player: %i UP", playerId);
            sendto(socketFileDescriptor, buffer, 1024, 0, (struct sockaddr *)&address, sizeof(address));
        }
        if (ch == 's')
        {
            bzero(buffer, 1024);
            sprintf(buffer, "Player: %i DOWN", playerId);
            sendto(socketFileDescriptor, buffer, 1024, 0, (struct sockaddr *)&address, sizeof(address));
        }
    }
    endwin();

    return 0;
}