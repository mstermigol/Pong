#include "config.h"

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