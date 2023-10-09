#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_SESSIONS 2
#define MAX_NICKNAME_LEN 20
#define WINNING_SCORE 1
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define PADDLE_WIDTH 10
#define PADDLE_HEIGHT 50
#define PADDLE_1_X 10
#define PADDLE_2_X 620
#define BALL_WIDTH 10
#define BALL_HEIGHT 10

typedef struct
{
    int ballX, ballY;
    int ballDx, ballDy;
    int paddle1Y, paddle2Y;
    int score1, score2;
} GameState;

typedef struct
{
    int sessionId;
    char name[MAX_NICKNAME_LEN];
    struct sockaddr_in address;
    int playerNumber;
} Client;

typedef struct
{
    int sessionId;
    int gameStarted;
    GameState gameState;
    Client clients[2];
    int numClients;
    int serverSocket;
} Session;

typedef struct
{
    char message[9];
    struct sockaddr_in addressFromClient;
    Session *sessions[MAX_SESSIONS];
} Message;

GameState InitGame(GameState game)
{

    game.ballX = SCREEN_WIDTH / 2;
    game.ballY = SCREEN_HEIGHT / 2;
    game.ballDy = 1;
    game.ballDx = 1;

    game.paddle1Y = SCREEN_HEIGHT / 2 - 50;
    game.paddle2Y = SCREEN_HEIGHT / 2 - 50;

    return game;
}

GameState MovePaddle(int upOrDown, int player, GameState game)
{
    if (player == 0)
    {
        if (upOrDown == 0)
        {
            if (game.paddle1Y >= SCREEN_HEIGHT - PADDLE_HEIGHT)
            {
                game.paddle1Y = SCREEN_HEIGHT - PADDLE_HEIGHT;
            }
            else
            {
                game.paddle1Y += 5;
            }
        }
        if (upOrDown == 1)
        {
            if (game.paddle1Y <= 0)
            {
                game.paddle1Y = 0;
            }
            else
            {
                game.paddle1Y -= 5;
            }
        }
    }
    else
    {
        if (upOrDown == 0)
        {
            if (game.paddle2Y >= SCREEN_HEIGHT - PADDLE_HEIGHT)
            {
                game.paddle2Y = SCREEN_HEIGHT - PADDLE_HEIGHT;
            }
            else
            {
                game.paddle2Y += 5;
            }
        }
        if (upOrDown == 1)
        {
            if (game.paddle2Y <= 0)
            {
                game.paddle2Y = 0;
            }
            else
            {
                game.paddle2Y -= 5;
            }
        }
    }
    return game;
}

int CheckCollision(GameState game, int player)
{
    int paddleY;
    int paddleX;

    if (player == 0)
    {
        paddleY = game.paddle1Y;
        paddleX = PADDLE_1_X;
    }
    else
    {
        paddleY = game.paddle2Y;
        paddleX = PADDLE_2_X;
    }

    int leftOfTheBall, leftOfPaddle;
    int rightOfTheBall, rightOfPaddle;
    int topOfTheBall, topOfThePaddle;
    int bottomOfTheBall, bottomOfThePaddle;

    leftOfTheBall = game.ballX;
    rightOfTheBall = game.ballX + BALL_WIDTH;
    topOfTheBall = game.ballY;
    bottomOfTheBall = game.ballY + BALL_HEIGHT;

    leftOfPaddle = paddleX;
    rightOfPaddle = paddleX + PADDLE_WIDTH;
    topOfThePaddle = paddleY;
    bottomOfThePaddle = paddleY + PADDLE_HEIGHT;

    if (leftOfTheBall > rightOfPaddle)
    {
        return 0;
    }
    if (rightOfTheBall < leftOfPaddle)
    {
        return 0;
    }
    if (topOfTheBall > bottomOfThePaddle)
    {
        return 0;
    }
    if (bottomOfTheBall < topOfThePaddle)
    {
        return 0;
    }
    return 1;
}

GameState MoveBall(GameState game)
{
    game.ballX += game.ballDx;
    game.ballY += game.ballDy;

    if (game.ballX < 0)
    {

        game.score2 += 1;
        game = InitGame(game);
    }

    if (game.ballX > SCREEN_WIDTH - 10)
    {

        game.score1 += 1;
        game = InitGame(game);
    }

    if (game.ballY < 0 || game.ballY > SCREEN_HEIGHT - 10)
    {

        game.ballDy = -game.ballDy;
    }

    int player;

    for (player = 0; player < 2; player++)
    {
        int paddle;

        if (player == 0)
        {
            paddle = game.paddle1Y;
        }
        else
        {
            paddle = game.paddle2Y;
        }

        int collision = CheckCollision(game, player);

        if (collision == 1)
        {

            if (game.ballDx < 0)
            {

                game.ballDx -= 1;
            }
            else
            {

                game.ballDx += 1;
            }

            game.ballDx = -game.ballDx;

            int hitPosition = (paddle + PADDLE_HEIGHT) - game.ballY;

            if (hitPosition >= 0 && hitPosition < 7)
            {
                game.ballDy = 4;
            }

            if (hitPosition >= 7 && hitPosition < 14)
            {
                game.ballDy = 3;
            }

            if (hitPosition >= 14 && hitPosition < 21)
            {
                game.ballDy = 2;
            }

            if (hitPosition >= 21 && hitPosition < 28)
            {
                game.ballDy = 1;
            }

            if (hitPosition >= 28 && hitPosition < 32)
            {
                game.ballDy = 0;
            }

            if (hitPosition >= 32 && hitPosition < 39)
            {
                game.ballDy = -1;
            }

            if (hitPosition >= 39 && hitPosition < 46)
            {
                game.ballDy = -2;
            }

            if (hitPosition >= 46 && hitPosition < 53)
            {
                game.ballDy = -3;
            }

            if (hitPosition >= 53 && hitPosition <= 60)
            {
                game.ballDy = -4;
            }

            if (game.ballDx > 0)
            {

                if (game.ballX < 30)
                {

                    game.ballX = 30;
                }
            }
            else
            {

                if (game.ballX > 600)
                {

                    game.ballX = 600;
                }
            }
        }
    }

    return game;
}

void UpdatePaddlePosition(Session *session, int playerNumber, int paddleY)
{
    if (playerNumber == 0)
    {
        session->gameState.paddle1Y += paddleY;
    }
    else if (playerNumber == 1)
    {
        session->gameState.paddle2Y += paddleY;
    }
}

int CheckScore(Session *session)
{
    if (session->gameState.score1 >= WINNING_SCORE)
    {
        return 0;
    }
    else if (session->gameState.score2 >= WINNING_SCORE)
    {
        return 1;
    }
    return 3;
}

void *GameLogicAndBroadcast(void *arg)
{
    char message[256];

    Session *session = (Session *)arg;

    while (session->gameStarted > 0)
    {
        session->gameState = MoveBall(session->gameState);

        int winner = CheckScore(session);

        if (winner != 3)
        {
            printf("Player %d wins in session %d!\n", winner, session->sessionId);

            session->numClients = 0;
            session->gameStarted = 0;
            session->gameState = InitGame(session->gameState);
            session->gameState.score1 = 0;
            session->gameState.score2 = 0;
            pthread_exit(NULL);
        }
        else
        {
            snprintf(message, sizeof(message), "GameState %d %d %d %d %d %d %d %d",
                     session->gameState.ballX, session->gameState.ballY,
                     session->gameState.ballDx, session->gameState.ballDy,
                     session->gameState.paddle1Y, session->gameState.paddle2Y,
                     session->gameState.score1, session->gameState.score2);

            for (int j = 0; j < session->numClients; j++)
            {
                ssize_t bytesSent = sendto(session->serverSocket, message, strlen(message), 0, (struct sockaddr *)&session->clients[j].address, sizeof(session->clients[j].address));
                if (bytesSent == -1)
                {
                    perror("Send error");
                }
            }

            usleep(100000);
        }
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s [port]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int serverSocket;
    struct sockaddr_in serverAddress;
    Session sessions[MAX_SESSIONS];

    if ((serverSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int flags = fcntl(serverSocket, F_GETFL, 0);
    if (flags < 0)
    {
        perror("Failed to get socket flags");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }
    if (fcntl(serverSocket, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        perror("Failed to set socket to non-blocking mode");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(atoi(argv[1]));

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    printf("Server started on port %d\n", atoi(argv[1]));

    for (int i = 0; i < MAX_SESSIONS; i++)
    {
        sessions[i].numClients = 0;
        sessions[i].gameStarted = 0;
        sessions[i].gameState = InitGame(sessions[i].gameState);
        sessions[i].gameState.score1 = 0;
        sessions[i].gameState.score2 = 0;
        sessions[i].serverSocket = serverSocket;
        sessions[i].sessionId = i;
    }

    while (1)
    {
        char buffer[1024];
        struct sockaddr_in clientAddress;
        socklen_t addressLength = sizeof(clientAddress);
        ssize_t bytesReceived = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientAddress, &addressLength);

        if (bytesReceived < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
        {
            continue;
        }
        else
        {
            buffer[bytesReceived] = '\0';

            if (strncmp(buffer, "name ", 5) == 0)
            {
                for (int i = 0; i < MAX_SESSIONS; i++)
                {
                    if (sessions[i].numClients < 2)
                    {
                        char nickname[MAX_NICKNAME_LEN];
                        Client newClient;
                        strncpy(newClient.name, nickname, MAX_NICKNAME_LEN);
                        newClient.playerNumber = sessions[i].numClients;
                        newClient.address = clientAddress;
                        newClient.sessionId = i;
                        sessions[i].clients[sessions[i].numClients] = newClient;

                        char playerNumber[2];
                        snprintf(playerNumber, sizeof(playerNumber), "%d", newClient.playerNumber);
                        ssize_t bytesSent = sendto(serverSocket, playerNumber, strlen(playerNumber), 0, (struct sockaddr *)&newClient.address, sizeof(newClient.address));
                        if (bytesSent == -1)
                        {
                            perror("Send error");
                        }

                        sessions[i].numClients++;

                        printf("Client %s connected as Player %d, %d\n", newClient.name, newClient.playerNumber, sessions[i].sessionId);

                        if (sessions[i].numClients == 2)
                        {
                            sessions[i].gameStarted = 1;
                            printf("Game started in session %d!\n", sessions[i].sessionId);

                            pthread_t GameLogicAndBroadcastThread;
                            pthread_create(&GameLogicAndBroadcastThread, NULL, GameLogicAndBroadcast, &sessions[i]);
                        }
                        break;
                    }
                    else
                    {
                        if (i == MAX_SESSIONS - 1)
                        {
                            printf("No space available. Ignoring client request.\n");
                        }
                    }
                }
            }
            else
            {
                int numSession;


                for (int i = 0; i < MAX_SESSIONS; i++)
                {
                    for (int j = 0; j < 2; j++)
                    {
                        if (memcmp(&clientAddress.sin_addr, &sessions[i].clients[j].address.sin_addr, sizeof(struct in_addr)) == 0 && clientAddress.sin_port == sessions[i].clients[j].address.sin_port)

                        {

                            numSession = i;
                            printf("Se escogio %d\n", i);
                            break;
                        }
                    }
                }


                int number, player;
                if (sscanf(buffer, "Move %d %d", &number, &player) == 2)
                {
                    if (player == 0 || player == 1)
                    {
                        printf("Player %d with number %d in session %d, moved a paddle\n", player, number, numSession);

                        sessions[numSession].gameState = MovePaddle(number, player, sessions[numSession].gameState);
                    }

                }
            }
        }
    }

    close(serverSocket);
    return 0;
}
