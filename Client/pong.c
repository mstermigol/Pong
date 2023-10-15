#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include "../Protocol/scc.h"
#include "../GameFiles/headers/gamestate.h"
#include "../GameFiles//headers/config.h"
#include "../GameFiles//headers/draw.h"
#include "../GameFiles//headers/client.h"

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

	fd_set readFileDescriptors;
	int maxFileDescriptors = clientSocket + 1;

	GameState game;

	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(atoi(argv[2]));
	if (inet_pton(AF_INET, argv[1], &serverAddress.sin_addr) <= 0)
	{
		perror("Invalid server IP address");
		close(clientSocket);
		exit(1);
	}

	char nickname[MAX_NICKNAME_LEN];
	strncpy(nickname, argv[3], MAX_NICKNAME_LEN);

	char *initialMessage = SendName(nickname);

	if (sendto(clientSocket, initialMessage, strlen(initialMessage), 0,
			   (struct sockaddr *)&serverAddress, serverLen) < 0)
	{
		perror("Error sending data");
		close(clientSocket);
		exit(1);
	}

	if (Init(SCREEN_WIDTH, SCREEN_HEIGHT) == 1)
	{

		return 0;
	}
	int screenWidth = SCREEN_WIDTH;
	int screenHeight = SCREEN_HEIGHT;
	SDL_GetWindowSize(window, &screenWidth, &screenHeight);

	int gameStatus = 0;
	int playerNumber;

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
			if (bytesReceived < 0)
			{
				perror("Error receiving data");
				continue;
			}

			buffer[bytesReceived] = '\0';

			printf("buffer: %s\n", buffer);

			int protocolSelector = Receive(buffer);

			if (protocolSelector == 1)
			{
				gameStatus = 0;
				playerNumber = atoi(buffer + 7);
			}
			else if (protocolSelector == 2)
			{
				gameStatus = 1;
			}
			else if (protocolSelector == 3 && gameStatus == 1)
			{
				game = deserializeGameState(buffer, game);
				SDL_PumpEvents();

				const Uint8 *keystate = SDL_GetKeyboardState(NULL);

				if (keystate[SDL_SCANCODE_UP])
				{
					char *message = SendMove(1, playerNumber);
					sendto(clientSocket, message, strlen(message), 0, (struct sockaddr *)&serverAddress, serverLen);
					free(message);
					printf("Sent: %s\n", message);
				}

				if (keystate[SDL_SCANCODE_DOWN])
				{
					char *message = SendMove(0, playerNumber);
					sendto(clientSocket, message, strlen(message), 0, (struct sockaddr *)&serverAddress, serverLen);
					free(message);
				}

				SDL_RenderClear(renderer);
				SDL_FillRect(screen, NULL, 0x00000000);

				DrawNet(game);

				DrawPaddle(game);

				DrawBall(game);

				DrawPlayer1Score(game);

				DrawPlayer2Score(game);

				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

				SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, screen);
				SDL_RenderCopy(renderer, texture, NULL, NULL);
				SDL_DestroyTexture(texture);

				SDL_RenderPresent(renderer);
			}
			else if (protocolSelector == 4)
			{
				printf("Error: received move message from server\n");
			}
			else if (protocolSelector == 5)
			{
				int winner = atoi(buffer + 4);
				printf("winner: %d\n", winner);

				SDL_RenderClear(renderer);
				SDL_FillRect(screen, NULL, 0x00000000);

				DrawGameOver(winner);

				SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, screen);
				SDL_RenderCopy(renderer, texture, NULL, NULL);
				SDL_DestroyTexture(texture);

				SDL_RenderPresent(renderer);

				usleep(5000000);
				break;
			}
			else if (strncmp(buffer, "GameState ", 9) == 0)
			{
				game = deserializeGameState(buffer, game);
			}
			else
			{
				printf("Error: received unknown message from server\n");
			}
			if (gameStatus == 0)
			{
				SDL_RenderClear(renderer);
				SDL_FillRect(screen, NULL, 0x00000000);

				DrawMenu();

				SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, screen);
				SDL_RenderCopy(renderer, texture, NULL, NULL);
				SDL_DestroyTexture(texture);

				SDL_RenderPresent(renderer);
			}
		}
	}

	SDL_FreeSurface(screen);
	SDL_DestroyRenderer(renderer);
	SDL_FreeSurface(title);
	SDL_FreeSurface(numbermap);
	SDL_FreeSurface(end);

	SDL_DestroyRenderer(renderer);

	SDL_DestroyWindow(window);

	SDL_Quit();

	close(clientSocket);

	return 0;
}
