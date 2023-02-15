#include "game.h"

SDL_RWops* MainGame::getResource(LPCWSTR name, LPCWSTR type)
{
	HINSTANCE hInst = windowInfo.info.win.hinstance;
	HRSRC hRsrc = FindResource(hInst, name, type);
	DWORD size = SizeofResource(hInst, hRsrc);
	HGLOBAL hGlobal = LoadResource(hInst, hRsrc);
	LPVOID data = LockResource(hGlobal);
	return SDL_RWFromConstMem(data, size);
}

SDL_Surface* MainGame::loadSurface(Uint32 id)
{
	SDL_RWops* pResource = getResource(MAKEINTRESOURCE(id), TEXT("PNG"));
	SDL_Surface* originalSurface = IMG_LoadPNG_RW(pResource);
	SDL_Surface* convertedSurface = SDL_ConvertSurface(originalSurface, format, NULL);
	SDL_FreeSurface(originalSurface);
	SDL_FreeRW(pResource);
	return convertedSurface;
}

void MainGame::initEnvironment()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	SDL_VERSION(&windowInfo.version);
}

void MainGame::initWindow()
{
	window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	surface = SDL_GetWindowSurface(window);
	format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
	keyStatus = SDL_GetKeyboardState(NULL);
	screenRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	SDL_GetWindowWMInfo(window, &windowInfo);
}

void MainGame::loadImage()
{
	images.background = loadSurface(IDB_PNG1);
	images.snake = loadSurface(IDB_PNG2);
	images.food = loadSurface(IDB_PNG3);
}

void MainGame::loadFont()
{
	fonts.info = TTF_OpenFontRW(getResource(MAKEINTRESOURCE(IDR_FONT1), RT_FONT), true, FONT_SIZE);
}

Uint32 MainGame::mainIntervalCallback(Uint32 interval, void* param)
{
	MainGame* pGame = (MainGame*)param;
	pGame->update();
	pGame->display();
	return interval;
}

void MainGame::startMainInterval()
{
	timers.mainInterval = SDL_AddTimer(INTERVAL, mainIntervalCallback, this);
}

void MainGame::freeImage()
{
	SDL_FreeSurface(images.background);
	SDL_FreeSurface(images.snake);
	SDL_FreeSurface(images.food);
}

void MainGame::freeFont()
{
	TTF_CloseFont(fonts.info);
}

void MainGame::endMainInterval()
{
	SDL_RemoveTimer(timers.mainInterval);
}

void MainGame::closeWindow()
{
	SDL_DestroyWindow(window);
	SDL_FreeFormat(format);
}

void MainGame::closeEnvironment()
{
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void MainGame::close()
{
	endMainInterval();
	freeImage();
	freeFont();
	closeWindow();
	closeEnvironment();
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

		if (!count(snake.getBodyBegin(), snake.getBodyEnd(), foodTemp) && !count(foodList.begin(), foodList.end(), foodTemp))
		{
			if (snake.getHeadX() != foodTemp.x && snake.getHeadY() != foodTemp.y)
			{
				foodList.push_back(foodTemp);
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
	for (auto it = foodList.begin(); it != foodList.end(); ++it)
	{
		if (it->x == snake.getHeadX() && it->y == snake.getHeadY())
		{
			snake.eat();
			foodList.erase(it);
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
				foodList.clear();
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

	textSurface = TTF_RenderText_Blended(fonts.info, text, BLACK);
	textRect.x = x;
	textRect.y = y;

	SDL_BlitSurface(textSurface, NULL, surface, &textRect);
	SDL_FreeSurface(surface);
}

void MainGame::displayBlock(SDL_Surface* blockImg, int x, int y)
{
	static SDL_Rect blockRect;

	blockRect.x = BORDER + BLOCK_SIZE * x;
	blockRect.y = BORDER + BLOCK_SIZE * y;

	SDL_BlitSurface(blockImg, NULL, surface, &blockRect);
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
	for (auto it = foodList.begin(); it != foodList.end(); ++it)
	{
		displayBlock(images.food, it->x, it->y);
	}
}

void MainGame::displaySnake()
{
	for (auto it = snake.getBodyBegin(); it != snake.getBodyEnd(); ++it)
	{
		displayBlock(images.snake, it->x, it->y);
	}
	displayBlock(images.snake, snake.getHeadX(), snake.getHeadY());
}

void MainGame::display()
{
	SDL_BlitSurface(images.background, NULL, surface, &screenRect);
	displayInfo();
	displayFood();
	displaySnake();
	SDL_UpdateWindowSurface(window);
}

void MainGame::delay()
{
	SDL_Delay(CONTROL_DELAY);
}