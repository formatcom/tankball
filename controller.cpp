#include <core/game.h>


void configController()
{
	Game::engine.setButton[0].push_back(
			SDL_CONTROLLER_BUTTON_DPAD_UP);

	Game::engine.setButton[1].push_back(
			SDL_CONTROLLER_BUTTON_DPAD_DOWN);

	Game::engine.setButton[2].push_back(
			SDL_CONTROLLER_BUTTON_DPAD_LEFT);

	Game::engine.setButton[3].push_back(
			SDL_CONTROLLER_BUTTON_DPAD_RIGHT);

}
