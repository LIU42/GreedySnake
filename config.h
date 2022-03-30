#ifndef __CONFIG_H__
#define __CONFIG_H__

#define window_title "Snake"
#define screen_width 720
#define screen_height 460
#define game_interval 250
#define user_input_delay 10
#define IMG_format SDL_PIXELFORMAT_RGBA32

#define table_width 700
#define table_height 420
#define table_x_max 50
#define table_y_max 30
#define block_width 14
#define border 10

#define font_size 16
#define text_rect_width 200
#define text_rect_height 20
#define text_border 4

#define start 0
#define playing 1
#define pause 2
#define gameover 3

#define snake_init_length 10
#define food_max_count 1
#define eat_score 10

#define UP 1
#define DOWN -1
#define LEFT 2
#define RIGHT -2
#endif