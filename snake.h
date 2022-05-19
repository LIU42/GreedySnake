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

class Window
{
	public:
		HINSTANCE hinstance;
		SDL_Window* window;
		SDL_Event events;
		TTF_Font* font;
		SDL_Rect screen_rect;
		SDL_Rect block_rect;
		SDL_PixelFormat* format;
		const Uint8* keystatus;

	public:
		SDL_Surface* surface;
		SDL_Surface* background;
		SDL_Surface* snake_body;
		SDL_Surface* food_red;

	public:
		SDL_RWops* get_resource(HINSTANCE, LPCWSTR, LPCWSTR);
		SDL_Surface* load_surface(DWORD);

	public:
		void text(const char*, int, int);
		void block(SDL_Surface*, int, int);
		void init();
		void load_image();
		void load_font();
		void free_image();
		void free_font();
		void close();
};

class Game
{
	public:
		SDL_TimerID main_interval;
		int score;
		int status;
		char text[TEXT_MAX_LEN];

	public:
		default_random_engine random;
		uniform_int_distribution <int> rand_X;
		uniform_int_distribution <int> rand_Y;

	public:
		Game();
		void init();
		void add_food(int);
		void start_main_interval();
		void end_main_interval();
		void update();
		void events();
		void display_info();
		void display_food();
		void display();
};

class Snake
{
	public:
		Head head;
		vector <Body> body;
		Body temp;
		bool alive;

	public:
		void init();
		void move();
		void crash();
		void eat();
		void display();
};

extern Window window;
extern Game game;
extern Snake snake;
extern vector <Food> food;
#endif