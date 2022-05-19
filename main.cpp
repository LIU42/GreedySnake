#include "snake.h"

using namespace std;

Window window;
Game game;
Snake snake;
vector <Food> food;

int main(int argc, char* argv[])
{
	window.init();
	window.load_image();
	window.load_font();

	game.init();
	game.start_main_interval();

	while (game.status != EXIT)
	{
		game.events();
		SDL_Delay(USER_INPUT_DELAY);
	}
	window.close();
	return 0;
}