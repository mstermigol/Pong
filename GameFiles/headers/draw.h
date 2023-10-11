#ifndef DRAW_H
#define DRAW_H

#include "gamestate.h"
#include "config.h"
#include <SDL2/SDL.h>

SDL_Window *window = NULL;
SDL_Renderer *renderer;
SDL_Texture *screenTexture;
SDL_Surface *screen;
SDL_Surface *title;
SDL_Surface *end;
SDL_Surface *numbermap;
SDL_Texture *screen_texture;

int Init(int width, int height);
void DrawMenu();
void DrawNet();
void DrawBall(GameState game);
void DrawPaddle(GameState game);
void DrawPlayer1Score(GameState game);
void DrawPlayer2Score(GameState game);
void DrawGameOver(int p);

#endif // DRAW_H