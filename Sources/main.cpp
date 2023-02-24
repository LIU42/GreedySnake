#include "game.h"

int main(int argc, char* argv[])
{
	MainGame game;

	while (game.isRunning())
	{
		game.events();
		game.delay();
	}
	return EXIT_SUCCESS;
}