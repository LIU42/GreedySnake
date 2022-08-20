#include "snake.h"

MainGame game;

int main(int argc, char* argv[])
{
	srand((unsigned)time(NULL));

	game.initWindow();
	game.initGame();
	game.loadImage();
	game.loadFont();
	game.startMainInterval();

	while (game.status != EXIT)
	{
		game.events();
		SDL_Delay(CONTROL_DELAY);
	}
	game.close();
	return 0;
}