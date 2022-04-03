#include "snake.h"
#include "config.h"
#include "resource.h"

using namespace std;

Game game;

SDL_RWops* get_resource(HINSTANCE hinst, LPCWSTR name, LPCWSTR type)
{
	HRSRC hrsrc = FindResource(hinst, name, type);
	DWORD size = SizeofResource(hinst, hrsrc);
	LPVOID data = LockResource(LoadResource(hinst, hrsrc));
	return SDL_RWFromConstMem(data, size);
}

SDL_Surface* load_surface(DWORD ID)
{
	SDL_RWops* src = get_resource(game.hinstance, MAKEINTRESOURCE(ID), TEXT("PNG"));
	SDL_Surface* origin_image = IMG_LoadPNG_RW(src);
	SDL_Surface* convert_image = SDL_ConvertSurface(origin_image, game.format, NULL);
	SDL_FreeRW(src);
	SDL_FreeSurface(origin_image);
	return convert_image;
}

Uint32 create_main_interval(Uint32 interval, void* param)
{
	game.update();
	game.display();
	return interval;
}

void display_block(SDL_Surface* img, int x, int y)
{
	game.block_rect = { border + block_width * x,border + block_width * y,block_width,block_width };
	SDL_BlitSurface(img, NULL, game.surface, &game.block_rect);
}

void display_text(const char* text, TTF_Font* type, int x, int y)
{
	SDL_Surface* text_surface = TTF_RenderText_Blended(type, text, { 0,0,0 });
	SDL_Rect text_rect = { x,y,text_rect_width,text_rect_height };
	SDL_BlitSurface(text_surface, NULL, game.surface, &text_rect);
	SDL_FreeSurface(text_surface);
}

void display_info()
{
	char info[30];
	sprintf_s(info, "Length: %d", game.snake.body.size() + 1);
	display_text(info, game.font, screen_width - 280, screen_height - (font_size + text_border));
	sprintf_s(info, "Score: %d", game.score);
	display_text(info, game.font, screen_width - 130, screen_height - (font_size + text_border));

	switch (game.status)
	{
		case start: display_text("Click anywhere to start...", game.font, border, screen_height - (font_size + text_border)); break;
		case pause: display_text("Click anywhere to resume...", game.font, border, screen_height - (font_size + text_border)); break;
		case gameover: display_text("Gameover!", game.font, border, screen_height - (font_size + text_border)); break;
	}
}

void display_food()
{
	for (int i = 0; i < game.food.size(); i++)
	{
		display_block(game.food_red, game.food[i].x, game.food[i].y);
	}
}

Game::Game() : random((unsigned)(time(NULL))), rand_X(0, table_x_max - 1), rand_Y(0, table_y_max - 1) {}

void Game::init_game()
{
	score = 0;
	status = start;
	keystatus = SDL_GetKeyboardState(NULL);
	add_food(food_max_count);
}

void Game::init_window()
{
	hinstance = GetModuleHandle(0);
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, SDL_WINDOW_SHOWN);
	surface = SDL_GetWindowSurface(window);
	format = SDL_AllocFormat(IMG_format);
	screen_rect = { 0,0,screen_width,screen_height };
}

void Game::load_image()
{
	background = load_surface(IDB_PNG1);
	snake_body = load_surface(IDB_PNG2);
	food_red = load_surface(IDB_PNG3);
}

void Game::init_font()
{
	TTF_Init();
	font = TTF_OpenFontRW(get_resource(hinstance, MAKEINTRESOURCE(IDR_FONT1), RT_FONT), 1, font_size);
}

void Game::start_main_interval()
{
	main_interval = SDL_AddTimer(game_interval, create_main_interval, NULL);
}

void Game::add_food(int add_food_count = 1)
{
	for (int i = 0; i < add_food_count; i++)
	{
		int x = rand_X(random);
		int y = rand_Y(random);
		Body key = { x,y };

		if (count(snake.body.begin(), snake.body.end(), key) || count(food.begin(), food.end(), key) || snake.head == key) { i--; }
		else { food.push_back(key); }
	}
}

void Game::exit_game()
{
	SDL_DestroyWindow(window);
	SDL_RemoveTimer(main_interval);
	TTF_CloseFont(font);
	SDL_FreeSurface(background);
	SDL_FreeSurface(snake_body);
	SDL_FreeSurface(food_red);
	SDL_Quit();
	exit(0);
}

void Game::update()
{
	if (status == playing)
	{
		snake.crash();
		snake.move();
		snake.eat();
	}
}

void Game::events()
{
	if (status == playing)
	{
		if (keystatus[SDL_SCANCODE_W]) { snake.head.next = UP; }
		if (keystatus[SDL_SCANCODE_S]) { snake.head.next = DOWN; }
		if (keystatus[SDL_SCANCODE_A]) { snake.head.next = LEFT; }
		if (keystatus[SDL_SCANCODE_D]) { snake.head.next = RIGHT; }
	}
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT) { exit_game(); }
		if (event.key.keysym.sym == SDLK_p && status == playing) { status = pause; }
		if (event.type == SDL_MOUSEBUTTONDOWN)
		{
			if (status == gameover)
			{
				food.clear();
				init_game();
				snake.init();
			}
			status = playing;
		}
	}
}

void Game::display()
{
	SDL_BlitSurface(background, NULL, surface, &screen_rect);
	display_info();
	display_food();
	snake.display();
	SDL_UpdateWindowSurface(window);
}

Snake::Snake() { init(); }
void Snake::init()
{
	head.x = table_x_max / 2;
	head.y = table_y_max / 2;
	head.next = RIGHT;
	head.next_last = RIGHT;
	temp.x = 0;
	temp.y = 0;
	alive = true;
	body.clear();

	for (int i = 1; i < snake_init_length; i++)
	{
		body.push_back({ head.x - i,head.y });
	}
}

void Snake::move()
{
	if (alive == true)
	{
		temp.x = head.x;
		temp.y = head.y;

		switch (head.next)
		{
			case UP: head.y -= 1; break;
			case DOWN: head.y += 1; break;
			case LEFT: head.x -= 1; break;
			case RIGHT: head.x += 1; break;
		}

		Body temp;
		for (int i = 0; i < body.size(); i++)
		{
			temp = Snake::temp;
			Snake::temp = body[i];
			body[i] = temp;
		}
		head.next_last = head.next;
	}
}

void Snake::crash()
{
	Position front = { head.x,head.y };
	bool crash = false;

	if (head.next == -head.next_last) { head.next = head.next_last; }

	switch (head.next)
	{
		case UP: front.y -= 1; break;
		case DOWN: front.y += 1; break;
		case LEFT: front.x -= 1; break;
		case RIGHT: front.x += 1; break;
	}

	if (front.x < 0 || front.x >= table_x_max || front.y < 0 || front.y >= table_y_max) { crash = true; }

	for (int i = 0; i < body.size(); i++)
	{
		if (body[i].x == front .x && body[i].y == front.y) { crash = true; }
	}
	if (crash == true)
	{
		game.status = gameover;
		alive = false;
	}
}

void Snake::eat()
{
	if (alive == true)
	{
		for (int i = 0; i < game.food.size(); i++)
		{
			if (game.food[i].x == head.x && game.food[i].y == head.y)
			{
				game.food.erase(game.food.begin() + i);
				body.push_back(temp);
				game.add_food();
				game.score += eat_score;
			}
		}
	}
}

void Snake::display()
{
	for (int i = 0; i < body.size(); i++)
	{
		display_block(game.snake_body, body[i].x, body[i].y);
	}
	display_block(game.snake_body, head.x, head.y);
}