#include "game.h"

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
	window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	keyStatus = SDL_GetKeyboardState(NULL);
	screen = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
}

void MainGame::loadImage()
{
	image.format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
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

Uint32 mainIntervalCallback(Uint32 interval, void* game)
{
	((MainGame*)game)->update();
	((MainGame*)game)->display();
	return interval;
}

void MainGame::startMainInterval()
{
	mainInterval = SDL_AddTimer(INTERVAL, mainIntervalCallback, this);
}

void MainGame::freeImage()
{
	SDL_FreeFormat(image.format);
	SDL_FreeSurface(image.background);
	SDL_FreeSurface(image.snake);
	SDL_FreeSurface(image.food);
}

void MainGame::freeFont()
{
	TTF_CloseFont(font);
}

void MainGame::endMainInterval()
{
	SDL_RemoveTimer(mainInterval);
}

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
	snake.init(TABLE_ROWS, TABLE_COLS);
	addFood();
}

bool MainGame::isRunning()
{
	return status != EXIT;
}

void MainGame::addFood()
{
	static Food foodTemp;

	while (true)
	{
		foodTemp.x = rand() % TABLE_ROWS;
		foodTemp.y = rand() % TABLE_COLS;

		if (!count(snake.getBodyBegin(), snake.getBodyEnd(), foodTemp) && !count(food.begin(), food.end(), foodTemp))
		{
			if (snake.getHeadX() != foodTemp.x && snake.getHeadY() != foodTemp.y)
			{
				food.push_back(foodTemp);
				break;
			}
		}
	}
}

void MainGame::snakeCrash()
{
	if (snake.getIsCrashed(TABLE_ROWS, TABLE_COLS))
	{
		status = OVER;
	}
}

void MainGame::snakeEat()
{
	for (auto it = food.begin(); it != food.end(); ++it)
	{
		if (it->x == snake.getHeadX() && it->y == snake.getHeadY())
		{
			snake.eat();
			food.erase(it);
			addFood();
			score += EAT_SCORE;
			break;
		}
	}
}

void MainGame::update()
{
	if (status == PLAYING)
	{
		snakeCrash();
		snakeEat();
		snake.move();
	}
}

void MainGame::events()
{
	if (status == PLAYING)
	{
		if (keyStatus[SDL_SCANCODE_W]) { snake.turnTo(UP); }
		if (keyStatus[SDL_SCANCODE_S]) { snake.turnTo(DOWN); }
		if (keyStatus[SDL_SCANCODE_A]) { snake.turnTo(LEFT); }
		if (keyStatus[SDL_SCANCODE_D]) { snake.turnTo(RIGHT); }
	}
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT) { status = EXIT; }
		if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_p && status == PLAYING) { status = PAUSE; }
		if (event.type == SDL_MOUSEBUTTONDOWN)
		{
			if (status == OVER)
			{
				food.clear();
				snake.init(TABLE_ROWS, TABLE_COLS);
				initGame();
			}
			status = PLAYING;
		}
	}
}

void MainGame::displayText(const char* text, int x, int y)
{
	static SDL_Surface* textSurface;
	static SDL_Rect textRect;

	textSurface = TTF_RenderText_Blended(font, text, { 0, 0, 0 });
	textRect.x = x;
	textRect.y = y;

	SDL_BlitSurface(textSurface, NULL, image.surface, &textRect);
	SDL_FreeSurface(textSurface);
}

void MainGame::displayBlock(SDL_Surface* blockImg, int x, int y)
{
	static SDL_Rect rect;

	rect.x = BORDER + BLOCK_SIZE * x;
	rect.y = BORDER + BLOCK_SIZE * y;

	SDL_BlitSurface(blockImg, NULL, image.surface, &rect);
}

void MainGame::displayInfo()
{
	static char text[TEXT_LENGTH];

	SDL_snprintf(text, TEXT_LENGTH, "Length: %d", snake.getLength());
	displayText(text, SCREEN_WIDTH - INFO_LENGTH_MARGIN, SCREEN_HEIGHT - (FONT_SIZE + TEXT_BORDER));
	SDL_snprintf(text, TEXT_LENGTH, "Score: %d", score);
	displayText(text, SCREEN_WIDTH - INFO_SCORE_MARGIN, SCREEN_HEIGHT - (FONT_SIZE + TEXT_BORDER));

	if (status != PLAYING)
	{
		switch (status)
		{
			case START: SDL_snprintf(text, TEXT_LENGTH, "Click anywhere to START..."); break;
			case PAUSE: SDL_snprintf(text, TEXT_LENGTH, "Click anywhere to RESUME..."); break;
			case OVER: SDL_snprintf(text, TEXT_LENGTH, "GAMEOVER!"); break;
		}
		displayText(text, BORDER, SCREEN_HEIGHT - (FONT_SIZE + TEXT_BORDER));
	}
}

void MainGame::displayFood()
{
	for (auto it = food.begin(); it != food.end(); ++it)
	{
		displayBlock(image.food, it->x, it->y);
	}
}

void MainGame::displaySnake()
{
	for (auto it = snake.getBodyBegin(); it != snake.getBodyEnd(); ++it)
	{
		displayBlock(image.snake, it->x, it->y);
	}
	displayBlock(image.snake, snake.getHeadX(), snake.getHeadY());
}

void MainGame::display()
{
	SDL_BlitSurface(image.background, NULL, image.surface, &screen);
	displayInfo();
	displayFood();
	displaySnake();
	SDL_UpdateWindowSurface(window);
}