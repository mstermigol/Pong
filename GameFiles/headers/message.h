#ifndef MESSAGE_H
#define MESSAGE_H

#include <netinet/in.h>
#include "session.h"

typedef struct
{
    char message[9];
    struct sockaddr_in addressFromClient;
    Session *sessions[MAX_SESSIONS];
} Message;

void logMessage(const char *format, ...);

#endif // MESSAGE_H