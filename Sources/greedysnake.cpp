#include "greedysnake.h"

SDL_RWops* MainGame::getResource(LPCWSTR name, LPCWSTR type)
{
	HINSTANCE hInst = windowInfo.info.win.hinstance;
	HRSRC hRsrc = FindResource(hInst, name, type);
	DWORD size = SizeofResource(hInst, hRsrc);
	HGLOBAL hGlobal = LoadResource(hInst, hRsrc);
	LPVOID data = LockResource(hGlobal);
	return SDL_RWFromConstMem(data, size);
}

SDL_Surface* MainGame::loadSurface(Uint32 resourceID)
{
	SDL_RWops* pResource = getResource(MAKEINTRESOURCE(resourceID), TEXT("PNG"));
	SDL_Surface* pOriginalSurface = IMG_LoadPNG_RW(pResource);
	SDL_Surface* pConvertedSurface = SDL_ConvertSurface(pOriginalSurface, pFormat, NULL);
	SDL_FreeSurface(pOriginalSurface);
	SDL_FreeRW(pResource);
	return pConvertedSurface;
}

void MainGame::getVersion()
{
	windowInfo.version.major = SDL_MAJOR_VERSION;
	windowInfo.version.minor = SDL_MINOR_VERSION;
	windowInfo.version.patch = SDL_PATCHLEVEL;
}

void MainGame::initSystem()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
}

void MainGame::initWindow()
{
	pWindow = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	pSurface = SDL_GetWindowSurface(pWindow);
	pFormat = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
	pKeyStatus = SDL_GetKeyboardState(NULL);
	screenRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	SDL_GetWindowWMInfo(pWindow, &windowInfo);
}

void MainGame::loadImage()
{
	images.pBackground = loadSurface(IDB_PNG1);
	images.pSnake = loadSurface(IDB_PNG2);
	images.pFood = loadSurface(IDB_PNG3);
}

void MainGame::loadFont()
{
	fonts.pInfo = TTF_OpenFontRW(getResource(MAKEINTRESOURCE(IDR_FONT1), RT_FONT), true, FONT_SIZE);
}

Uint32 mainIntervalCallback(Uint32 interval, void* pParam)
{
	MainGame* pGame = (MainGame*)pParam;
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
	SDL_FreeSurface(images.pBackground);
	SDL_FreeSurface(images.pSnake);
	SDL_FreeSurface(images.pFood);
}

void MainGame::freeFont()
{
	TTF_CloseFont(fonts.pInfo);
}

void MainGame::endMainInterval()
{
	SDL_RemoveTimer(timers.mainInterval);
}

void MainGame::closeWindow()
{
	SDL_DestroyWindow(pWindow);
	SDL_FreeFormat(pFormat);
}

void MainGame::closeSystem()
{
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

void MainGame::addFood()
{
	static Food foodTemp;

	while (true)
	{
		foodTemp.x = rand() % TABLE_ROWS;
		foodTemp.y = rand() % TABLE_COLS;

		if (count(snake.getBodyBegin(), snake.getBodyEnd(), foodTemp) || count(foodList.begin(), foodList.end(), foodTemp))
		{
			continue;
		}
		if (snake.getHeadX() != foodTemp.x && snake.getHeadY() != foodTemp.y)
		{
			foodList.push_back(foodTemp);
			break;
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
	for (auto bodyIter = foodList.begin(); bodyIter != foodList.end(); ++bodyIter)
	{
		if (bodyIter->x == snake.getHeadX() && bodyIter->y == snake.getHeadY())
		{
			snake.eat();
			foodList.erase(bodyIter);
			addFood();
			score += EAT_SCORE;
			break;
		}
	}
}

void MainGame::displayText(const char* pText, int x, int y)
{
	SDL_Surface* pTextSurface = TTF_RenderText_Blended(fonts.pInfo, pText, BLACK);
	SDL_Rect textRect = { x, y, 0, 0 };

	SDL_BlitSurface(pTextSurface, NULL, pSurface, &textRect);
	SDL_FreeSurface(pSurface);
}

void MainGame::displayBlock(SDL_Surface* pBlockSurface, int x, int y)
{
	SDL_Rect blockRect = { 0, 0, BLOCK_SIZE, BLOCK_SIZE };

	blockRect.x = BORDER + BLOCK_SIZE * x;
	blockRect.y = BORDER + BLOCK_SIZE * y;

	SDL_BlitSurface(pBlockSurface, NULL, pSurface, &blockRect);
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
	for (auto foodIter = foodList.begin(); foodIter != foodList.end(); ++foodIter)
	{
		displayBlock(images.pFood, foodIter->x, foodIter->y);
	}
}

void MainGame::displaySnake()
{
	for (auto bodyIter = snake.getBodyBegin(); bodyIter != snake.getBodyEnd(); ++bodyIter)
	{
		displayBlock(images.pSnake, bodyIter->x, bodyIter->y);
	}
	displayBlock(images.pSnake, snake.getHeadX(), snake.getHeadY());
}


MainGame::MainGame()
{
	srand((unsigned)time(NULL));
	getVersion();
	initSystem();
	initWindow();
	initGame();
	loadImage();
	loadFont();
	startMainInterval();
}

MainGame::~MainGame()
{
	endMainInterval();
	freeImage();
	freeFont();
	closeWindow();
	closeSystem();
}

bool MainGame::isRunning()
{
	return status != EXIT;
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
		if (pKeyStatus[SDL_SCANCODE_W]) { snake.turnTo(UP); }
		if (pKeyStatus[SDL_SCANCODE_S]) { snake.turnTo(DOWN); }
		if (pKeyStatus[SDL_SCANCODE_A]) { snake.turnTo(LEFT); }
		if (pKeyStatus[SDL_SCANCODE_D]) { snake.turnTo(RIGHT); }
	}
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			status = EXIT;
		}
		if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_p && status == PLAYING)
		{
			status = PAUSE;
		}
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

void MainGame::display()
{
	SDL_BlitSurface(images.pBackground, NULL, pSurface, &screenRect);
	displayInfo();
	displayFood();
	displaySnake();
	SDL_UpdateWindowSurface(pWindow);
}

void MainGame::delay()
{
	SDL_Delay(CONTROL_DELAY);
}