#include "header.h"

int main(int argc, char** argv) {
	Game* game = Game::get_instance();
	if (game->init()) {
		while (!game->is_quit_game())
			game->run();
		game->exit();
	}

	return 0;
}