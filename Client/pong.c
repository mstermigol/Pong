#include <arpa/inet.h>
#include <sys/time.h>
#include "Protocol/scc.h"
#include "../headers/gamestate.h"
#include "../headers/config.h"
#include "../headers/draw.h"

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

	char initialMessage[1024];
	snprintf(initialMessage, sizeof(initialMessage), "name %s", nickname);

	if (sendto(clientSocket, initialMessage, strlen(initialMessage), 0,
			   (struct sockaddr *)&serverAddress, serverLen) < 0)
	{
		perror("Error sending data");
		close(clientSocket);
		exit(1);
	}

	char playerNumberAux[1024];
	int bytesReceived = recvfrom(clientSocket, playerNumberAux, sizeof(playerNumberAux), 0,
								 (struct sockaddr *)&serverAddress, &serverLen);

	int playerNumber = atoi(playerNumberAux);
	printf("Player number: %d\n", playerNumber);

	if (Init(SCREEN_WIDTH, SCREEN_HEIGHT) == 1)
	{

		return 0;
	}

	SDL_GetWindowSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

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
			// Receive game state from server
			char buffer[1024];
			int bytesReceived = recvfrom(clientSocket, buffer, sizeof(buffer), 0,
										 (struct sockaddr *)&serverAddress, &serverLen);
			if (bytesReceived < 0)
			{
				perror("Error receiving data");
				continue;
			}

			buffer[bytesReceived] = '\0';

			if (strncmp(buffer, "GameState ", 9) == 0)
			{
				game = deserializeGameState(buffer, game);

				SDL_PumpEvents();

				const Uint8 *keystate = SDL_GetKeyboardState(NULL);

				if (keystate[SDL_SCANCODE_UP])
				{
					char message[1024];
					snprintf(message, sizeof(message), "Move 1 %d", playerNumber);
					sendto(clientSocket, message, strlen(message), 0,
						   (struct sockaddr *)&serverAddress, serverLen);
					printf("Sent: %s\n", message);
				}

				if (keystate[SDL_SCANCODE_DOWN])
				{
					char message[1024];
					snprintf(message, sizeof(message), "Move 0 %d", playerNumber);
					sendto(clientSocket, message, strlen(message), 0,
						   (struct sockaddr *)&serverAddress, serverLen);
					printf("Sent: %s\n", message);
				}

				SDL_RenderClear(renderer);
				SDL_FillRect(screen, NULL, 0x00000000);

				DrawNet();

				// draw paddles
				DrawPaddle(game);

				//* Put the ball on the screen.
				DrawBall(game);

				// draw the score
				DrawPlayer1Score(game);

				// draw the score
				DrawPlayer2Score(game);

				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

				// Copy the screen surface to the renderer
				SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, screen);
				SDL_RenderCopy(renderer, texture, NULL, NULL);
				SDL_DestroyTexture(texture);

				// Present the renderer to the screen
				SDL_RenderPresent(renderer);
			}
		}
	}

	// Free the screen surface and renderer
	SDL_FreeSurface(screen);
	SDL_DestroyRenderer(renderer);
	SDL_FreeSurface(title);
	SDL_FreeSurface(numbermap);
	SDL_FreeSurface(end);

	// free renderer and all textures used with it
	SDL_DestroyRenderer(renderer);

	// destinationroy window
	SDL_DestroyWindow(window);

	// Quit SDL subsystems
	SDL_Quit();

	close(clientSocket);

	return 0;
}
