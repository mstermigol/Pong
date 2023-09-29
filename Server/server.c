#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 10
#define MAX_NICKNAME_LENGTH 20

// Define a structure to store client information
struct ClientInfo {
    char nickname[MAX_NICKNAME_LENGTH];
    struct sockaddr_in client_addr;
};

void printClients(struct ClientInfo *clients, int num_clients) {
    printf("Connected Clients:\n");
    for (int i = 0; i < num_clients; i++) {
        printf("Client %d: %s - Socket Structure: %s:%d\n",
               i + 1, clients[i].nickname,
               inet_ntoa(clients[i].client_addr.sin_addr),
               ntohs(clients[i].client_addr.sin_port));
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [PORT]\n", argv[0]);
        exit(1);
    }

    int server_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Create a UDP socket
    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket < 0) {
        perror("Socket creation error");
        exit(1);
    }

    // Bind the server socket
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding error");
        close(server_socket);
        exit(1);
    }

    printf("Server is running and listening on port %d...\n", atoi(argv[1]));

    struct ClientInfo clients[MAX_CLIENTS];
    int num_clients = 0;

    while (1) {
        char buffer[1024];
        int bytes_received = recvfrom(server_socket, buffer, sizeof(buffer), 0,
                                      (struct sockaddr *)&client_addr, &client_len);
        if (bytes_received < 0) {
            perror("Error receiving data");
            continue;
        }

        buffer[bytes_received] = '\0';

        // Extract the client's nickname from the received message
        char nickname[MAX_NICKNAME_LENGTH];
        sscanf(buffer, "%s", nickname);

        // Find the client in the array by nickname
        int client_index = -1;
        for (int i = 0; i < num_clients; i++) {
            if (strcmp(clients[i].nickname, nickname) == 0) {
                client_index = i;
                break;
            }
        }

        if (client_index != -1) {
            // If the client is found, update the sockaddr_in
            memcpy(&clients[client_index].client_addr, &client_addr, sizeof(client_addr));

            // Handle the rest of the client's messages and data here
        } else {
            // If the client is not found, it's a new client, so create a new entry
            if (num_clients < MAX_CLIENTS) {
                strncpy(clients[num_clients].nickname, nickname, MAX_NICKNAME_LENGTH);
                memcpy(&clients[num_clients].client_addr, &client_addr, sizeof(client_addr));
                num_clients++;

                // Print all connected clients
                printClients(clients, num_clients);

                // Send a welcome message back to the client
                char welcome_msg[1024];
                snprintf(welcome_msg, sizeof(welcome_msg),
                         "%s welcome! This is your socket structure: %s:%d\n",
                         nickname, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

                sendto(server_socket, welcome_msg, strlen(welcome_msg), 0,
                       (struct sockaddr *)&client_addr, client_len);
            } else {
                printf("Maximum number of clients reached. Ignoring new connection request.\n");
            }
        }
    }

    // Close the server socket (this part will never be reached in this example)
    close(server_socket);

    return 0;
}
