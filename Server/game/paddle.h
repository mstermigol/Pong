#include "config.h"
#include "game.h"

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

        // if the up arrow is pressed move paddle up
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

        // if the up arrow is pressed move paddle up
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