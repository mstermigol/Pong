#ifndef CLIENT_H
#define CLIENT_H

#include <netinet/in.h>
#include "config.h"

typedef struct
{
    int sessionId;
    char name[MAX_NICKNAME_LEN];
    struct sockaddr_in address;
    int playerNumber;
} Client;

#endif // CLIENT_H