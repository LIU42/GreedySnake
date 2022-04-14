#include "snake.h"

using namespace std;

int main(int argc, char* argv[])
{
	game.init_game();
	game.init_window();
	game.init_font();
	game.load_image();
	game.start_main_interval();

	while (true)
	{
		game.events();
		SDL_Delay(USER_INPUT_DELAY);
	}
	return 0;
}