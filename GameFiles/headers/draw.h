#ifndef DRAW_H
#define DRAW_H

#include "gamestate.h"
#include "config.h"
#include <SDL2/SDL.h>

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *screenTexture;
extern SDL_Surface *screen;
extern SDL_Surface *title;
extern SDL_Surface *end;
extern SDL_Surface *numbermap;
extern SDL_Texture *screen_texture;

int Init(int width, int height);
void DrawMenu();
void DrawNet();
void DrawBall(GameState game);
void DrawPaddle(GameState game);
void DrawPlayer1Score(GameState game);
void DrawPlayer2Score(GameState game);
void DrawGameOver(int p);

#endif // DRAW_H