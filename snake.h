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
	int nextLast;
	bool operator== (Position pos) { return this->x == pos.x && this->y == pos.y; }
};

typedef Position Body;
typedef Position Food;

class Window
{
	public:
		HINSTANCE hInstance;
		SDL_Window* window;
		SDL_Event events;
		TTF_Font* font;
		SDL_Rect screenRect;
		SDL_Rect blockRect;
		SDL_PixelFormat* format;
		const Uint8* keyStatus;

	public:
		SDL_Surface* surface;
		SDL_Surface* backgroundImg;
		SDL_Surface* snakeImg;
		SDL_Surface* foodImg;

	public:
		SDL_RWops* getResource(HINSTANCE, LPCWSTR, LPCWSTR);
		SDL_Surface* loadSurface(DWORD);

	public:
		void text(const char*, int, int);
		void block(SDL_Surface*, int, int);
		void init();
		void loadImage();
		void loadFont();
		void freeImage();
		void freeFont();
		void close();
};

class Game
{
	public:
		SDL_TimerID mainInterval;
		int score;
		int status;
		char text[TEXT_MAX_LEN];

	public:
		default_random_engine random;
		uniform_int_distribution <int> randX;
		uniform_int_distribution <int> randY;

	public:
		Game();
		void init();
		void addFood(int);
		void startMainInterval();
		void endMainInterval();
		void update();
		void events();
		void displayInfo();
		void displayFood();
		void display();
};

class Snake
{
	public:
		Head head;
		vector <Body> body;
		Body temp;
		bool isAlive;

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