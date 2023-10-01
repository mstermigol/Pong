#ifndef PADDLE_H
#define PADDLE_H

#include "config.h"
#include <SDL2/SDL.h>
#include "game.h"

SDL_Surface *screen;

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

#endif // PADDLE_H