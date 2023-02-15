#include "game.h"

int main(int argc, char* argv[])
{
	MainGame game;

	srand((unsigned)time(NULL));

	game.initEnvironment();
	game.initWindow();
	game.initGame();
	game.loadImage();
	game.loadFont();
	game.startMainInterval();

	while (game.isRunning())
	{
		game.events();
		game.delay();
	}
	game.close();
	return 0;
}