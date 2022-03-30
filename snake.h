#ifndef __SNAKE_H__
#define __SNAKE_H__

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <Windows.h>
#include <vector>
#include <random>

#include "config.h"

using namespace std;

struct Body
{
	int x;
	int y;
	bool operator== (Body b) { return this->x == b.x && this->y == b.y; }
};

struct Head
{
	int x;
	int y;
	int next;
	int next_last;
	bool operator== (Body b) { return this->x == b.x && this->y == b.y; }
};

typedef Body Food;
typedef Body Position;

class Game
{
	public:
		int score;
		int status;
		const Uint8* keystatus;

	public:
		void init_game();
		void init_window();
		void init_font();
		void load_image();
		void start_main_interval();
		void update();
		void events();
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
		Snake();
		void init();
		void move();
		void crash();
		void eat();
		void display();
};

extern Game game;
#endif