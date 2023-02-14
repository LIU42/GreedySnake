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

struct Font
{
	TTF_Font* info;
};

struct Timer
{
	SDL_TimerID mainInterval;
};

class MainGame
{
	public:
		static constexpr auto TITLE = "Snake";

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

	public:
		static constexpr SDL_Color BLACK = { 0, 0, 0 };

	private:
		SDL_Window* window;
		SDL_SysWMinfo sysInfo;
		SDL_Rect screen;
		SDL_Event event;
		const Uint8* keyStatus;

	private:
		Image image;
		Font font;
		Timer timer;

	private:
		Snake snake;
		FoodList food;
		Status status;
		int score;

	private:
		static Uint32 mainIntervalCallback(Uint32, void*);

	private:
		SDL_RWops* getResource(LPCWSTR, LPCWSTR);
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