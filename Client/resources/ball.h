#ifndef BALL_H
#define BALL_H

#include "config.h"
#include "game.h"
#include "collision.h"

GameState MoveBall(GameState game)
{

    /* Move the ball by its motion vector. */
    game.ballX += game.ballDx;
    game.ballY += game.ballDy;

    /* Turn the ball around if it hits the edge of the screen. */
    if (game.ballX < 0)
    {

        game.score2 += 1;
        game = InitGame(game);
    }

    if (game.ballX > screen->w - 10)
    {

        game.score1 += 1;
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

#endif // BALL_H