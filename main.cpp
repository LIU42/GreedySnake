#include "snake.h"

using namespace std;

Window window;
Game game;
Snake snake;
vector <Food> food;

int main(int argc, char* argv[])
{
	window.init();
	window.loadImage();
	window.loadFont();

	game.init();
	game.startMainInterval();

	while (game.status != EXIT)
	{
		game.events();
		SDL_Delay(USER_INPUT_DELAY);
	}
	window.close();
	return 0;
}