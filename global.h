#ifndef __GLOBAL_H__
#define __GLOBAL_H__

using namespace std;

HINSTANCE hinstance;
SDL_Window* window;
SDL_Event event;
TTF_Font* font;

SDL_Rect screen_rect;
SDL_Rect block_rect;

SDL_Surface* surface;
SDL_Surface* background;
SDL_Surface* snake_body;
SDL_Surface* food_red;

SDL_TimerID main_interval;
SDL_PixelFormat* format;

default_random_engine random((unsigned)(time(NULL)));
uniform_int_distribution <int> randx(0, table_x_max - 1);
uniform_int_distribution <int> randy(0, table_y_max - 1);

Game game;
Snake snake;
vector <Food> food;
#endif