
// Using libs SDL, glibc
#include "SDL2/SDL.h" //SDL version 2.0
#include <stdlib.h>
#include <stdio.h>

#include "resources/config.h"
#include "resources/score.h"
#include "resources/game.h"

// function prototypes
// initilise SDL
int init(int w, int h, int argc, char *args[]);

// Program globals

SDL_Window *window = NULL; // The window we'll be rendering to
SDL_Renderer *renderer;	   // The renderer SDL will use to draw to the screen

// surfaces
static SDL_Surface *screen;
static SDL_Surface *title;
static SDL_Surface *numbermap;
static SDL_Surface *end;

// textures
SDL_Texture *screenTexture;

// inisilise starting position and sizes of game elemements
GameState InitGame(GameState game)
{

	game.ballX = screen->w / 2;
	game.ballY = screen->h / 2;
	game.ballDy = 1;
	game.ballDx = 1;

	game.paddle1Y = screen->h / 2 - 50;
	game.paddle2Y = screen->h / 2 - 50;

	return game;
}

// if return value is 1 collision occured. if return is 0, no collision.
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
	rightOfPaddle = paddleY + PADDLE_WIDTH;
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

/* This routine moves each ball by its motion vector. */
GameState MoveBall(GameState game)
{

	/* Move the ball by its motion vector. */
	game.ballX += game.ballDx;
	game.ballY += game.ballDy;

	/* Turn the ball around if it hits the edge of the screen. */
	if (game.ballX < 0)
	{

		game.score1 += 1;
		game = InitGame(game);
	}

	if (game.ballX > screen->w - 10)
	{

		game.score2 += 1;
		game = InitGame(game);
	}

	if (game.ballY < 0 || game.ballY > screen->h - 10)
	{

		game.ballDy = -game.ballDy;
	}

	// check for collision with the paddle
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

		// collision detected
		if (collision == 1)
		{

			// ball moving left
			if (game.ballDx < 0)
			{

				game.ballDx -= 1;

				// ball moving right
			}
			else
			{

				game.ballDx += 1;
			}

			// change ball direction
			game.ballDx = -game.ballDx;

			// change ball angle based on where on the paddle it hit
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

			// ball moving right
			if (game.ballDx > 0)
			{

				// teleport ball to avoid mutli collision glitch
				if (game.ballX < 30)
				{

					game.ballX = 30;
				}

				// ball moving left
			}
			else
			{

				// teleport ball to avoid mutli collision glitch
				if (game.ballX > 600)
				{

					game.ballX = 600;
				}
			}
		}
	}

	return game;
}

GameState MovePaddle(int d, int pad, GameState game)
{

	if (pad == 0)
	{
		if (d == 0)
		{

			if (game.paddle1Y >= screen->h - PADDLE_HEIGHT)
			{

				game.paddle1Y = screen->h - PADDLE_HEIGHT;
			}
			else
			{

				game.paddle1Y += 5;
			}
		}

		// if the up arrow is pressed move paddle up
		if (d == 1)
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
		if (d == 0)
		{

			if (game.paddle2Y >= screen->h - PADDLE_HEIGHT)
			{

				game.paddle2Y = screen->h - PADDLE_HEIGHT;
			}
			else
			{

				game.paddle2Y += 5;
			}
		}

		// if the up arrow is pressed move paddle up
		if (d == 1)
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
	if (init(SCREEN_WIDTH, SCREEN_HEIGHT, argc, args) == 1)
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

int init(int width, int height, int argc, char *args[])
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
