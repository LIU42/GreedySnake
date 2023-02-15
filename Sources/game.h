#ifndef __GAME_H__
#define __GAME_H__

#include <SDL.h>
#include <SDL_syswm.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <time.h>
#include <string>

#include "snake.h"
#include "resource.h"

enum Status { START, PLAYING, PAUSE, OVER, EXIT };

struct Images
{
	SDL_Surface* background;
	SDL_Surface* snake;
	SDL_Surface* food;
};

struct Fonts
{
	TTF_Font* info;
};

struct Timers
{
	SDL_TimerID mainInterval;
};

class MainGame
{
	private:
		static constexpr auto TITLE = "Snake";

	private:
		static const int SCREEN_WIDTH = 620;
		static const int SCREEN_HEIGHT = 440;
		static const int INTERVAL = 250;
		static const int CONTROL_DELAY = 10;
		static const int EAT_SCORE = 10;

	private:
		static const int TABLE_ROWS = 30;
		static const int TABLE_COLS = 20;
		static const int BLOCK_SIZE = 20;
		static const int BORDER = 10;

	private:
		static const int FONT_SIZE = 14;
		static const int TEXT_BORDER = 4;
		static const int TEXT_LENGTH = 30;
		static const int INFO_LENGTH_MARGIN = 230;
		static const int INFO_SCORE_MARGIN = 110;

	private:
		static constexpr SDL_Color BLACK = { 0, 0, 0 };

	private:
		SDL_Window* window;
		SDL_Surface* surface;
		SDL_PixelFormat* format;
		SDL_SysWMinfo sysInfo;
		SDL_Rect screenRect;
		SDL_Event event;
		const Uint8* keyStatus;

	private:
		Images images;
		Fonts fonts;
		Timers timers;

	private:
		Snake snake;
		FoodList foodList;
		Status status;
		int score;

	private:
		static Uint32 mainIntervalCallback(Uint32, void*);

	private:
		SDL_RWops* getResource(LPCWSTR, LPCWSTR);
		SDL_Surface* loadSurface(Uint32);

	private:
		void freeImage();
		void freeFont();
		void endMainInterval();
		void closeWindow();
		void closeEnvironment();

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
		void initEnvironment();
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
		void delay();
};
#endif