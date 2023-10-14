#include "../headers/gamestate.h"
#include "../headers/config.h"
#include <stdio.h>

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