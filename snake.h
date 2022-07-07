#ifndef __SNAKE_H__
#define __SNAKE_H__

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <Windows.h>
#include <time.h>
#include <vector>

#include "config.h"
#include "resource.h"

using namespace std;

struct Point
{
	int x;
	int y;

	bool operator == (Point point)
	{
		return this->x == point.x && this->y == point.y;
	}
};

struct Head
{
	int x;
	int y;
	int next;
	int nextLast;

	bool operator == (Point point)
	{
		return this->x == point.x && this->y == point.y;
	}
};

struct Image
{
	SDL_PixelFormat* format;
	SDL_Surface* surface;
	SDL_Surface* background;
	SDL_Surface* snake;
	SDL_Surface* food;
};

struct Rect
{
	SDL_Rect screen;
	SDL_Rect block;
};

typedef Point Body;
typedef Point Food;

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

class MainGame
{
	public:
		HINSTANCE hInstance;
		SDL_Window* window;
		SDL_Event events;
		TTF_Font* font;
		SDL_TimerID mainInterval;
		const Uint8* keyStatus;

	public:
		Image image;
		Rect rect;

	public:
		Snake snake;
		vector <Food> food;

	public:
		int score;
		int status;

	public:
		SDL_RWops* getResource(HINSTANCE, LPCWSTR, LPCWSTR);
		SDL_Surface* loadSurface(int);

	public:
		void initWindow();
		void initGame();
		void loadImage();
		void loadFont();
		void freeImage();
		void freeFont();
		void startMainInterval();
		void endMainInterval();
		void close();

	public:
		void addFood();
		void update();
		void control();
		void displayText(const char*, int, int);
		void displayBlock(SDL_Surface*, int, int);
		void displayInfo();
		void displayFood();
		void display();
};

extern MainGame game;
#endif