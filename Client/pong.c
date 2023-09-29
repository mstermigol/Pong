
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