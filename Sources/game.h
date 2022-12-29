#ifndef __GAME_H__
#define __GAME_H__

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <Windows.h>
#include <time.h>
#include <list>

#include "snake.h"
#include "resource.h"

using namespace std;

enum Status { START, PLAYING, PAUSE, OVER, EXIT };

struct Image
{
	SDL_PixelFormat* format;
	SDL_Surface* surface;
	SDL_Surface* background;
	SDL_Surface* snake;
	SDL_Surface* food;
};

class MainGame
{
	public:
		static const int SCREEN_WIDTH = 620;
		static const int SCREEN_HEIGHT = 440;
		static const int INTERVAL = 250;
		static const int CONTROL_DELAY = 10;
		static const int EAT_SCORE = 10;

	public:
		static const int TABLE_ROWS = 30;
		static const int TABLE_COLS = 20;
		static const int BLOCK_SIZE = 20;
		static const int BORDER = 10;

	public:
		static const int FONT_SIZE = 14;
		static const int TEXT_BORDER = 4;
		static const int TEXT_LENGTH = 30;
		static const int INFO_LENGTH_MARGIN = 230;
		static const int INFO_SCORE_MARGIN = 110;

	private:
		HINSTANCE hInstance;
		SDL_Window* window;
		SDL_Rect screen;
		SDL_Event event;
		TTF_Font* font;
		SDL_TimerID mainInterval;
		const Uint8* keyStatus;

	private:
		Image image;
		Snake snake;
		list <Food> food;
		Status status;
		int score;

	private:
		SDL_RWops* getResource(HINSTANCE, LPCWSTR, LPCWSTR);
		SDL_Surface* loadSurface(int);

	private:
		void freeImage();
		void freeFont();
		void endMainInterval();

	private:
		void addFood();
		void snakeCrash();
		void snakeEat();

	private:
		void displayText(const char*, int, int);
		void displayBlock(SDL_Surface*, int, int);
		void displayInfo();
		void displayFood();
		void displaySnake();

	public:
		void initWindow();
		void initGame();
		void loadImage();
		void loadFont();
		void startMainInterval();

	public:
		bool isRunning();
		void close();
		void update();
		void events();
		void display();
};
#endif