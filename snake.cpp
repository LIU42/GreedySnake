#include "snake.h"

using namespace std;

SDL_RWops* Window::getResource(HINSTANCE hInst, LPCWSTR name, LPCWSTR type)
{
	HRSRC hRsrc = FindResource(hInst, name, type);
	DWORD size = SizeofResource(hInst, hRsrc);
	LPVOID data = LockResource(LoadResource(hInst, hRsrc));
	return SDL_RWFromConstMem(data, size);
}

SDL_Surface* Window::loadSurface(DWORD ID)
{
	SDL_RWops* src = getResource(hInstance, MAKEINTRESOURCE(ID), TEXT("PNG"));
	SDL_Surface* originImage = IMG_LoadPNG_RW(src);
	SDL_Surface* convertImage = SDL_ConvertSurface(originImage, format, NULL);
	SDL_FreeSurface(originImage);
	SDL_FreeRW(src);
	return convertImage;
}

void Window::text(const char* text, int x, int y)
{
	SDL_Surface* textSurface = TTF_RenderText_Blended(font, text, TEXT_COLOR);
	SDL_Rect textRect = { x, y, TEXT_RECT_WIDTH, TEXT_RECT_HEIGHT };
	SDL_BlitSurface(textSurface, NULL, surface, &textRect);
	SDL_FreeSurface(textSurface);
}

void Window::block(SDL_Surface* image, int x, int y)
{
	blockRect = { BORDER + BLOCK_WIDTH * x, BORDER + BLOCK_WIDTH * y, BLOCK_WIDTH, BLOCK_WIDTH };
	SDL_BlitSurface(image, NULL, surface, &blockRect);
}

void Window::init()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	hInstance = GetModuleHandle(0);
	window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	surface = SDL_GetWindowSurface(window);
	format = SDL_AllocFormat(IMG_FORMAT);
	screenRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	keyStatus = SDL_GetKeyboardState(NULL);
}

void Window::loadImage()
{
	backgroundImg = loadSurface(IDB_PNG1);
	snakeImg = loadSurface(IDB_PNG2);
	foodImg = loadSurface(IDB_PNG3);
}

void Window::loadFont()
{
	TTF_Init();
	font = TTF_OpenFontRW(getResource(hInstance, MAKEINTRESOURCE(IDR_FONT1), RT_FONT), 1, FONT_SIZE);
}

void Window::freeImage()
{
	SDL_FreeSurface(backgroundImg);
	SDL_FreeSurface(snakeImg);
	SDL_FreeSurface(foodImg);
}

void Window::freeFont() { TTF_CloseFont(font); }

void Window::close()
{
	SDL_DestroyWindow(window);
	SDL_FreeFormat(format);
	game.endMainInterval();
	freeImage();
	freeFont();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

Game::Game() : random((unsigned)(time(NULL))), randX(0, TABLE_X_MAX - 1), randY(0, TABLE_Y_MAX - 1) {}

void Game::init()
{
	score = 0;
	status = START;
	snake.init();
	addFood(FOOD_MAX_COUNT);
}

void Game::addFood(int add_food_count = 1)
{
	for (int i = 0; i < add_food_count; i++)
	{
		int x = randX(random);
		int y = randY(random);
		Body key = { x, y };

		if (count(snake.body.begin(), snake.body.end(), key) || count(food.begin(), food.end(), key) || snake.head == key) { i--; }
		else { food.push_back(key); }
	}
}

Uint32 mainIntervalFunction(Uint32 interval, void* param)
{
	game.update();
	game.display();
	return interval;
}

void Game::startMainInterval() { mainInterval = SDL_AddTimer(GAME_INTERVAL, mainIntervalFunction, NULL); }
void Game::endMainInterval() { SDL_RemoveTimer(mainInterval); }

void Game::update()
{
	if (status == PLAYING)
	{
		snake.crash();
		snake.move();
		snake.eat();
	}
}

void Game::events()
{
	if (status == PLAYING)
	{
		if (window.keyStatus[SDL_SCANCODE_W]) { snake.head.next = UP; }
		if (window.keyStatus[SDL_SCANCODE_S]) { snake.head.next = DOWN; }
		if (window.keyStatus[SDL_SCANCODE_A]) { snake.head.next = LEFT; }
		if (window.keyStatus[SDL_SCANCODE_D]) { snake.head.next = RIGHT; }
	}
	while (SDL_PollEvent(&window.events))
	{
		if (window.events.type == SDL_QUIT) { status = EXIT; }
		if (window.events.type == SDL_KEYDOWN && window.events.key.keysym.sym == SDLK_p && status == PLAYING) { status = PAUSE; }
		if (window.events.type == SDL_MOUSEBUTTONDOWN)
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

void Game::displayInfo()
{
	SDL_snprintf(text, TEXT_MAX_LEN, "Length: %d", snake.body.size() + 1);
	window.text(text, SCREEN_WIDTH - 230, SCREEN_HEIGHT - (FONT_SIZE + TEXT_BORDER));
	SDL_snprintf(text, TEXT_MAX_LEN, "Score: %d", score);
	window.text(text, SCREEN_WIDTH - 110, SCREEN_HEIGHT - (FONT_SIZE + TEXT_BORDER));

	switch (status)
	{
		case START: window.text("Click anywhere to START...", BORDER, SCREEN_HEIGHT - (FONT_SIZE + TEXT_BORDER)); break;
		case PAUSE: window.text("Click anywhere to resume...", BORDER, SCREEN_HEIGHT - (FONT_SIZE + TEXT_BORDER)); break;
		case GAMEOVER: window.text("GAMEOVER!", BORDER, SCREEN_HEIGHT - (FONT_SIZE + TEXT_BORDER)); break;
	}
}

void Game::displayFood()
{
	for (int i = 0; i < food.size(); i++)
	{
		window.block(window.foodImg, food[i].x, food[i].y);
	}
}

void Game::display()
{
	SDL_BlitSurface(window.backgroundImg, NULL, window.surface, &window.screenRect);
	displayInfo();
	displayFood();
	snake.display();
	SDL_UpdateWindowSurface(window.window);
}

void Snake::init()
{
	head.x = TABLE_X_MAX / 2;
	head.y = TABLE_Y_MAX / 2;
	head.next = RIGHT;
	head.nextLast = RIGHT;
	temp.x = 0;
	temp.y = 0;
	isAlive = true;
	body.clear();

	for (int i = 1; i < SNAKE_INIT_LENGTH; i++)
	{
		body.push_back({ head.x - i, head.y });
	}
}

void Snake::move()
{
	if (isAlive)
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
		head.nextLast = head.next;
	}
}

void Snake::crash()
{
	Position front = { head.x, head.y };
	bool isCrash = false;

	if (head.next == -head.nextLast) { head.next = head.nextLast; }

	switch (head.next)
	{
		case UP: front.y -= 1; break;
		case DOWN: front.y += 1; break;
		case LEFT: front.x -= 1; break;
		case RIGHT: front.x += 1; break;
	}

	if (front.x < 0 || front.x >= TABLE_X_MAX || front.y < 0 || front.y >= TABLE_Y_MAX) { isCrash = true; }

	for (int i = 0; i < body.size(); i++)
	{
		if (body[i].x == front .x && body[i].y == front.y) { isCrash = true; }
	}
	if (isCrash)
	{
		game.status = GAMEOVER;
		isAlive = false;
	}
}

void Snake::eat()
{
	if (isAlive)
	{
		for (int i = 0; i < food.size(); i++)
		{
			if (food[i].x == head.x && food[i].y == head.y)
			{
				food.erase(food.begin() + i);
				body.push_back(temp);
				game.addFood();
				game.score += EAT_SCORE;
			}
		}
	}
}

void Snake::display()
{
	for (int i = 0; i < body.size(); i++) { window.block(window.snakeImg, body[i].x, body[i].y); }
	window.block(window.snakeImg, head.x, head.y);
}