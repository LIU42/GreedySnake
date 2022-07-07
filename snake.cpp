#include "snake.h"

using namespace std;

SDL_RWops* MainGame::getResource(HINSTANCE hInst, LPCWSTR name, LPCWSTR type)
{
	HRSRC hRsrc = FindResource(hInst, name, type);
	DWORD size = SizeofResource(hInst, hRsrc);
	LPVOID data = LockResource(LoadResource(hInst, hRsrc));
	return SDL_RWFromConstMem(data, size);
}

SDL_Surface* MainGame::loadSurface(int id)
{
	SDL_RWops* src = getResource(hInstance, MAKEINTRESOURCE(id), TEXT("PNG"));
	SDL_Surface* originSurface = IMG_LoadPNG_RW(src);
	SDL_Surface* convertSurface = SDL_ConvertSurface(originSurface, image.format, NULL);
	SDL_FreeSurface(originSurface);
	SDL_FreeRW(src);
	return convertSurface;
}

void MainGame::initWindow()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	hInstance = GetModuleHandle(0);
	window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	keyStatus = SDL_GetKeyboardState(NULL);
	SDL_GetWindowSize(window, &rect.screen.w, &rect.screen.h);
}

void MainGame::loadImage()
{
	image.format = SDL_AllocFormat(IMG_FORMAT);
	image.surface = SDL_GetWindowSurface(window);
	image.background = loadSurface(IDB_PNG1);
	image.snake = loadSurface(IDB_PNG2);
	image.food = loadSurface(IDB_PNG3);
}

void MainGame::loadFont()
{
	TTF_Init();
	font = TTF_OpenFontRW(getResource(hInstance, MAKEINTRESOURCE(IDR_FONT1), RT_FONT), 1, FONT_SIZE);
}

Uint32 mainIntervalCallback(Uint32 interval, void* param)
{
	game.update();
	game.display();
	return interval;
}

void MainGame::startMainInterval() { mainInterval = SDL_AddTimer(GAME_INTERVAL, mainIntervalCallback, NULL); }

void MainGame::freeImage()
{
	SDL_FreeFormat(image.format);
	SDL_FreeSurface(image.background);
	SDL_FreeSurface(image.snake);
	SDL_FreeSurface(image.food);
}

void MainGame::freeFont() { TTF_CloseFont(font); }

void MainGame::endMainInterval() { SDL_RemoveTimer(mainInterval); }

void MainGame::close()
{
	SDL_DestroyWindow(window);
	endMainInterval();
	freeImage();
	freeFont();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void MainGame::initGame()
{
	score = 0;
	status = START;
	snake.init();
	addFood();
}

void MainGame::addFood()
{
	while (true)
	{
		int x = rand() % TABLE_COLS;
		int y = rand() % TABLE_ROWS;
		Point point = { x, y };

		if (!count(snake.body.begin(), snake.body.end(), point) && !count(food.begin(), food.end(), point) && !(snake.head == point))
		{
			food.push_back(point);
			break;
		}
	}
}

void MainGame::update()
{
	if (status == PLAYING)
	{
		snake.crash();
		snake.move();
		snake.eat();
	}
}

void MainGame::control()
{
	if (status == PLAYING)
	{
		if (keyStatus[SDL_SCANCODE_W]) { snake.head.next = UP; }
		if (keyStatus[SDL_SCANCODE_S]) { snake.head.next = DOWN; }
		if (keyStatus[SDL_SCANCODE_A]) { snake.head.next = LEFT; }
		if (keyStatus[SDL_SCANCODE_D]) { snake.head.next = RIGHT; }
	}
	while (SDL_PollEvent(&events))
	{
		if (events.type == SDL_QUIT) { status = EXIT; }
		if (events.type == SDL_KEYDOWN && events.key.keysym.sym == SDLK_p && status == PLAYING) { status = PAUSE; }
		if (events.type == SDL_MOUSEBUTTONDOWN)
		{
			if (status == GAMEOVER)
			{
				food.clear();
				snake.init();
				initGame();
			}
			status = PLAYING;
		}
	}
}

void MainGame::displayText(const char* text, int x, int y)
{
	SDL_Surface* textSurface = TTF_RenderText_Blended(font, text, TEXT_COLOR);
	SDL_Rect textRect = { x, y, TEXT_RECT_WIDTH, TEXT_RECT_HEIGHT };
	SDL_BlitSurface(textSurface, NULL, image.surface, &textRect);
	SDL_FreeSurface(textSurface);
}

void MainGame::displayBlock(SDL_Surface* blockImg, int x, int y)
{
	rect.block = { BORDER + BLOCK_WIDTH * x, BORDER + BLOCK_WIDTH * y, BLOCK_WIDTH, BLOCK_WIDTH };
	SDL_BlitSurface(blockImg, NULL, image.surface, &rect.block);
}

void MainGame::displayInfo()
{
	static char text[TEXT_MAX_LEN];

	SDL_snprintf(text, TEXT_MAX_LEN, "Length: %d", snake.body.size() + 1);
	displayText(text, SCREEN_WIDTH - 230, SCREEN_HEIGHT - (FONT_SIZE + TEXT_BORDER));
	SDL_snprintf(text, TEXT_MAX_LEN, "Score: %d", score);
	displayText(text, SCREEN_WIDTH - 110, SCREEN_HEIGHT - (FONT_SIZE + TEXT_BORDER));

	switch (status)
	{
		case START: displayText("Click anywhere to START...", BORDER, SCREEN_HEIGHT - (FONT_SIZE + TEXT_BORDER)); break;
		case PAUSE: displayText("Click anywhere to RESUME...", BORDER, SCREEN_HEIGHT - (FONT_SIZE + TEXT_BORDER)); break;
		case GAMEOVER: displayText("GAMEOVER!", BORDER, SCREEN_HEIGHT - (FONT_SIZE + TEXT_BORDER)); break;
	}
}

void MainGame::displayFood()
{
	for (int i = 0; i < food.size(); i++)
	{
		displayBlock(image.food, food[i].x, food[i].y);
	}
}

void MainGame::display()
{
	SDL_BlitSurface(image.background, NULL, image.surface, &rect.screen);
	displayInfo();
	displayFood();
	snake.display();
	SDL_UpdateWindowSurface(window);
}

void Snake::init()
{
	head.x = TABLE_COLS / 2;
	head.y = TABLE_ROWS / 2;
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
		body.insert(body.begin(), temp);
		body.pop_back();
		head.nextLast = head.next;
	}
}

void Snake::crash()
{
	Point front = { head.x, head.y };
	bool isCrash = false;

	if (head.next == -head.nextLast) { head.next = head.nextLast; }

	switch (head.next)
	{
		case UP: front.y -= 1; break;
		case DOWN: front.y += 1; break;
		case LEFT: front.x -= 1; break;
		case RIGHT: front.x += 1; break;
	}

	if (front.x < 0 || front.x >= TABLE_COLS || front.y < 0 || front.y >= TABLE_ROWS) { isCrash = true; }

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
		for (int i = 0; i < game.food.size(); i++)
		{
			if (game.food[i].x == head.x && game.food[i].y == head.y)
			{
				game.food.erase(game.food.begin() + i);
				body.push_back(temp);
				game.addFood();
				game.score += EAT_SCORE;
			}
		}
	}
}

void Snake::display()
{
	for (int i = 0; i < body.size(); i++) { game.displayBlock(game.image.snake, body[i].x, body[i].y); }
	game.displayBlock(game.image.snake, head.x, head.y);
}