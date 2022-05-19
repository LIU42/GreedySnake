#ifndef __CONFIG_H__
#define __CONFIG_H__

#define WINDOW_TITLE "Snake"
#define SCREEN_WIDTH 620
#define SCREEN_HEIGHT 440
#define GAME_INTERVAL 250
#define USER_INPUT_DELAY 10
#define IMG_FORMAT SDL_PIXELFORMAT_RGBA32

#define TABLE_WIDTH 600
#define TABLE_HEIGHT 400
#define TABLE_X_MAX 30
#define TABLE_Y_MAX 20
#define BLOCK_WIDTH 20
#define BORDER 10

#define FONT_SIZE 14
#define TEXT_RECT_WIDTH 200
#define TEXT_RECT_HEIGHT 20
#define TEXT_BORDER 4
#define TEXT_MAX_LEN 30

#define START 0
#define PLAYING 1
#define PAUSE 2
#define GAMEOVER 3
#define EXIT 4

#define SNAKE_INIT_LENGTH 10
#define FOOD_MAX_COUNT 1
#define EAT_SCORE 10

#define UP 1
#define DOWN -1
#define LEFT 2
#define RIGHT -2
#endif