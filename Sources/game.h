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
	SDL_Surface* pBackground;
	SDL_Surface* pSnake;
	SDL_Surface* pFood;
};

struct Fonts
{
	TTF_Font* pInfo;
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
		SDL_Window* pWindow;
		SDL_Surface* pSurface;
		SDL_PixelFormat* pFormat;
		SDL_SysWMinfo windowInfo;
		SDL_Rect screenRect;
		SDL_Event event;
		const Uint8* pKeyStatus;

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
		void getVersion();
		void initSystem();
		void initWindow();
		void initGame();
		void loadImage();
		void loadFont();
		void startMainInterval();

	private:
		void freeImage();
		void freeFont();
		void endMainInterval();
		void closeWindow();
		void closeSystem();

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
		MainGame();
		~MainGame();

	public:
		bool isRunning();
		void update();
		void events();
		void display();
		void delay();
};
#endif