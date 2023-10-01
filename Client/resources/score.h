#ifndef SCORE_H
#define SCORE_H

int CheckScore(int score1, int score2)
{

    int i;

    if (score1 == 5)
    {

        return 1;
    }
    else if (score2 == 5)
    {

        return 2;
    }
    else
    {

        return 0;
    }
}

#endif // SCORE_H