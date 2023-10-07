
// Using libs SDL, glibc
#include "SDL2/SDL.h" //SDL version 2.0
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <ncurses.h>
#include <pthread.h>
#include <sys/socket.h>

#define MAX_NICKNAME_LENGTH 20

#define PADDLE_1_X 20
#define PADDLE_2_X SCREEN_WIDTH - 20 - PADDLE_WIDTH

#define PADDLE_WIDTH 10
#define PADDLE_HEIGHT 100

#define BALL_WIDTH 10
#define BALL_HEIGHT 10

int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;

typedef struct
{
	int ballX, ballY;
	int ballDx, ballDy;
	int paddle1Y, paddle2Y;
	int score1, score2;
} GameState;

SDL_Window *window = NULL;
SDL_Renderer *renderer;
SDL_Texture *screenTexture;
SDL_Surface *screen;
SDL_Surface *title;
SDL_Surface *end;
SDL_Surface *numbermap;
SDL_Texture *screen_texture;

int Init(int width, int height);
void DrawMenu();
void DrawNet();
void DrawBall(GameState game);
void DrawPaddle(GameState game);
void DrawPlayer1Score(GameState game);
void DrawPlayer2Score(GameState game);
void DrawGameOver(int p);

GameState deserializeGameState(char *buffer, GameState game)
{
	GameState aux;

	printf("Buffer: %s\n", buffer);

	if (sscanf(buffer, "GameState %d %d %d %d %d %d %d %d", &aux.ballX, &aux.ballY, &aux.ballDx, &aux.ballDy,
			   &aux.paddle1Y, &aux.paddle2Y, &aux.score1, &aux.score2) == 8)
	{
		game.ballX = aux.ballX;
		game.ballY = aux.ballY;
		game.ballDx = aux.ballDx;
		game.ballDy = aux.ballDy;
		game.paddle1Y = aux.paddle1Y;
		game.paddle2Y = aux.paddle2Y;
		game.score1 = aux.score1;
		game.score2 = aux.score2;
		printf("GameState received\n");
	}
	return game;
}

int Init(int width, int height)
{

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{

		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());

		return 1;
	}

	int i;

	SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN, &window, &renderer);

	if (window == NULL)
	{

		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());

		return 1;
	}

	// create the screen sruface where all the elemnts will be drawn onto (ball, paddles, net etc)
	screen = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA32);

	if (screen == NULL)
	{

		printf("Could not create the screen surfce! SDL_Error: %s\n", SDL_GetError());

		return 1;
	}

	// create the screen texture to render the screen surface to the actual display
	screenTexture = SDL_CreateTextureFromSurface(renderer, screen);

	if (screenTexture == NULL)
	{

		printf("Could not create the screenTexture! SDL_Error: %s\n", SDL_GetError());

		return 1;
	}

	// Load the title image
	title = SDL_LoadBMP("assets/title.bmp");

	if (title == NULL)
	{

		printf("Could not Load title image! SDL_Error: %s\n", SDL_GetError());

		return 1;
	}

	// Load the numbermap image
	numbermap = SDL_LoadBMP("assets/numbermap.bmp");

	if (numbermap == NULL)
	{

		printf("Could not Load numbermap image! SDL_Error: %s\n", SDL_GetError());

		return 1;
	}

	// Load the gameover image
	end = SDL_LoadBMP("assets/gameover.bmp");

	if (end == NULL)
	{

		printf("Could not Load title image! SDL_Error: %s\n", SDL_GetError());

		return 1;
	}

	// Set the title colourkey.
	Uint32 colorkey = SDL_MapRGB(title->format, 255, 0, 255);
	SDL_SetColorKey(title, SDL_TRUE, colorkey);
	SDL_SetColorKey(numbermap, SDL_TRUE, colorkey);

	return 0;
}

void DrawGameOver(int p)
{

	SDL_Rect player1;
	SDL_Rect player2;
	SDL_Rect cpu;
	SDL_Rect destination;

	player1.x = 0;
	player1.y = 0;
	player1.w = end->w;
	player1.h = 75;

	player2.x = 0;
	player2.y = 75;
	player2.w = end->w;
	player2.h = 75;

	cpu.x = 0;
	cpu.y = 150;
	cpu.w = end->w;
	cpu.h = 75;

	destination.x = (screen->w / 2) - (end->w / 2);
	destination.y = (screen->h / 2) - (75 / 2);
	destination.w = end->w;
	destination.h = 75;

	switch (p)
	{

	case 1:
		SDL_BlitSurface(end, &player1, screen, &destination);
		break;
	case 2:
		SDL_BlitSurface(end, &player2, screen, &destination);
		break;
	default:
		SDL_BlitSurface(end, &cpu, screen, &destination);
	}
}

void DrawMenu()
{

	SDL_Rect source;
	SDL_Rect destination;

	source.x = 0;
	source.y = 0;
	source.w = title->w;
	source.h = title->h;

	destination.x = (screen->w / 2) - (source.w / 2);
	destination.y = (screen->h / 2) - (source.h / 2);
	destination.w = title->w;
	destination.h = title->h;

	SDL_BlitSurface(title, &source, screen, &destination);
}

void DrawNet()
{

	SDL_Rect net;

	net.x = screen->w / 2;
	net.y = 20;
	net.w = 5;
	net.h = 15;

	// draw the net
	int i, r;

	for (i = 0; i < 15; i++)
	{

		r = SDL_FillRect(screen, &net, 0xffffffff);

		if (r != 0)
		{

			printf("fill rectangle faliled in func DrawNet()");
		}

		net.y = net.y + 30;
	}
}

void DrawBall(GameState game)
{

	SDL_Rect source;

	source.x = game.ballX;
	source.y = game.ballY;
	source.w = BALL_WIDTH;
	source.h = BALL_HEIGHT;

	int r = SDL_FillRect(screen, &source, 0xffffffff);

	if (r != 0)
	{

		printf("fill rectangle faliled in func drawball()");
	}
}

void DrawPaddle(GameState game)
{

	SDL_Rect source;
	int i;

	for (i = 0; i < 2; i++)
	{
		if (i == 0)
		{
			source.x = PADDLE_1_X;
			source.y = game.paddle1Y;
		}
		else
		{
			source.x = PADDLE_2_X;
			source.y = game.paddle2Y;
		}

		source.w = PADDLE_WIDTH;
		source.h = PADDLE_HEIGHT;

		int r = SDL_FillRect(screen, &source, 0xffffffff);

		if (r != 0)
		{

			printf("fill rectangle faliled in func DrawPaddle()");
		}
	}
}

void DrawPlayer1Score(GameState game)
{

	SDL_Rect source;
	SDL_Rect destination;

	source.x = 0;
	source.y = 0;
	source.w = 64;
	source.h = 64;

	destination.x = (screen->w / 2) - source.w - 12; // 12 is just padding spacing
	destination.y = 0;
	destination.w = 64;
	destination.h = 64;

	if (game.score1 > 0 && game.score1 < 10)
	{

		source.x += source.w * game.score1;
	}

	SDL_BlitSurface(numbermap, &source, screen, &destination);
}

void DrawPlayer2Score(GameState game)
{

	SDL_Rect source;
	SDL_Rect destination;

	source.x = 0;
	source.y = 0;
	source.w = 64;
	source.h = 64;

	destination.x = (screen->w / 2) + 12;
	destination.y = 0;
	destination.w = 64;
	destination.h = 64;

	if (game.score2 > 0 && game.score2 < 10)
	{

		source.x += source.w * game.score2;
	}

	SDL_BlitSurface(numbermap, &source, screen, &destination);
}

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

	char nickname[MAX_NICKNAME_LENGTH];
	strncpy(nickname, argv[3], MAX_NICKNAME_LENGTH);

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

	int sleep = 0;
	int quit = 0;
	int state = 0;
	int r = 0;
	Uint32 next_game_tick = SDL_GetTicks();

	SDL_Surface *screen2 = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);

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

				// Clear the screen surface
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