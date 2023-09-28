#ifndef SOCKET_H
#define SOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>

int Socket(int family, int type, int protocol) {
    int ret = socket(family, type, protocol);
    if (ret < 0) {
        perror("socket");
        exit(1);
    }
    return ret;
}

int Bind(int socketFD, const struct sockaddr *address, socklen_t addressLen) {
    int ret = bind(socketFD, address, addressLen);
    if (ret < 0) {
        perror("bind");
        exit(1);
    }
    return ret;
}

int Recvfrom(int socketFD, void *buffer, size_t length, int flags, struct sockaddr *address, socklen_t *addressLen) {
    int ret = recvfrom(socketFD, buffer, length, flags, address, addressLen);
    if (ret < 0) {
        perror("recvfrom");
        exit(1);
    }
    return ret;
}

int Sendto(int socketFD, const void *buffer, size_t length, int flags, const struct sockaddr *destAddr, socklen_t destLen) {
    int ret = sendto(socketFD, buffer, length, flags, destAddr, destLen);
    if (ret < 0) {
        perror("sendto");
        exit(1);
    }
    return ret;
}

int Close(int socketFD) {
    int ret = close(socketFD);
    if (ret < 0) {
        perror("close");
        exit(1);
    }
    return ret;
}

int Shutdown(int socketFD, int how) {
    int ret = shutdown(socketFD, how);
    if (ret < 0) {
        perror("shutdown");
        exit(1);
    }
    return ret;
}

void HandleShutdownSignal(int socketFD) {
    Close(socketFD);
    exit(0);
}

#endif