#ifndef __CONFIG_H__
#define __CONFIG_H__

#define WINDOW_TITLE "Snake"
#define SCREEN_WIDTH 720
#define SCREEN_HEIGHT 460
#define GAME_INTERVAL 250
#define USER_INPUT_DELAY 10
#define IMG_FORMAT SDL_PIXELFORMAT_RGBA32

#define TABLE_WIDTH 700
#define TABLE_HEIGHT 420
#define TABLE_X_MAX 50
#define TABLE_Y_MAX 30
#define BLOCK_WIDTH 14
#define BORDER 10

#define FONT_SIZE 16
#define TEXT_RECT_WIDTH 200
#define TEXT_RECT_HEIGHT 20
#define TEXT_BORDER 4

#define START 0
#define PLAYING 1
#define PAUSE 2
#define GAMEOVER 3

#define SNAKE_INIT_LENGTH 10
#define FOOD_MAX_COUNT 1
#define EAT_SCORE 10

#define UP 1
#define DOWN -1
#define LEFT 2
#define RIGHT -2
#endif