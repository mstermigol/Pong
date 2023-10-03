
// Using libs SDL, glibc
#include "SDL2/SDL.h" //SDL version 2.0
#include <stdlib.h>
#include <stdio.h>

#include "resources/config.h"
#include "resources/score.h"
#include "resources/game.h"
#include "resources/collision.h"
#include "resources/ball.h"
#include "resources/paddle.h"
#include "resources/draw.h"

int Init(int w, int h, int argc, char *args[]);

SDL_Window *window = NULL;
SDL_Renderer *renderer;
SDL_Texture *screenTexture;
SDL_Surface *screen;
SDL_Surface *title;
SDL_Surface *end;
SDL_Surface *numbermap;

int Init(int width, int height, int argc, char *args[])
{

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{

		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());

		return 1;
	}

	int i;

	for (i = 0; i < argc; i++)
	{

		// Create window
		if (strcmp(args[i], "-f"))
		{

			SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN, &window, &renderer);
		}
		else
		{

			SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN_DESKTOP, &window, &renderer);
		}
	}

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

static void DrawGameOver(int p)
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

static void DrawMenu()
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

static void DrawNet()
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

static void DrawBall(GameState game)
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

static void DrawPaddle(GameState game)
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

static void DrawPlayer1Score(GameState game)
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

static void DrawPlayer2Score(GameState game)
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

int main(int argc, char *args[])
{

	// SDL Window setup
	if (Init(SCREEN_WIDTH, SCREEN_HEIGHT, argc, args) == 1)
	{

		return 0;
	}

	// get the size of the window
	int width, height;
	SDL_GetWindowSize(window, &width, &height);

	int sleep = 0;
	int quit = 0;
	int state = 0;
	int r = 0;
	int moveY;
	Uint32 nextGameTick = SDL_GetTicks();

	GameState game;

	// Initialize the ball position data.
	game = InitGame(game);

	// render loop
	while (quit == 0)
	{

		// check for new events every frame
		SDL_PumpEvents();

		const Uint8 *keystate = SDL_GetKeyboardState(NULL);

		if (keystate[SDL_SCANCODE_ESCAPE])
		{

			quit = 1;
		}

		if (keystate[SDL_SCANCODE_DOWN])
		{

			game = MovePaddle(0, 1, game);
		}

		if (keystate[SDL_SCANCODE_S])
		{

			game = MovePaddle(0, 0, game);
		}

		if (keystate[SDL_SCANCODE_UP])
		{

			game = MovePaddle(1, 1, game);
		}

		if (keystate[SDL_SCANCODE_W])
		{

			game = MovePaddle(1, 0, game);
		}

		// draw background
		SDL_RenderClear(renderer);
		SDL_FillRect(screen, NULL, 0x000000ff);

		// display main menu
		if (state == 0)
		{

			if (keystate[SDL_SCANCODE_SPACE])
			{

				state = 1;
			}

			// draw menu
			DrawMenu();

			// display gameover
		}
		else if (state == 2)
		{

			if (keystate[SDL_SCANCODE_SPACE])
			{
				state = 0;
				// delay for a little bit so the space bar press dosnt get triggered twice
				// while the main menu is showing
				SDL_Delay(500);
			}

			// draw game over screen
			DrawGameOver(r);

			// display the game
		}
		else if (state == 1)
		{

			// check score
			r = CheckScore(game.score1, game.score2);

			// if either player wins, change to game over state
			if (r == 1)
			{

				state = 2;
			}
			else if (r == 2)
			{

				state = 2;
			}

			//* Move the balls for the next frame.
			game = MoveBall(game);

			// draw net
			DrawNet();

			// draw paddles
			DrawPaddle(game);

			//* Put the ball on the screen.
			DrawBall(game);

			// draw the score
			DrawPlayer1Score(game);

			// draw the score
			DrawPlayer2Score(game);
		}

		SDL_UpdateTexture(screenTexture, NULL, screen->pixels, screen->w * sizeof(Uint32));
		SDL_RenderCopy(renderer, screenTexture, NULL, NULL);

		// draw to the display
		SDL_RenderPresent(renderer);

		// time it takes to render  frame in milliseconds
		nextGameTick += 1000 / 60;
		sleep = nextGameTick - SDL_GetTicks();

		if (sleep >= 0)
		{

			SDL_Delay(sleep);
		}
	}

	// free loaded images
	SDL_FreeSurface(screen);
	SDL_FreeSurface(title);
	SDL_FreeSurface(numbermap);
	SDL_FreeSurface(end);

	// free renderer and all textures used with it
	SDL_DestroyRenderer(renderer);

	// destinationroy window
	SDL_DestroyWindow(window);

	// Quit SDL subsystems
	SDL_Quit();

	return 0;
}