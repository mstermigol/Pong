#include "headers/game.h"

Paddle paddle[2];

static void MovePaddle(int d, int pad)
{

    // if the down arrow is pressed move paddle down
    if (d == 0)
    {
        if (paddle[pad].y >= screen->h - paddle[pad].h)
        {

            paddle[pad].y = screen->h - paddle[pad].h;
        }
        else
        {

            paddle[pad].y += 5;
        }
    }

    // if the up arrow is pressed move paddle up
    if (d == 1)
    {
        if (paddle[pad].y <= 0)
        {

            paddle[pad].y = 0;
        }
        else
        {

            paddle[pad].y -= 5;
        }
    }
}