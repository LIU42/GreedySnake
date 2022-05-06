#include "snake.h"

using namespace std;

SDL_RWops* Window::get_resource(HINSTANCE hinst, LPCWSTR name, LPCWSTR type)
{
	HRSRC hrsrc = FindResource(hinst, name, type);
	DWORD size = SizeofResource(hinst, hrsrc);
	LPVOID data = LockResource(LoadResource(hinst, hrsrc));
	return SDL_RWFromConstMem(data, size);
}

SDL_Surface* Window::load_surface(DWORD ID)
{
	SDL_RWops* src = get_resource(hinstance, MAKEINTRESOURCE(ID), TEXT("PNG"));
	SDL_Surface* origin_image = IMG_LoadPNG_RW(src);
	SDL_Surface* convert_image = SDL_ConvertSurface(origin_image, format, NULL);
	SDL_FreeRW(src);
	SDL_FreeSurface(origin_image);
	return convert_image;
}

void Window::text(const char* text, int x, int y)
{
	SDL_Surface* text_surface = TTF_RenderText_Blended(font, text, { 0,0,0 });
	SDL_Rect text_rect = { x,y,TEXT_RECT_WIDTH,TEXT_RECT_HEIGHT };
	SDL_BlitSurface(text_surface, NULL, surface, &text_rect);
	SDL_FreeSurface(text_surface);
}

void Window::block(SDL_Surface* img, int x, int y)
{
	block_rect = { BORDER + BLOCK_WIDTH * x,BORDER + BLOCK_WIDTH * y,BLOCK_WIDTH,BLOCK_WIDTH };
	SDL_BlitSurface(img, NULL, surface, &block_rect);
}

void Window::init()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	hinstance = GetModuleHandle(0);
	window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	surface = SDL_GetWindowSurface(window);
	format = SDL_AllocFormat(IMG_FORMAT);
	screen_rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };
	keystatus = SDL_GetKeyboardState(NULL);
}

void Window::load_image()
{
	background = load_surface(IDB_PNG1);
	snake_body = load_surface(IDB_PNG2);
	food_red = load_surface(IDB_PNG3);
}

void Window::load_font()
{
	TTF_Init();
	font = TTF_OpenFontRW(get_resource(hinstance, MAKEINTRESOURCE(IDR_FONT1), RT_FONT), 1, FONT_SIZE);
}

void Window::delete_image()
{
	SDL_FreeSurface(background);
	SDL_FreeSurface(snake_body);
	SDL_FreeSurface(food_red);
}

void Window::delete_font() { TTF_CloseFont(font); }

void Window::close()
{
	SDL_DestroyWindow(window);
	game.delete_timer();
	delete_image();
	delete_font();
	SDL_Quit();
	exit(0);
}

Game::Game() : random((unsigned)(time(NULL))), rand_X(0, TABLE_X_MAX - 1), rand_Y(0, TABLE_Y_MAX - 1) {}

void Game::init()
{
	score = 0;
	status = START;
	snake.init();
	add_food(FOOD_MAX_COUNT);
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

Uint32 function_main_interval(Uint32 interval, void* param)
{
	game.update();
	game.display();
	return interval;
}

void Game::start_main_interval() { main_interval = SDL_AddTimer(GAME_INTERVAL, function_main_interval, NULL); }
void Game::delete_timer() { SDL_RemoveTimer(main_interval); }

void Game::update()
{
	if (status == PLAYING)
	{
		snake.crash();
		snake.move();
		snake.eat();
	}
}

void Game::event()
{
	if (status == PLAYING)
	{
		if (window.keystatus[SDL_SCANCODE_W]) { snake.head.next = UP; }
		if (window.keystatus[SDL_SCANCODE_S]) { snake.head.next = DOWN; }
		if (window.keystatus[SDL_SCANCODE_A]) { snake.head.next = LEFT; }
		if (window.keystatus[SDL_SCANCODE_D]) { snake.head.next = RIGHT; }
	}
	while (SDL_PollEvent(&window.event))
	{
		if (window.event.type == SDL_QUIT) { window.close(); }
		if (window.event.type == SDL_KEYDOWN && window.event.key.keysym.sym == SDLK_p && status == PLAYING) { status = PAUSE; }
		if (window.event.type == SDL_MOUSEBUTTONDOWN)
		{
			if (status == GAMEOVER)
			{
				food.clear();
				snake.init();
				init();
			}
			status = PLAYING;
		}
	}
}

void Game::display_info()
{
	char info[INFO_MAX_LEN];
	SDL_snprintf(info, INFO_MAX_LEN, "Length: %d", snake.body.size() + 1);
	window.text(info, SCREEN_WIDTH - 230, SCREEN_HEIGHT - (FONT_SIZE + TEXT_BORDER));
	SDL_snprintf(info, INFO_MAX_LEN, "Score: %d", score);
	window.text(info, SCREEN_WIDTH - 110, SCREEN_HEIGHT - (FONT_SIZE + TEXT_BORDER));

	switch (status)
	{
		case START: window.text("Click anywhere to START...", BORDER, SCREEN_HEIGHT - (FONT_SIZE + TEXT_BORDER)); break;
		case PAUSE: window.text("Click anywhere to resume...", BORDER, SCREEN_HEIGHT - (FONT_SIZE + TEXT_BORDER)); break;
		case GAMEOVER: window.text("GAMEOVER!", BORDER, SCREEN_HEIGHT - (FONT_SIZE + TEXT_BORDER)); break;
	}
}

void Game::display_food()
{
	for (int i = 0; i < food.size(); i++)
	{
		window.block(window.food_red, food[i].x, food[i].y);
	}
}

void Game::display()
{
	SDL_BlitSurface(window.background, NULL, window.surface, &window.screen_rect);
	display_info();
	display_food();
	snake.display();
	SDL_UpdateWindowSurface(window.window);
}

void Snake::init()
{
	head.x = TABLE_X_MAX / 2;
	head.y = TABLE_Y_MAX / 2;
	head.next = RIGHT;
	head.next_last = RIGHT;
	temp.x = 0;
	temp.y = 0;
	alive = true;
	body.clear();

	for (int i = 1; i < SNAKE_INIT_LENGTH; i++)
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

	if (front.x < 0 || front.x >= TABLE_X_MAX || front.y < 0 || front.y >= TABLE_Y_MAX) { crash = true; }

	for (int i = 0; i < body.size(); i++)
	{
		if (body[i].x == front .x && body[i].y == front.y) { crash = true; }
	}
	if (crash == true)
	{
		game.status = GAMEOVER;
		alive = false;
	}
}

void Snake::eat()
{
	if (alive == true)
	{
		for (int i = 0; i < food.size(); i++)
		{
			if (food[i].x == head.x && food[i].y == head.y)
			{
				food.erase(food.begin() + i);
				body.push_back(temp);
				game.add_food();
				game.score += EAT_SCORE;
			}
		}
	}
}

void Snake::display()
{
	for (int i = 0; i < body.size(); i++)
	{
		window.block(window.snake_body, body[i].x, body[i].y);
	}
	window.block(window.snake_body, head.x, head.y);
}