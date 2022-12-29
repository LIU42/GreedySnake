#include "game.h"

int main(int argc, char* argv[])
{
	MainGame game;

	srand((unsigned)time(NULL));

	game.initWindow();
	game.initGame();
	game.loadImage();
	game.loadFont();
	game.startMainInterval();

	while (game.isRunning())
	{
		game.events();
		SDL_Delay(game.CONTROL_DELAY);
	}
	game.close();
	return 0;
}