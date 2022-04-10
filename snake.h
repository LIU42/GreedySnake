#ifndef __SNAKE_H__
#define __SNAKE_H__

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <Windows.h>
#include <vector>
#include <random>

#include "config.h"
#include "resource.h"

using namespace std;

struct Position
{
	int x;
	int y;
	bool operator== (Position pos) { return this->x == pos.x && this->y == pos.y; }
};

struct Head
{
	int x;
	int y;
	int next;
	int next_last;
	bool operator== (Position pos) { return this->x == pos.x && this->y == pos.y; }
};

typedef Position Body;
typedef Position Food;

class Snake
{
	public:
		Head head;
		vector <Body> body;
		Body temp;
		bool alive;

	public:
		Snake();
		void init();
		void move();
		void crash();
		void eat();
		void display();
};

class Game
{
	public:
		int score;
		int status;
		const Uint8* keystatus;
	
	public:
		HINSTANCE hinstance;
		SDL_Window* window;
		SDL_Event event;
		TTF_Font* font;
		SDL_Rect screen_rect;
		SDL_Rect block_rect;

	public:
		SDL_Surface* surface;
		SDL_Surface* background;
		SDL_Surface* snake_body;
		SDL_Surface* food_red;

	public:
		SDL_TimerID main_interval;
		SDL_PixelFormat* format;

	public:
		default_random_engine random;
		uniform_int_distribution <int> rand_X;
		uniform_int_distribution <int> rand_Y;

	public:
		Snake snake;
		vector <Food> food;

	public:
		Game();
		void init_game();
		void init_window();
		void init_font();
		void load_image();
		void exit_game();
		void add_food(int);
		void start_main_interval();
		void update();
		void events();
		void display();
};

extern Game game;
#endif